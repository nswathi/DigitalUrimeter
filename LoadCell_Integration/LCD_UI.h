#ifndef _LCD_UI_H
#define _LCD_UI_H

#include "application.h"

#define UI_toggleButton D5

#define MOTION_NOTIFICATION "MOTION DETECTED"
#define CALIBRATION_NOTIFICATION "CALIBRATING...."

void UI_init();
void UI_display();
void UI_toggle();
void UI_refresh();
void UI_setMode(int m);
void UI_notification(String str);

#endif