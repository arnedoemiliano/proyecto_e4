
#include "stm32f1xx_hal.h"
#include "math.h"

#define devAddRead 0xEE
#define devAddWrite 0xEE
extern I2C_HandleTypeDef hi2c1;
//#define BMP180_I2C &hi2c1


//variables globales (pueden ser usadas y cambiadas por cualquier funcion)

short AC1,AC2,AC3=0x00;  //Todas de 16 bits
unsigned short AC4,AC5,AC6=0x00;
short B1,B2,MB,MC,MD=0x00;
long X1, X2, X3, B3, B5, B6, P, T=0;
unsigned long B4, B7=0;


void readCalibrationData (void)
{
uint8_t calibData[21]={0};

//HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, MemAddSize (por bytes), pData, Size, Timeout)
HAL_I2C_Mem_Read(&hi2c1, devAddRead, 0xAA, 1, calibData, 21, 50);

AC1 = (calibData[0]<<8 | calibData[1]);
AC2 = (calibData[2]<<8 | calibData[3]);
AC3 = (calibData[4]<<8 | calibData[5]);
AC4 = (calibData[6]<<8 | calibData[7]);
AC5 = (calibData[8]<<8 | calibData[9]);
AC6 = (calibData[10]<<8 | calibData[11]);
B1 = (calibData[12]<<8 | calibData[13]);
B2 = (calibData[14]<<8 | calibData[15]);
MB = (calibData[16]<<8 | calibData[17]);
MC = (calibData[18]<<8 | calibData[19]);
MD = (calibData[20]<<8 | calibData[21]);

}

uint16_t getUncompensatedTemperature(void){

	uint8_t uTMSB, uTLSB =0;
	uint8_t dataToWrite =0x2E;
	uint16_t uT =0;


	//HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout)
	HAL_I2C_Mem_Write(&hi2c1, devAddWrite, 0xF4, 1, &dataToWrite, 1, 50);
	HAL_Delay(5);
	HAL_I2C_Mem_Read(&hi2c1, devAddRead,0xF6, 1, &uTMSB, 1, 50);
	HAL_I2C_Mem_Read(&hi2c1, devAddRead, 0xF7, 1, &uTLSB, 1, 50);

	////////delete this//////////

	/////////////////////////////

	uT = uTMSB << 8 | uTLSB;

	return uT;
}

uint32_t getUncompensatedPressure(char oss){

uint8_t uData[2]={0};
uint8_t dataToWrite= 0x34 | (oss<<6);
uint32_t uP=0;

HAL_I2C_Mem_Write(&hi2c1, devAddWrite, 0xF4, 1, &dataToWrite, 1, 50);

HAL_Delay(26); //tomo el delay maximo

HAL_I2C_Mem_Read(&hi2c1, devAddRead, 0xF6, 1, uData, 3, 50);

uP= (uData[0]<<16 | uData[1]<<8 | uData[2]) >> (8-oss);

return uP;
}

float getTemperature(void){

uint16_t UT = getUncompensatedTemperature();
X1 = ((UT-AC6) * (AC5/(pow(2,15))));
X2 = ((MC*(pow(2,11))) / (X1+MD));
B5 = X1+X2;
T = (B5+8)/(pow(2,4));
return T/10.0;

}


float getPressure(char oss){

	uint32_t UP = getUncompensatedPressure(oss);
	B6 = B5-4000;
	X1 = (B2 * (B6*B6/(pow(2,12))))/(pow(2,11));
	X2 = AC2*B6/(pow(2,11));
	X3 = X1+X2;
	B3 = (((AC1*4+X3)<<oss)+2)/4;
	X1 = AC3*B6/pow(2,13);
	X2 = (B1 * (B6*B6/(pow(2,12))))/(pow(2,16));
	X3 = ((X1+X2)+2)/pow(2,2);
	B4 = AC4*(unsigned long)(X3+32768)/(pow(2,15));
	B7 = ((unsigned long)UP-B3)*(50000>>oss);
	if (B7<0x80000000) P = (B7*2)/B4;
	else P = (B7/B4)*2;
	X1 = (P/(pow(2,8)))*(P/(pow(2,8)));
	X1 = (X1*3038)/(pow(2,16));
	X2 = (-7357*P)/(pow(2,16));
	P = P + (X1+X2+3791)/(pow(2,4));

	return P;
}

void bmpInit (void){

	readCalibrationData();

}











