#include "LCD_UI.h"
#include "Adafruit_LiquidCrystal.h"
#include "patient.h"


Adafruit_LiquidCrystal LCD(0);
char s[16];

char * notification;
void printTime(int h, int m);
void printmL(int mL);
void printDate(int m, int d);

//keeps track of last toggle for cooldown so it doesn't toggle multiple times a press
int lastToggle = 0;

int UI_state = 0;
int UI_previousState = 0;

void UI_setMode(int m) { UI_state = m;}

void UI_init() {
    UI_state = 0;
    LCD.begin(16,2);
    LCD.noCursor();
    pinMode(UI_toggleButton, INPUT_PULLUP); //UI toggle button
    attachInterrupt(UI_toggleButton, UI_toggle, FALLING); //assign function for button press
    LCD.clear();
    delay(50);
    UI_refresh();
}

//toggles UI between patient info and urine output
void UI_toggle() {
    if (millis() - lastToggle > 300) {
        UI_state++;
        if (UI_state > 1) UI_state = 0;
        lastToggle = millis();
    }
}

//refresh current screen
void UI_refresh() {
    if (UI_state != UI_previousState) {
        LCD.clear();
    }
    UI_previousState = UI_state;
    switch(UI_state) {
        //urine output
        case 0: {
            LCD.setCursor(0,0);
            LCD.print2("OLD");
            LCD.setCursor(0,1);
            LCD.print2("NOW");
            
            measurement o;
            measurement c;
            o = getOldMeasurement();
            c = getCurrentMeasurement();
            
            LCD.setCursor(4,0);
            printTime(o.hour, o.min);
            LCD.setCursor(10,0);
            printmL(o.mL);
            
            LCD.setCursor(4,1);
            printTime(c.hour, c.min);
            LCD.setCursor(10,1);
            printmL(c.mL);
            break;
        }
        //patient info
        case 1: {
            identity p;
            p = getIdentity();
            LCD.setCursor(0,0);
            LCD.print2(p.name.c_str());
            for (int i = p.name.length(); i < 10; i++) LCD.print2(" ");
            LCD.setCursor(11, 0);
            printDate(Time.month(), Time.day());
            
            LCD.setCursor(0,1);
            sprintf(s, "%d", p.id);
            LCD.print2(s);
            
            LCD.setCursor(11, 1);
            printTime(Time.hour(), Time.minute());
            break;
        }
        case 2: {
            LCD.setCursor(0,0);
            LCD.print2(notification);
            break;
        }
    }
}

void UI_notification(String str) {
    notification = strncpy(notification, str, 16);
    UI_state = 2;
}

//print date format at cursor location
void printDate(int month, int day) {
    char ss[8];
    if (month < 10) LCD.print2("0");
    sprintf(ss, "%d/", month);
    LCD.print2(ss);
    if (day < 10) LCD.print2("0");
    sprintf(ss, "%d", day);
    LCD.print2(ss);
}

//print time format on cursor location
void printTime(int hour, int min) {
    char ss[8];
    if (hour < 10) LCD.print2("0");
    sprintf(ss, "%d:", hour);
    LCD.print2(ss);
    if (min < 10) LCD.print2("0");
    sprintf(ss, "%d", min);
    LCD.print2(ss);
}

//print mL format at cursor location
void printmL(int mL) {
    char ss[8];
    if (mL < 1000) LCD.print2(" ");
    if (mL < 100) LCD.print2(" ");
    if (mL < 10) LCD.print2(" ");
    if (mL < 0) mL = 0;
    sprintf(ss, "%dmL", mL);
    LCD.print2(ss);
}