#include <mcs51/8052.h>
#include <stdio.h>
#define Beep	P3_6	//Buzzer
#define DQ		P3_7	//Data pin of the DS18B20
#define LCD_RS	P1_0	//Register select pin of the LCD
#define LCD_EN	P1_1	//Enable pin of the LCD
#define LCD_D4	P1_2	//Data pins of the LCD (D4-D7)
#define LCD_D5	P1_3
#define LCD_D6	P1_4
#define LCD_D7	P1_5
unsigned char temp; 			// Current temperature
unsigned char highest_temp;		// Highest temperature in the past minute
unsigned char lowest_temp;		// Lowest temperature in the past minute
unsigned char temp_history[10];	// Temperature history for the past minute
unsigned char temp_history_index;	// Index into the temperature history buffer//Temp variable

//Delay func (not so accurate)
void delay_ms(unsigned int t) {
	unsigned int i,j;
	for(i = 0; i < t; i++)
		for(j = 0; j < 125; j++);
}

//Delay func for the DS18B20 (not so accurate)
void delay_us_DS18B20(unsigned char t) {
    while(t--);
}

//Init for the DS18B20 -> See Init Timing in the datasheet
void Init_DS18B20() {
    DQ = 1;
    delay_us_DS18B20(10);
    DQ = 0;					//Reset pulse ...
    delay_us_DS18B20(150); 	//for a minimum period. See the datasheet.
    DQ = 1;					//Rising edge
    delay_us_DS18B20(20);   //Wait for the DS18B20 to response
}

//Read a byte from the Scratchpad
unsigned char ReadByteFromScratchpad() {
    unsigned char i = 0;
    unsigned char byte = 0;
    for (i = 8; i > 0; i--){
          DQ = 0;
          byte >>= 1;
          DQ = 1;			//The master bus releases the 1-Wire bus
          if(DQ)			//Read 1? Otherwise, Read 0
          	byte |= 0x80;
          delay_us_DS18B20(20);
    }
    return(byte);
}

//Write a byte to the Scratchpad
void WriteByteToScratchpad(unsigned char byte) {
    unsigned char i = 0;
    for (i = 8; i > 0; i--){
        DQ = 0;
        DQ = byte&0x01;
		if(DQ)						//Write 1
        	delay_us_DS18B20(3);
		else						//Write 0
			delay_us_DS18B20(20);
        DQ = 1;	  					//The master bus releases the 1-Wire bus
        byte >>= 1;
    }
}

//Read the DS18B20
void ReadTemperature() {
    unsigned char Byte0 = 0;		//Byte0 of the Scratchpad
    unsigned char Byte1 = 0;		//Byte1 of the Scratchpad

    Init_DS18B20();				   	//DS18B20 initialization
    WriteByteToScratchpad(0xCC);	//The master issues Skip ROM [CCh] command
    WriteByteToScratchpad(0x44);	//Convert T [44h] command. To initiate a temp measurement and A-to-D conversion.
    delay_us_DS18B20(10);

    Init_DS18B20();					//DS18B20 initialization
    WriteByteToScratchpad(0xCC);	//The master issues Skip ROM [CCh] command
    WriteByteToScratchpad(0xBE);	//Read temp value. Read Scratpad [BEh] command.
    delay_us_DS18B20(10);

    Byte0 = ReadByteFromScratchpad();	//Read Byte0
    Byte1 = ReadByteFromScratchpad();	//Read Byte1
    temp = Byte0>>4;					//Ignore the first 4 bits
    temp |= (Byte1<<4);					//Get the 12 bits of the temperature
}

//Send command to LCD
void lcd_cmd(unsigned char cmd) {
	LCD_RS = 0;
	LCD_D7 = (cmd & 0x80)>>7;
	LCD_D6 = (cmd & 0x40)>>6;
	LCD_D5 = (cmd & 0x20)>>5;
	LCD_D4 = (cmd & 0x10)>>4;
	LCD_EN = 1;
	delay_ms(1);
	LCD_EN = 0;
	LCD_D7 = (cmd & 0x08)>>3;
	LCD_D6 = (cmd & 0x04)>>2;
	LCD_D5 = (cmd & 0x02)>>1;
	LCD_D4 = cmd & 0x01;
	LCD_EN = 1;
	delay_ms(1);
	LCD_EN = 0;
}

