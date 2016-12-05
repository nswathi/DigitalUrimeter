// This #include statement was automatically added by the Particle IDE.
#include "urimeterEEprom.h"

// This #include statement was automatically added by the Particle IDE.
#include "patient.h"


// This #include statement was automatically added by the Particle IDE.
#include "loadcell.h"

// This #include statement was automatically added by the Particle IDE.
#include "LCD_UI.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_MCP23008.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_LiquidCrystal.h"

// This #include statement was automatically added by the Particle IDE.
#include "LCD_UI.h"

// This #include statement was automatically added by the Particle IDE.
#include "SparkIntervalTimer.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_MCP23008.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_LiquidCrystal.h"

// This #include statement was automatically added by the Particle IDE.
#include "loadcell.h"
#include "application.h"

#define TIME_ZONE -5
#define RESET_BUTTON D6
#define CalibrateButton D4
#define connectionLED D7

SYSTEM_MODE(SEMI_AUTOMATIC);

IntervalTimer updateDisplay;

float prevReading = 0;
float reading = 0;
int quantity = 0;
int lastReadTime = 0;

TCPClient client;
byte server[] = { 10,251,92,211 };                          // to the queen(one as a server photon)  wlan IP
int PORT = 8001;
int prevPostTime = 0;
bool connected = false;

identity patient;

uint32_t lastConnect = 0;
uint32_t lastSync = 0;

char BEGIN = 1;
bool movement = false;

void startCollection();

void setup() {
    //connnect to wifi
	Particle.connect();
	lastConnect = millis();
	
	pinMode(connectionLED, OUTPUT);
    pinMode(RESET_BUTTON, INPUT_PULLUP); //reset button
    attachInterrupt(RESET_BUTTON, resetEEprom, FALLING); //assign function for button press
    pinMode(CalibrateButton, INPUT_PULLUP); //calibrate button
    attachInterrupt(CalibrateButton, tare, FALLING); //make isr for button press
    //connect to server
    if (client.connect(server, PORT)) {
        digitalWrite(connectionLED, HIGH);
		Serial.println("Connected");
		client.println(patient.name);                // send patient name and EMR number at the time of handshake
		} else {
		Serial.println("connection failed");                    // if you didn't get a connection to the server:
	}
	//set timezone for time
    Time.zone(TIME_ZONE);
    
    //initiate patient and UI
	patientInit();
	UI_init();
	
	//initiate load cell peripherals
	loadCellInit();
	
	//read settings from EEprom if available, else save fresh settings
	if (!readSettings()) {
	    //startCollection
	    saveSettings();
	}
	
	// Make sure your Serial Terminal app is closed before powering your device
	Serial.begin(9600);
	
	delay(1000);
	
	//update display every half second
	updateDisplay.begin(refresh, 2000, hmSec, TIMER4);
	reading = convert(readScale());
    quantity = reading - prevReading;
	lastReadTime = millis();
}

void loop() {
    if (BEGIN) {
        patient = getIdentity();
        if (client.connected()) {
            digitalWrite(connectionLED, HIGH);
            connected = true;
            if ((unsigned long) (millis() - prevPostTime) > 1000*60) {
                char str[50];
    		    sprintf(str, "%d-%d-%d:%d:%d:", Time.year(),Time.month(),Time.day(),Time.hour(), (int)reading);
    		    strcat(str, patient.name);
    		    char id[16];
    		    sprintf(id, "_%u", patient.id);
    		    strcat(str, id);
    		    Serial.println(str);
    		    client.print(str);
    		    newMeasurement();
    		    prevReading = reading;
    		    prevPostTime = millis();
            }
            //upload stored data if any
            if (hasMeasurement()) {
                while (hasMeasurement() && client.connected()) {
                    measurement m;
                    m = popMeasurement();
                    char str[50];
    		        sprintf(str, "%d-%d-%d:%d:%d:", Time.year(),Time.month(),Time.day(),m.hour, m.mL);
    		        strcat(str, patient.name);
    		        char id[16];
    		        sprintf(id, "_%u", patient.id);
    		        strcat(str, id);
    		        client.print(str);
                }
            }
        } 
        else {
            digitalWrite(connectionLED, LOW);
            //store measurement if it is time to post and no server connection
            if ((unsigned long) (millis() - prevPostTime) > 1000*10) {
                measurement m;
                m.hour = Time.hour();
                m.min = Time.minute();
                m.mL = (int) quantity;
                storeMeasurement(m);
                newMeasurement();
                prevReading = reading;
                prevPostTime = millis();
            }
            connected = false;
            client.connect(server, PORT);
    		if (client.connect(server, PORT)) {
    		    connected = true;
    			Serial.println("connected");
    			char p[32];
    			p[0] = '/0';
    			strcat(p, patient.name);
    			char id[16];
    			sprintf(id, "_%u", patient.id);
    			strcat(p, id);
    			client.println(p);            // send patient name and EMR number at the time of handshake
    			} else {
    			Serial.println("connection failed");                // if you didn't get a connection to the server:
    			client.println("connection failed");
    		}
        }
    }
	
	
	//try to connect every 10 sec if not connected
	if (!Particle.connected() && ((unsigned long) (millis() - lastConnect) > 1000*10)) {
	    Particle.connect();
	    lastConnect = millis();    
	}
	
	if ((unsigned long) (millis() - lastSync) > 24*60*60*1000) {
		// Request time synchronization from the Particle Cloud
		Particle.syncTime();
		lastSync = millis();
	}
	
	
	delay(2000);
}

unsigned int lastMovement;

void refresh() {
    float minReading = 5000;
    float maxReading = -5000;
    for (int i = 0; i < 3; i++) {
        reading = convert(readScale());
        if (reading < minReading) minReading = reading;
        if (reading > maxReading) maxReading = reading;
    }
    if (maxReading - minReading > 1.0) {
        movement = true;
        //UI_notification(MOTION_NOTIFICATION);
        lastMovement = millis();
    }
    else if ((unsigned int) (millis() - lastMovement) > 2) movement = false;
    reading = convert(readScale());
    quantity = reading - prevReading;
    if (reading < 0.0) reading = 0.0;
    if ((quantity) < 1) quantity = 0;
    updateMeasurement(reading);
    UI_refresh();
}

void startCollection() {
    tare();
    BEGIN = 1;
    UI_setMode(1);
}
