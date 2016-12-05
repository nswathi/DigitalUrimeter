#include "loadcell.h"
#include "application.h"
#include "SparkIntervalTimer.h"
#include "urimeterEEprom.h"

int scale_zero = 0x80d1c0;
int per_g = 215.457;
float tempComp = 51.77;
float temp_zero = 70.0;

IntervalTimer getData;

void loadCellInit() {
    pinMode(D7, OUTPUT);
    
    pinMode(loadcellCLK, OUTPUT); //clk input for load cell amp
    digitalWrite(loadcellCLK, LOW);
    pinMode(loadcellDAT, INPUT); //data pin from load cell amp
    
    
    
    pinMode(tempSensor, INPUT); //temperature sensor analog input
    
    //Tare
    readScale();
    readScale();
    
    //getData.begin(_ISRgetData, 2000, hmSec, TIMER4);
    
    Serial.begin(9600);
}

int readScale() {
    int value = 0;
	while (digitalRead(loadcellDAT));
	for (int c = 0; c < 24; c++) {
		digitalWrite(loadcellCLK, HIGH);
		value = value << 1;
		digitalWrite(loadcellCLK, LOW);
		if (digitalRead(loadcellDAT)) value++;
	}
	for (int c = 0; c < 3; c++) {
		digitalWrite(loadcellCLK, HIGH);
		for (int l = 0; l < 5; l++) ;
		digitalWrite(loadcellCLK, LOW);
	}
	value = value ^ 0x800000;
	return value;
}

void tare() {
    int avg = 0;
	float temp_z = 0;
	for (int c = 0; c < 5; c++) {
		avg += readScale();
		temp_z += analogRead(tempSensor);
	}
	avg /= 5.0;
	temp_z /= 5.0;
	temp_z *= 3.3/4095.0;
	temp_zero = temp_z;
	setZero(avg);
}

//for data acquisition
void _ISRgetData() {
    static int tot = 0;
	static float v = 0;
	static char ovf = 0;
	digitalWrite(D7, !digitalRead(D7));
	Serial.println("hh");
	char n = 2;
	
	ovf++;
	
	int s = readScale();
	float adc = analogRead(tempSensor)/4095.0*3.3;
	
	tot += s;// & 0x7fff;
	v += adc;
	if (ovf >= n) {
		tot /= n*1.0;
		v /= n*1.0;
		
		float k = convert(tot);
		
		if (0.0 > k) {
			k *= -1.0;
			Serial.print("-");
		}
		Serial.print(k, 2);
		Serial.print(" ");
		Serial.print(tot, HEX);
		Serial.print(" ");
		Serial.print(v, 3);
		
		tot = 0;
		v = 0;
		ovf = 0;
	}
}

float convert(int raw) {
	float temp = analogRead(tempSensor)/4095.0*3.3*100;
	//temp compensation
	//raw += tempComp*(temp - temp_z);
	return (raw - (float)scale_zero)/per_g;
}

void setZero(int value) {
    scale_zero = value;
    EEPROM.put(ZERO_START, scale_zero);
    EEPROM.put(CHECKSUM_START, scale_zero ^ per_g);
}

void setPerG(int value) {
    per_g = value;
    EEPROM.put(PER_G_START, per_g);
    EEPROM.put(CHECKSUM_START, scale_zero ^ per_g);
}

int getZero() {return scale_zero;}
int getPerG() {return per_g;}