

#include "stm32f1xx_hal.h"

void readCalibrationData (void);

uint16_t getUncompensatedTemperature(void);

uint32_t getUncompensatedPressure(char oss);

float getTemperature(void);

float getPressure(char oss);

void bmpInit (void);
