/*
 * GccApplication2.c
 *
 * Created: 12/18/2019 2:42:27 AM
 * Author : tayyebi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// #define F_CPU 8000000UL // FCPU Definition for util/delay used in AVR STUDIO only

// LCD
void LCD_send_command(unsigned char cmnd);
void LCD_send_data(unsigned char data);
void LCD_init(void);
void LCD_goto(unsigned char y, unsigned char x);
void LCD_print(char *string);
void LCD_blink(void);
void LCD_clear(void);

// Keypad
char keypad1();

// Calculator
void memory_send_data(unsigned char data );
char * memory_get_data();
int isOperand(char c);
int value(char c);
int eval(char *str);

// LCD
#define DATA_PORT PORTC
#define DATA_DDR DDRC
#define CNTRL_PORT PORTB
#define CNTRL_DDR DDRB
#define RS_PIN 5
#define RW_PIN 6
#define ENABLE_PIN 7


// KEYPAD
#define prt PORTD
#define ddr DDRD
#define pin PIND
#define delay 260

// KEYPAD LAYOUT
unsigned char key[4][4] = {
	'7','4','1',0x01,
	'8','5','2','0',
	'9','6','3','=',
	'/','*','-','+'
};
unsigned char coll,ro_loc;
unsigned char z='m';
unsigned char memory [16];

/*
// INTERRUPT FUNCTION
ISR(INT2_vect) {
	
	// Turn on the light
	DDRA = 0x80;
	PORTA = 0x80;
	_delay_ms(delay);
	PORTA = 0x00;
	
}
*/



// Main
int main(void)
{
	// LCD
	CNTRL_DDR = 0xF0;
	CNTRL_PORT = 0x00;
	DATA_DDR = 0xFF;
	DATA_PORT = 0x00;
	LCD_clear();
	LCD_init();
	LCD_goto(1,2);
	
	// TODO: Create a delay with timer interrupt
	LCD_print("TAYYEBI NEGAHDAR");
	_delay_ms(delay);
	LCD_clear();
	
	/*
	
	// INTERRUPT
	GICR = 1 << INT2;						// Enable INT2
	MCUCR = 1 << ISC01 | 0 << ISC00;		// Trigger INT2 on falling edge
	asm("sei");	
	
	*/
	
	ddr=0XF0;
	while (1)
	{
		char selected = keypad1();
		
		// If clicked clear
		if (selected == 0x01)
		{
			LCD_clear();
			LCD_goto(1,2);
		}
		
		// If clicked calc
		else if (selected == '=')
		{
			LCD_clear();
			LCD_init();
			LCD_goto(1,2);
			
			char array[64];
			int myInteger = eval(memory_get_data());
			sprintf(array, "%d", myInteger );
			LCD_print(array);
			
			LCD_blink();
			
		}
		else
		{
			LCD_send_data(selected);
			memory_send_data(selected);
		}
	}
}

// Read and return keypad value
char keypad1()
{
	prt=0xFF;
	ddr=0xF0;
	while(1)
	{
		prt=0xef; // 1110 1111
		coll=(pin & 0x0f);
		if(coll!=0x0f)
		{
			ro_loc=0;
			goto xx;
		}
		
		prt=0xdf; // 1101 1111
		coll=(pin & 0x0f);
		if(coll!=0x0f)
		{
			ro_loc=1;
			goto xx;
		}
		
		prt=0xbf; // 1011 1111
		coll=(pin & 0x0f);
		if(coll!=0x0f)
		{
			ro_loc=2;
			goto xx;
		}
		
		prt=0x7f; // 0111 1111
		coll=(pin & 0x0f);
		if(coll!=0x0f)
		{
			ro_loc=3;
			goto xx;
		}
		
		xx:;
		switch (coll)
		{
			case 0x0e: // 1110
				z=(key[0][ro_loc]);
				_delay_ms(delay);
				return z;
				break;
			
			case 0x0d: // 1101
				z=(key[1][ro_loc]);
				_delay_ms(delay);
				return z;
				break;
				
			case 0x0b: // 1011
				z=(key[2][ro_loc]);
				_delay_ms(delay);
				return z;
				break;
				
			case 0x07: // 0111
				z=(key[3][ro_loc]);
				_delay_ms(delay);
				return z;
				break;
		}
		
	}
}

// Send command to LCD
void LCD_send_command(unsigned char cmnd)
{
	DATA_PORT = cmnd;
	CNTRL_PORT &= ~(1<< RW_PIN);
	CNTRL_PORT &= ~(1<< RS_PIN);
	CNTRL_PORT |= (1<< ENABLE_PIN);
	_delay_us(2);
	CNTRL_PORT &= ~(1<< ENABLE_PIN);
	_delay_us(100);
}

// Send data to LCD
void LCD_send_data(unsigned char data)
{
	DATA_PORT = data;
	CNTRL_PORT &= ~(1<< RW_PIN);
	CNTRL_PORT |= (1<< RS_PIN);
	CNTRL_PORT |= (1<< ENABLE_PIN);
	_delay_us(2);
	CNTRL_PORT &= ~(1<< ENABLE_PIN);
	_delay_us(100);
}

// Init LCD
void LCD_init()
{
	_delay_ms(10);
	LCD_send_command(0x38);
	LCD_send_command(0x0E);
	LCD_send_command(0x01);
	_delay_ms(10);
	LCD_send_command(0x06);
}

// Goto XY
void LCD_goto(unsigned char y, unsigned char x)
{
	unsigned char firstAddress[] = {0x80,0xC0,0x94,0xD4};
	LCD_send_command(firstAddress[y-1] + x-1);
	_delay_ms(10);
}

// Print String
void LCD_print(char *string) //put a string in memory location & use it as POINTER
{
	while(*string > 0)
	{
		LCD_send_data(*string++);
	}
}

// For fun! ;)
void LCD_blink()
{
	LCD_send_command(0x08);
	_delay_ms(250);
	LCD_send_command(0x0C);
	_delay_ms(250);
}

// Clear LCD
void LCD_clear(void)
{
	LCD_send_command(0x01);
	_delay_ms(100);
}

// ====== Calculator 

void memory_send_data(unsigned char data ){
	int i = 0 ;
	for ( ; memory[i] ; i++)
	continue;
	memory[i] = data;
}

char * memory_get_data(){
	return memory;
}

int isOperand(char c) { return (c >= '0' && c <= '9'); }
int value(char c) {  return (c - '0'); }
int eval(char *str)
{
	if (*str == '\0')  return -1;
	int res = value(str[0]);
	for (int i = 1; str[i]; i += 2)
	{
		char opr = str[i], opd = str[i+1];
		if (!isOperand(opd))  return -1;
		if (opr == '+')       res += value(opd);
		else if (opr == '-')  res -= value(opd);
		else if (opr == '*')  res *= value(opd);
		else if (opr == '/')  res /= value(opd);
		else return -1;
	}
	return res;
}
