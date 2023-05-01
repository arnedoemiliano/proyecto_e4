

#include "i2c-lcd.h"
#include "string.h"
#include "stdio.h"

extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly


#define SLAVE_ADDRESS_LCD 0x4E // change this according to your setup



extern uint8_t flag_clear;

void lcdSendCmd (char cmd)	//cmd=10111010
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);	  //data_u=10110000		&: and bit a bit
	data_l = ((cmd<<4)&0xf0); //data_1=10100000		|: or bit a bit
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 20);
}

void lcdSendData (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 20);
}

void lcdClear(void)		//Tiene una bandera de activacion
{
	if(flag_clear == 1){
	lcdSendCmd (0x80);
	for (int i=0; i<70; i++)
	{
		lcdSendData (' ');
	}
	}
	flag_clear=0;
}

void lcdCursor(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcdSendCmd (col);
}


void lcdInit (void)
{
	// 4 bit initialization
	HAL_Delay(50);  // wait for >40ms
	lcdSendCmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcdSendCmd (0x30);
	HAL_Delay(1);  // wait for >100us
	lcdSendCmd (0x30);
	HAL_Delay(10);
	lcdSendCmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // display initialization
	lcdSendCmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcdSendCmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcdSendCmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcdSendCmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcdSendCmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcdSendString (char *str)
{
	while (*str) lcdSendData (*str++);
}



/* ***FLOAT TO STRING *** */

void floatToString (float number, char* floatString){

	int limit=0;
	int cnt=1;
	char *p;
	char cadena[16]={0};

	memset(floatString,0,16);


	sprintf(cadena, "%.2f",number);

	//Busco donde esta el punto flotante para limitar en dos la cantidad de decimales

	p=cadena;
	while(*p != '\0'){
		if(*p == '.'){
			limit=cnt;		//limit tiene la posicion del punto flotante
		}
		p++;
		cnt++;
	}

	if(limit<4){    //si el punto flotante esta en la tercera posicion o menor el dato recibido es de temperatura

	    	strncpy(floatString, cadena, limit+1); //limito en uno los decimales
	   }

	else{
	    	strncpy(floatString, cadena, limit+2); //limito en dos los decimales
	   }


}



void displayTemp (char* stringTemp){


	char stringToSend[16]="Temp: ";
	char label[4]={0};
	label[0]= 0xDF;
	label[1]= 'C';

	strcat(stringToSend, stringTemp);
	strcat(stringToSend, label );

	lcdSendString(stringToSend);


}




void displayPressure(char* stringPress){
	char stringToSend[16]="P:";
	char label[4]="hpa";		//tiene que ser 5

	strcat(stringToSend, stringPress);
	strcat(stringToSend, label );

	lcdSendString(stringToSend);

}


void displayAlarm(char* stringAlarm){		//Para mostrar la temperatura que se configura en el modo CONFIG_TEMP

	char string1Line[16]="Temperatura max:";	//string de la primera linea	//ERA 8 ANTES
	char label[4]={0};
	label[0]= 0xDF;	//simbolo de grados (°)
	label[1]= 'C';

	strcat(stringAlarm, label);		//stringAlarm ahora es la string de la segunda linea.


	lcdSendString(string1Line);
	lcdCursor(1, 0);
	lcdSendString(stringAlarm);

}


void displayInicioAlarm(char* stringAlarmFinal){

	char string1Line[16]="Alarma en:";
	char label[4]={0};
	label[0]= 0xDF;	//simbolo de grados (°)
	label[1]= 'C';

	strcat(stringAlarmFinal, label);

	lcdSendString(string1Line);
	lcdCursor(1, 0);
	lcdSendString(stringAlarmFinal);


}


