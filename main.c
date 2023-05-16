// /*
//  * finalproject.c
//  *
//  * Created: 4/27/2023 6:04:16 PM
//  * Author : jbartoszczyk
//  */ 
// #include <mfrc522.h>
// #include <spi.h>
// #include <util/delay.h>
// //#include <utils.h>
// #include <avr/io.h>
// //#include <motor.h>
// #include <avr/io.h>
// #include <util/delay.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <ctype.h>
// #include <stdint.h>
// #include <stdbool.h>
// 
// #define F_CPU 16000000L
// #define UART_BAUD 9600
// 
// //usart prototype for functions
// int uart_putchar(char, FILE*);
// int uart_getchar(FILE*);
// 
// static FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
// 
// 
// 
// 
// int main(void)
// {
// 	stdout = stdin = &uart_io;
// 	
// 	UCSR0A = 1 << U2X0;  //explain lab report
// 	UBRR0L = (F_CPU / (8UL * UART_BAUD)) - 1;
// 	
// 	UCSR0B = 1 << TXEN0 | 1 << RXEN0;
// 	bool check;
// 	DDRD |= (1 << PORTD7);
// 	DDRD |= (1 << PORTD3);
// 	uint8_t byte;
// 	uint8_t str[MAX_LEN];
// 	uint8_t granted[4] = {0x88, 0x04, 0x0d ,0x4c};
// 	printf("hello world");
// 	spi_init();
// 	mfrc522_init();
// 	PORTD &= ~(1 << PORTD7);
// 	PORTD |= (1 << PORTD3);
// 	while(1){
// 		byte = mfrc522_request(PICC_REQALL,str);
// 		if(byte == CARD_FOUND){
// 			byte = mfrc522_get_card_serial(str);
// 			if(byte == CARD_FOUND){
// 				
// 				for(int i =0; i< 8;i++){
// 					printf("%02x ",str[i]);
// 				}
// 				
// 				for(int i =0; i <4;i++){
// 					check = granted[i] == str[i];
// 				}
// 				if(check){
// 					PORTD |= (1 << PORTD7);
// 					PORTD &= ~(1 << PORTD3);
// 					printf("Access Granted!\n");
// 				}else{
// 					printf("Access Denied!\n");
// 					PORTD |= (1 << PORTD3);
// 					PORTD &= ~(1 << PORTD7);
// 				}
// 				
// 				printf("\n");
// 			}
// 		}else{
// 		
// 		PORTD |= (1 << PORTD3);
// 		PORTD &= ~(1 << PORTD7);
// 		
// 		}
// 	
// 	}
// }
// 
// int uart_putchar(char c, FILE *f){
// 	if(c == '\n'){
// 		uart_putchar('\r', f);
// 	}
// 	
// 	while( !(UCSR0A & (1<<UDRE0)) );
// 	UDR0 = c;
// 	
// 	return 0;
// }











/*
 * anothertest.c
 *
 * Created: 5/1/2023 9:13:25 PM
 * Author : brend
 */ 

#define F_CPU 16000000L
#define UART_BAUD 9600

#include <mfrc522.h>
#include <spi.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#define lcd_D7_port     PORTD                   // LCD D7 connection
#define lcd_D7_bit      PORTD7
#define lcd_D7_ddr      DDRD

#define lcd_D6_port     PORTD                   // LCD D6 connection
#define lcd_D6_bit      PORTD6
#define lcd_D6_ddr      DDRD

#define lcd_D5_port     PORTD                   // LCD D5 connection
#define lcd_D5_bit      PORTD5
#define lcd_D5_ddr      DDRD

#define lcd_D4_port     PORTD                   // LCD D4 connection
#define lcd_D4_bit      PORTD4
#define lcd_D4_ddr      DDRD

#define lcd_E_port      PORTC                   // LCD Enable pin
#define lcd_E_bit       PORTC4
#define lcd_E_ddr       DDRC

#define lcd_RS_port     PORTC                   // LCD Register Select pin
#define lcd_RS_bit      PORTC5
#define lcd_RS_ddr      DDRC

// LCD instructions
#define lcd_Clear           0b00000001          // replace all characters with ASCII 'space'
#define lcd_Home            0b00000010          // return cursor to first position on first line
#define lcd_EntryMode       0b00000110          // shift cursor from left to right on read/write
#define lcd_DisplayOff      0b00001000          // turn display off
#define lcd_DisplayOn       0b00001100          // display on, cursor off, don't blink character
#define lcd_FunctionReset   0b00110000          // reset the LCD
#define lcd_FunctionSet4bit 0b00101000          // 4-bit data, 2-line display, 5 x 7 font
#define lcd_SetCursor       0b10000000          // set cursor position