//Send data to LCD
void lcd_data(unsigned char data) {
	LCD_RS = 1;
	LCD_D7 = (data & 0x80)>>7;
	LCD_D6 = (data & 0x40)>>6;
	LCD_D5 = (data & 0x20)>>5;
	LCD_D4 = (data & 0x10)>>4;
	LCD_EN = 1;
	delay_ms(1);
	LCD_EN = 0;
	LCD_D7 = (data & 0x08)>>3;
	LCD_D6 = (data & 0x04)>>2;
	LCD_D5 = (data & 0x02)>>1;
	LCD_D4 = data & 0x01;
	LCD_EN = 1;
	delay_ms(1);
	LCD_EN = 0;
}

//Initialize the LCD
void lcd_init() {
	LCD_RS = 0;
	LCD_EN = 0;
	LCD_D4 = 0;
	LCD_D5 = 0;
	LCD_D6 = 0;
	LCD_D7 = 0;
	delay_ms(15);
	lcd_cmd(0x30);
	delay_ms(5);
	lcd_cmd(0x30);
	delay_ms(1);
	lcd_cmd(0x30);
	delay_ms(1);
	lcd_cmd(0x20);
	lcd_cmd(0x28);
	lcd_cmd(0x0C);
	lcd_cmd(0x01);
	delay_ms(2);
	lcd_cmd(0x06);
}

//Clear the LCD
void lcd_clear() {
	lcd_cmd(0x01);
	delay_ms(2);
}

//Set cursor position on the LCD
void lcd_gotoxy(unsigned char x, unsigned char y) {
	unsigned char address;
	if (y == 0) {
		address = 0x80 + x;
	} else {
		address = 0xC0 + x;
	}
	lcd_cmd(address);
}

//Print a string on the LCD
void lcd_puts(unsigned char *str) {
	while (*str) {
		lcd_data(*str++);
	}
}

//Print the temperature on the LCD
void lcd_put_temp(unsigned char temp) {
	unsigned char buf[7];
	if (temp >= 100) {
		sprintf(buf, "%d.%dC", temp/10, temp%10);
	} else {
		sprintf(buf, " %d.%dC", temp/10, temp%10);
	}
	lcd_puts(buf);
}

void main() {
	lcd_init();						// Initialize the LCD
	lcd_clear();					// Clear the LCD
	lcd_gotoxy(0,0);				// Set cursor to the first row, first column
	lcd_puts("Temperature:");		// Print "Temperature:" on the LCD

	highest_temp = 0;				// Initialize the highest temperature
	lowest_temp = 255;				// Initialize the lowest temperature
	temp_history_index = 0;			// Initialize the temperature history index
	for (int i = 0; i < 10; i++) {
		temp_history[i] = 0;		// Initialize the temperature history buffer
	}

	while(1) {
		// Read temperature from the DS18B20
		ReadTemperature();

		// Update the temperature history buffer
		temp_history[temp_history_index] = temp;
		temp_history_index = (temp_history_index + 1) % 10;

		// Update the highest and lowest temperature values
		highest_temp = 0;
		lowest_temp = 255;
		for (int i = 0; i < 10; i++) {
			if (temp_history[i] > highest_temp) {
				highest_temp = temp_history[i];
			}
			if (temp_history[i] < lowest_temp) {
				lowest_temp = temp_history[i];
			}
		}

		// Display the current temperature, highest temperature, and lowest temperature
		lcd_gotoxy(0,1);				// Set cursor to the second row, first column
		lcd_puts("  ");					// Clear the old temperature
		lcd_gotoxy(0,1);				// Set cursor to the second row, first column
		lcd_put_temp(temp);				// Print the new temperature on the LCD
		lcd_puts(" ");

		lcd_gotoxy(9,0);				// Set cursor to the first row, tenth column
		lcd_puts("   ");				// Clear the old highest temperature
		lcd_gotoxy(9,0);				// Set cursor to the first row, tenth column
		lcd_put_temp(highest_temp);		// Print the new highest temperature on the LCD

		lcd_gotoxy(13,0);				// Set cursor to the first row, fourteenth column
		lcd_puts("   ");				// Clear the old lowest temperature
		lcd_gotoxy(13,0);				// Set cursor to the first row, fourteenth column
		lcd_put_temp(lowest_temp);		// Print the new lowest temperature on the LCD

		delay_ms(6000);					// Wait for 6 seconds
	}
}

