#include "urimeterEEprom.h"
#include "application.h"
#include "patient.h"
#include "loadcell.h"
#include <inttypes.h>

uint8_t numReadings = 0;

struct settings{
    int zero;
    int per_g;
    int checksum;
    String patient_name;
    unsigned int patient_id;
};
typedef struct settings settings;

bool readSettings() {
    //try to read scale_zero and per_g values from memory
    int z, p, chksum;
    EEPROM.get(ZERO_START, z);
    EEPROM.get(PER_G_START, p);
    EEPROM.get(CHECKSUM_START, chksum);
    //if checksum is good, continue on
    if ((int)(z^p) != chksum) return false;
    setZero(z);
    setPerG(p);
    
    //read name from memory
    
    char str[16];
    char i = 0;
    str[i] = EEPROM.read(PATIENT_NAME_START);
    while (str[++i] != '\0') {
        if (i >= 16) break;
        str[i] = EEPROM.read(PATIENT_NAME_START + i);
    }
    str[i] = '\0';
    String name(str);
    
    //String name;
    //EEPROM.get(PATIENT_NAME_START, name);
    setPatientName(name);
    
    //read patient id from memory
    char s[16];
    unsigned int id;
    EEPROM.get(PATIENT_ID_START, id);
    sprintf(s, "%u", id);
    setPatientID(s);
    
    EEPROM.get(NUM_READINGS_START, numReadings);
    return true;
}

void saveSettings() {
    int z, p, c;
    z = getZero();
    p = getPerG();
    EEPROM.put(ZERO_START, z);
    EEPROM.put(PER_G_START, p);
    c = z^p;
    EEPROM.put(CHECKSUM_START, c);
    
    identity patient = getIdentity();
    /*
    char const * ch = patient.name.c_str();
    char i = 0;
    EEPROM.write(PATIENT_NAME_START, ch[i]);
    while (i++ < patient.name.length()) {
        if (i >= 10) break;
        EEPROM.write(PATIENT_NAME_START + i, ch[i]);
    }
    EEPROM.write(PATIENT_NAME_START + i, '\0');*/
    char k[10] = "Jon";
    int i = 0;
    while (k[i] != '\0') EEPROM.put(PATIENT_NAME_START + i++, k[i]);
    EEPROM.put(PATIENT_NAME_START + i, '\0');
    setPatientName(k);
    EEPROM.put(PATIENT_ID_START, patient.id);
    EEPROM.put(NUM_READINGS_START, numReadings);
}

void storeMeasurement(measurement m) {
    EEPROM.put(READINGS_START + numReadings * READINGS_SIZE, m);
    numReadings++;
    EEPROM.put(NUM_READINGS_START, numReadings);
}

bool hasMeasurement() {return (numReadings > 0);}

measurement popMeasurement() {
    measurement m;
    EEPROM.get(READINGS_START + (numReadings-1)*READINGS_SIZE, m);
    numReadings--;
    return m;
}

//clears patient data and resets photon
void resetEEprom() {
    int chksum = 0;
    EEPROM.put(CHECKSUM_START, chksum);
    String name = "N/A";
    EEPROM.put(PATIENT_NAME_START, name);
    unsigned int id = 9999;
    EEPROM.put(PATIENT_ID_START, id);
    char n = 0;
    EEPROM.put(NUM_READINGS_START, n);
    System.reset();
}