// Program ID
uint8_t signin[] = "Please Sign In";
uint8_t timeRemaining[]   = "Time Remaining:";
uint8_t timeList[][10] = {"0","1","2","3","4","5","6","7","8","9"};
uint8_t timeList2[][7] = {"0","1","2","3","4","5","0"};

// Function Prototypes
bool countdownMinutes(int);
bool seconds(int);
void lcd_write_4(uint8_t);
void instrWrite(uint8_t);
void lcd_write_character_4d(uint8_t);
void lcd_write_string_4d(uint8_t *);
void lcdInit(void);
int lcdCountdown(void);
int uart_putchar(char, FILE*);
int uart_getchar(FILE*);

static FILE uart_io = FDEV_SETUP_STREAM(uart_putchar,uart_getchar,_FDEV_SETUP_RW);

int uart_putchar(char c, FILE *f){
	if(c == '\n'){
		uart_putchar('r',f);
	}
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
	return 0;
}

int uart_getchar(FILE *F){
	while(!(UCSR0A & (1<<RXC0)));
	uint8_t c = UDR0;
	return c;
}
uint8_t byte;
uint8_t str[MAX_LEN];
uint8_t granted[4] = {0xde, 0x7c ,0xc0 ,0x7e};
int main(void)
{
	stdout = stdin = &uart_io;
	
	UCSR0A = 1 << U2X0;
	UBRR0L = (F_CPU / (8UL * UART_BAUD)) - 1;
	UCSR0B = 1 << TXEN0 | 1 << RXEN0;
	bool check;
	DDRD |= (1 << PORTD2);
	DDRD |= (1 << PORTD3);
	DDRC |= (1 << PORTC2);
// 	uint8_t byte;
// 	uint8_t str[MAX_LEN];
// 	uint8_t granted[4] = {0x88 ,0x04 ,0x0d ,0x4c};
	printf("hello world");
	spi_init();
	mfrc522_init();
	PORTD &= ~(1 << PORTD2);
	PORTD |= (1 << PORTD3);
	//PORTC |= (1<< PORTC2);
	while (1)
	{
		beginSignin();
		byte = mfrc522_request(PICC_REQALL,str);
		if(byte == CARD_FOUND){
			byte = mfrc522_get_card_serial(str);
			if(byte == CARD_FOUND){
				for(int i = 0; i < 8; i++){
					printf("%02x ",str[i]);
				}
				for(int i = 0; i < 4; i++){
					check = granted[i] == str[i];
				}
				if(check){
					PORTD |= (1 << PORTD2);
					PORTD &= ~(1 << PORTD3);
					PORTC |= (1<< PORTC2);
					lcd_D7_ddr |= (1<<lcd_D7_bit);                  // 4 data lines - output
					lcd_D6_ddr |= (1<<lcd_D6_bit);
					lcd_D5_ddr |= (1<<lcd_D5_bit);
					lcd_D4_ddr |= (1<<lcd_D4_bit);

					lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
					lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output

					lcdInit();
					lcd_write_string_4d("ACCESS GRANTED");
					printf("Access Granted");
					lcdCountdown();
				}
				else{
					printf("Access Denied");
					lcd_D7_ddr |= (1<<lcd_D7_bit);                  // 4 data lines - output
					lcd_D6_ddr |= (1<<lcd_D6_bit);
					lcd_D5_ddr |= (1<<lcd_D5_bit);
					lcd_D4_ddr |= (1<<lcd_D4_bit);

					lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
					lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output

					lcdInit();
					lcd_write_string_4d("ACCESS DENIED");
					PORTC |= (1<< PORTC2);
					_delay_ms(300);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(300);					
				}
				printf("\n");
			}
		}
		else{
			PORTD |= (1 << PORTD3);
			PORTD &= ~(1 << PORTD2);
		}
	}
}

int lcdCountdown(void)
{
	_delay_ms(300);
	PORTC &= ~(1<< PORTC2);
	_delay_ms(700);
	PORTD |= (1 << PORTD3);
	PORTD &= ~(1 << PORTD2);
	
	lcd_D7_ddr |= (1<<lcd_D7_bit);                  // 4 data lines - output
	lcd_D6_ddr |= (1<<lcd_D6_bit);
	lcd_D5_ddr |= (1<<lcd_D5_bit);
	lcd_D4_ddr |= (1<<lcd_D4_bit);

	lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
	lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output

	lcdInit();                                  // initialize the LCD display for a 4-bit interface

	lcd_write_string_4d(timeRemaining);

	instrWrite(lcd_SetCursor | 0x40 );
	_delay_us(80);
	
	int hourTime = 0;
	int minuteTime = 1;
	finalCountdown(hourTime,minuteTime);

	return 0;
}

