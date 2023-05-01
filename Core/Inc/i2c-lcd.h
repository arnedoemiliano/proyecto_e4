#include "stm32f1xx_hal.h"

void lcdInit (void);   // initialize lcd

void lcdSendCmd (char cmd);  // send command to the lcd

void lcdSendData (char data);  // send data to the lcd

void lcdSendString (char *str);  // send string to the lcd

void lcdCursor(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcdClear (void);

void floatToString (float number, char* floatString);

void displayTemp (char* stringTemp);

void displayPressure(char* stringPress);

void displayAlarm(char* stringAlarm);

void displayInicioAlarm(char* stringAlarmFinal);
