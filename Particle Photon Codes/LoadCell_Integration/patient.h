#ifndef _PATIENT_H
#define _PATIENT_H

#include <inttypes.h>
#include "application.h"


struct identity {
    String name;
    unsigned int id;
};
typedef struct identity identity;

struct measurement {
    uint8_t hour;
    uint8_t min;
    
    int mL;
};
typedef struct measurement measurement;


void patientInit();
int setPatientName(String name);
int setPatientID(String id);
int updateMeasurement(int mL);
void newMeasurement();
measurement getOldMeasurement();
measurement getCurrentMeasurement();
identity getIdentity();
void startPatientData();
bool patient_isStart();


#endif