int beginSignin(void){
		lcd_D7_ddr |= (1<<lcd_D7_bit);                  // 4 data lines - output
		lcd_D6_ddr |= (1<<lcd_D6_bit);
		lcd_D5_ddr |= (1<<lcd_D5_bit);
		lcd_D4_ddr |= (1<<lcd_D4_bit);

		lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
		lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output

		lcdInit();                                  // initialize the LCD display for a 4-bit interface

		lcd_write_string_4d(signin);
}

void lcdInit(void)
{
	_delay_ms(100);

	lcd_RS_port &= ~(1<<lcd_RS_bit);
	lcd_E_port &= ~(1<<lcd_E_bit);

	lcd_write_4(lcd_FunctionReset);
	_delay_ms(10);

	lcd_write_4(lcd_FunctionReset);
	_delay_us(200);

	lcd_write_4(lcd_FunctionReset);
	_delay_us(200);
	
	lcd_write_4(lcd_FunctionSet4bit);
	_delay_us(80);

	instrWrite(lcd_FunctionSet4bit);
	_delay_us(80);

	instrWrite(lcd_DisplayOff);
	_delay_us(80);

	instrWrite(lcd_Clear);
	_delay_ms(4);

	instrWrite(lcd_EntryMode);
	_delay_us(80);

	instrWrite(lcd_DisplayOn);
	_delay_us(80);
}

void lcd_write_string_4d(uint8_t theString[])
{
	volatile int i = 0;                             // character counter*/
	while (theString[i] != 0){
		lcd_write_character_4d(theString[i]);
		i++;
		_delay_us(80);                         // 40 uS delay (min)
	}
}

void lcd_write_character_4d(uint8_t theData){
	lcd_RS_port |= (1<<lcd_RS_bit);                 // select the Data Register (RS high)
	lcd_E_port &= ~(1<<lcd_E_bit);                  // make sure E is initially low
	lcd_write_4(theData);                           // write the upper 4-bits of the data
	lcd_write_4(theData << 4);                      // write the lower 4-bits of the data
}

void instrWrite(uint8_t theInstruction){
	lcd_RS_port &= ~(1<<lcd_RS_bit);                // select the Instruction Register (RS low)
	lcd_E_port &= ~(1<<lcd_E_bit);                  // make sure E is initially low
	lcd_write_4(theInstruction);                    // write the upper 4-bits of the data
	lcd_write_4(theInstruction << 4);               // write the lower 4-bits of the data
}

void lcd_write_4(uint8_t theByte){
	lcd_D7_port &= ~(1<<lcd_D7_bit);
	if (theByte & 1<<7) lcd_D7_port |= (1<<lcd_D7_bit);

	lcd_D6_port &= ~(1<<lcd_D6_bit);
	if (theByte & 1<<6) lcd_D6_port |= (1<<lcd_D6_bit);

	lcd_D5_port &= ~(1<<lcd_D5_bit);
	if (theByte & 1<<5) lcd_D5_port |= (1<<lcd_D5_bit);

	lcd_D4_port &= ~(1<<lcd_D4_bit);
	if (theByte & 1<<4) lcd_D4_port |= (1<<lcd_D4_bit);

	lcd_E_port |= (1<<lcd_E_bit);
	_delay_us(1);
	lcd_E_port &= ~(1<<lcd_E_bit);
	_delay_us(1);
}

