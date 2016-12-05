#include "application.h"
#include "patient.h"
#include <inttypes.h>
#include "urimeterEEprom.h"

identity p;
measurement old;
measurement current;
int start = 0;

void patientInit() {
    p.name = "Jon";
    p.id = 12345;
    Particle.function("patientName",setPatientName);
    Particle.function("patientID",setPatientID);
    old.hour = Time.hour();
    old.min = Time.minute();
    old.mL = 0;
    current.hour = Time.hour();
    current.min = Time.minute();
    current.mL = 0;
}

int setPatientName(String name) {
    if(name.length()>2 && name.length() < 16){
           p.name = name;
           int i = 0;
           const char * k = name.c_str();
        while (k[i] != '\0') EEPROM.put(PATIENT_NAME_START + i++, k[i]);
        EEPROM.put(PATIENT_NAME_START + i, '\0');
           //EEPROM.put(PATIENT_NAME_START, name);
           /*for (int c = 0; c < 16; c++) {
               EEPROM.write(PATIENT_NAME_START + c, 'a');
           }
        char const * ch = name.c_str();
        for (int i = 0; i < name.length(); i++) {
            if (i >= 10) break;
            EEPROM.write(PATIENT_NAME_START + i, ch[i]);
        }
        EEPROM.write(PATIENT_NAME_START + name.length(), '\0'); */
           return 1;
       } 
       else{ 
           return -1;
       }
}

int setPatientID(String id) {
    if(id.length() > 2 && id.length() < 11){
        p.id = atoi(id);
        EEPROM.put(PATIENT_ID_START, p.id);
        return 1;
    } 
    else{ 
        return -1;
    }
}


int updateMeasurement(int mL) {
    int diff = mL - current.mL;
    //if (diff >= 0) {
        current.mL = mL;
        current.hour = Time.hour();
        current.min = Time.minute();
        return 1;
    //}
    //else return 0;
}

void newMeasurement() {
    old.hour = current.hour;
    old.min = current.min;
    old.mL = current.mL;
    
    current.hour = Time.hour();
    current.min = Time.minute();
    current.mL = 0;
}

measurement getOldMeasurement() {
    return old;
}

measurement getCurrentMeasurement() {
    return current;
}

identity getIdentity() {
    return p;
}

void startPatientData() {start = 1;}
bool patient_isStart() {return start == 1;}