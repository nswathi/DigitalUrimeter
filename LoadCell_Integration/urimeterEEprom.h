#ifndef URIMETER_EEPROM
#define URIMETER_EEPROM

#include "patient.h"

#define ZERO_START 0
#define PER_G_START 4
#define CHECKSUM_START 8
#define PATIENT_NAME_START 12
#define PATIENT_ID_START 28
#define STATUS_START 32
#define NUM_READINGS_START 33
#define READINGS_START 34
#define READINGS_SIZE 6


bool readSettings();
void saveSettings();
void storeMeasurement(measurement);
bool hasMeasurement();
measurement popMeasurement();
void resetEEprom();



#endif