void finalCountdown(int hours, int minutes){
	if (hours > 3){
		hours = 3;
	}
	if (minutes > 59){
		minutes = 59;
	}
	
	int tenPlace = minutes/10;
	int onePlace = minutes - (tenPlace * 10);
	instrWrite(lcd_SetCursor | 0x40 );
	_delay_us(80);
	lcd_write_string_4d(timeList[hours]);
	_delay_ms(1000);
	instrWrite(lcd_SetCursor | 0x41 );
	_delay_us(80);
	lcd_write_string_4d(":");
	instrWrite(lcd_SetCursor | 0x42 );
	_delay_us(80);
	lcd_write_string_4d(timeList2[tenPlace]);
	instrWrite(lcd_SetCursor | 0x43 );
	_delay_us(80);
	lcd_write_string_4d(timeList[onePlace]);
	instrWrite(lcd_SetCursor | 0x44 );
	_delay_us(80);
	lcd_write_string_4d(":00");
	_delay_ms(1000);
	
	if(countdownMinutes(minutes)){
		PORTD |= (1 << PORTD2);
		PORTD &= ~(1 << PORTD3);
		PORTC |= (1<< PORTC2);
		_delay_ms(300);
		PORTC &= ~(1<< PORTC2);
		
		_delay_ms(700);
		
		PORTD |= (1 << PORTD3);
		PORTD &= ~(1 << PORTD2);
		return;
	}
	for(int i = (hours-1); i >= 0; i--){
		instrWrite(lcd_SetCursor | 0x40 );
		_delay_us(80);
		lcd_write_string_4d(timeList[i]);
		if(countdownMinutes(60)){
			return;
		}
	}
	lcd_D7_ddr |= (1<<lcd_D7_bit);                  // 4 data lines - output
	lcd_D6_ddr |= (1<<lcd_D6_bit);
	lcd_D5_ddr |= (1<<lcd_D5_bit);
	lcd_D4_ddr |= (1<<lcd_D4_bit);

	lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
	lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output

	lcdInit();                                  // initialize the LCD display for a 4-bit interface

	lcd_write_string_4d("TIME EXCEEDED!");
	instrWrite(lcd_SetCursor | 0x40);
	lcd_write_string_4d("SSCAN ID!");
	bool check;
	int x = 500;
	while(1){
		PORTC |= (1<< PORTC2);
		PORTD |= (1 << PORTD3);
		PORTD &= ~(1 << PORTD2);
		for(int i = x; i > 0; i--){
			_delay_ms(1);
			
		}
		PORTD |= (1 << PORTD2);
		PORTD &= ~(1 << PORTD3);
		PORTC &= ~(1<< PORTC2);
		for(int i = x; i > 0; i--){
			_delay_ms(1);
			
		}
		x = x-20;
		if(x < 100){
			x = 100;
		}
		
		
		byte = mfrc522_request(PICC_REQALL,str);
		if(byte == CARD_FOUND){
			byte = mfrc522_get_card_serial(str);
			if(byte == CARD_FOUND){
				for(int i = 0; i < 8; i++){
					printf("%02x ",str[i]);
				}
				for(int i = 0; i < 4; i++){
					check = granted[i] == str[i];
				}
				if(check){
					printf("Access Granted");
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(550);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(400);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(200);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(500);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(480);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(380);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(150);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(400);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					_delay_ms(160);
					PORTC |= (1<< PORTC2);
					_delay_ms(100);
					PORTC &= ~(1<< PORTC2);
					return;
				}
				else{
					printf("Access Denied");
				}
				printf("\n");
			}
		}
	}
	
	
}


bool countdownMinutes(int minutes){
	
	instrWrite(lcd_SetCursor | 0x44 );
	_delay_us(80);
	lcd_write_string_4d(":");
	
	for(int j = (minutes - 1); j >= 0; j--){
		int tens = j/10;
		int ones = j - (tens * 10);
		instrWrite(lcd_SetCursor | 0x42 );
		_delay_us(80);
		lcd_write_string_4d(timeList[tens]);
		instrWrite(lcd_SetCursor | 0x43);
		_delay_us(80);
		lcd_write_string_4d(timeList[ones]);
		if(seconds(0x45)){
			return false;
		}
	}
	return true;
	
}

bool seconds(int addr){
	bool check;
	int addr2 = addr + 1;
	for(int j = 59; j >= 0; j--){
		byte = mfrc522_request(PICC_REQALL,str);
		if(byte == CARD_FOUND){
			byte = mfrc522_get_card_serial(str);
			if(byte == CARD_FOUND){
				for(int i = 0; i < 8; i++){
					printf("%02x ",str[i]);
				}
				for(int i = 0; i < 4; i++){
					check = granted[i] == str[i];
				}
				if(check){
					printf("Access Granted");
					return false;
				}
				else{
					PORTC |= (1<< PORTC2);
					_delay_ms(300);
					PORTC &= ~(1<< PORTC2);
					printf("Access Denied");
				}
				printf("\n");
			}
		}
		int tens = j/10;
		int ones = j - (tens * 10);
		instrWrite(lcd_SetCursor | addr );
		_delay_us(80);
		lcd_write_string_4d(timeList2[tens]);
		instrWrite(lcd_SetCursor | addr2);
		_delay_us(80);
		lcd_write_string_4d(timeList[ones]);
		_delay_ms(1000);
	}
	return true;
}




