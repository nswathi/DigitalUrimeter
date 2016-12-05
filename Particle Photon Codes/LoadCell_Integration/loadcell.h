#ifndef _LOADCELL_H
#define _LOADCELL_H

#include "application.h"

#define loadcellDAT D3
#define loadcellCLK D2

#define tempSensor A0


void loadCellInit();
int readScale();
void tare();
void _ISRgetData();
float convert(int raw);
void setZero(int value);
void setPerG(int value);
int getZero();
int getPerG();

#endif