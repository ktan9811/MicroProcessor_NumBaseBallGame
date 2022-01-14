#include <avr/io.h>
#include <avr/interrupt.h>
///////////////////////// FOR GLCD
#define LCD_Control (* (volatile unsigned char *) 0x8001)
#define LCD_Data (* (volatile unsigned char *) 0x8000)
#define EX_LED				  (*(volatile unsigned char*)0x8008)


#define RS_ON 		LCD_Control=(LCD_Controller|=0x01)
#define RS_OFF 		LCD_Control=(LCD_Controller&=0xfe)
#define RW_ON		LCD_Control=(LCD_Controller|=0x02)
#define RW_OFF 		LCD_Control=(LCD_Controller&=0xfd)
#define EN_ON 		LCD_Control=(LCD_Controller|=0x04)
#define EN_OFF		LCD_Control=(LCD_Controller&=0xfb)
#define CS2_ON 		LCD_Control=(LCD_Controller|=0x10)
#define CS2_OFF 	LCD_Control=(LCD_Controller&=0xef)
#define CS1_ON 		LCD_Control=(LCD_Controller|=0x08)
#define CS1_OFF 	LCD_Control=(LCD_Controller&=0xf7)
#define RESET_ON 	LCD_Control=(LCD_Controller|=0x20)
#define RESET_OFF 	LCD_Control=(LCD_Controller&=0xdf)
#define CS_ALL 		LCD_Control=(LCD_Controller|=0x18)
#define ON 1
#define OFF 0
#define delaytime 0
////////////////////// FOR USART0
#define SYSTEM_CLOCK 16000000
#define USARTBoudRate ((SYSTEM_CLOCK/57600/16)-1)
#define NULL 0
///////////////////// FOR DOT
#define DM_SEL			(*(volatile unsigned int *)0x8004)
#define DM_DATA			(*(volatile unsigned int *)0x8006)

////////////////// FOR GAME
unsigned char answer[4];
unsigned char user_answer[4];
unsigned char strike = 0;
unsigned char ball = 0;
unsigned char x = 0;

////////////////// SONG
unsigned int freq[9] = {0xc440, 0xcac4, 0xd095, 0xd33b, 0xd817, 0xdc7e, 0xe100, 0xe300, 0xe562};
char win_music[4] = { 0x04, 0x24, 0x44, 0x74};
char lose_music[3] = { 0x64, 0x34, 0x08};
char start_music[4] = { 0x02, 0x22, 0x42, 0x54};
char error_sound[2] = { 0x81,0x62};
char input_sound[1] = {0x44};
volatile int sec = 0, tick = 0, m_tick = 0, m_reset = 0, s_tick = 0;

volatile char cf = 100;
 
///////////////// 5*8 FONT  WANNA USE (CHAR - 32)
const unsigned char GLCD_Font[95][5] = {
	{0x00, 0x00, 0x00, 0x00, 0x00},    // 0x20 Space
	{0x00, 0x00, 0x4F, 0x00, 0x00},    // 0x21 !
	{0x00, 0x07, 0x00, 0x07, 0x00},    // 0x22 "
	{0x14, 0x7F, 0x14, 0x7F, 0x14},    // 0x23 #
	{0x24, 0x2A, 0x7F, 0x2A, 0x12},    // 0x24 $
	{0x23, 0x13, 0x08, 0x64, 0x62},    // 0x25 %
	{0x36, 0x49, 0x55, 0x22, 0x50},    // 0x26 &
	{0x00, 0x05, 0x03, 0x00, 0x00},    // 0x27 '
	{0x00, 0x1C, 0x22, 0x41, 0x00},    // 0x28 (
	{0x00, 0x41, 0x22, 0x1C, 0x00},    // 0x29 )
	{0x14, 0x08, 0x3E, 0x08, 0x14},    // 0x2A *
	{0x08, 0x08, 0x3E, 0x08, 0x08},    // 0x2B +
	{0x00, 0x50, 0x30, 0x00, 0x00},    // 0x2C ,
	{0x08, 0x08, 0x08, 0x08, 0x08},    // 0x2D -
	{0x00, 0x60, 0x60, 0x00, 0x00},    // 0x2E .
	{0x20, 0x10, 0x08, 0x04, 0x02},    // 0x2F /
	{0x3E, 0x51, 0x49, 0x45, 0x3E},    // 0x30 0
	{0x00, 0x42, 0x7F, 0x40, 0x00},    // 0x31 1
	{0x42, 0x61, 0x51, 0x49, 0x46},    // 0x32 2
	{0x21, 0x41, 0x45, 0x4B, 0x31},    // 0x33 3
	{0x18, 0x14, 0x12, 0x7F, 0x10},    // 0x34 4
	{0x27, 0x45, 0x45, 0x45, 0x39},    // 0x35 5
	{0x3C, 0x4A, 0x49, 0x49, 0x30},    // 0x36 6
	{0x01, 0x71, 0x09, 0x05, 0x03},    // 0x37 7
	{0x36, 0x49, 0x49, 0x49, 0x36},    // 0x38 8
	{0x06, 0x49, 0x49, 0x29, 0x1E},    // 0x39 9
	{0x00, 0x36, 0x36, 0x00, 0x00},    // 0x3A :
	{0x00, 0x56, 0x36, 0x00, 0x00},    // 0x3B ;
	{0x08, 0x14, 0x22, 0x41, 0x00},    // 0x3C <
	{0x14, 0x14, 0x14, 0x14, 0x14},    // 0x3D =
	{0x00, 0x41, 0x22, 0x14, 0x08},    // 0x3E >
	{0x02, 0x01, 0x51, 0x09, 0x06},    // 0x3F ?
	{0x32, 0x49, 0x79, 0x41, 0x3E},    // 0x40 @
	{0x7E, 0x11, 0x11, 0x11, 0x7E},    // 0x41 A
	{0x7F, 0x49, 0x49, 0x49, 0x36},    // 0x42 B
	{0x3E, 0x41, 0x41, 0x41, 0x22},    // 0x43 C
	{0x7F, 0x41, 0x41, 0x22, 0x1C},    // 0x44 D
	{0x7F, 0x49, 0x49, 0x49, 0x41},    // 0x45 E
	{0x7F, 0x09, 0x09, 0x09, 0x01},    // 0x46 F
	{0x3E, 0x41, 0x49, 0x49, 0x7A},    // 0x47 G
	{0x7F, 0x08, 0x08, 0x08, 0x7F},    // 0x48 H
	{0x00, 0x41, 0x7F, 0x41, 0x00},    // 0x49 I
	{0x20, 0x40, 0x41, 0x3F, 0x01},    // 0x4A J
	{0x7F, 0x08, 0x14, 0x22, 0x41},    // 0x4B K
	{0x7F, 0x40, 0x40, 0x40, 0x40},    // 0x4C L
	{0x7F, 0x02, 0x0C, 0x02, 0x7F},    // 0x4E M
	{0x7F, 0x04, 0x08, 0x10, 0x7F},    // 0x4E N
	{0x3E, 0x41, 0x41, 0x41, 0x3E},    // 0x4F O
	{0x7F, 0x09, 0x09, 0x09, 0x06},    // 0x50 P
	{0x3E, 0x41, 0x51, 0x21, 0x5E},    // 0x51 Q
	{0x7F, 0x09, 0x19, 0x29, 0x46},    // 0x52 R
	{0x26, 0x49, 0x49, 0x49, 0x32},    // 0x53 S
	{0x01, 0x01, 0x7F, 0x01, 0x01},    // 0x54 T
	{0x3F, 0x40, 0x40, 0x40, 0x3F},    // 0x55 U
	{0x1F, 0x20, 0x40, 0x20, 0x1F},    // 0x56 V
	{0x3F, 0x40, 0x38, 0x40, 0x3F},    // 0x57 W
	{0x63, 0x14, 0x08, 0x14, 0x63},    // 0x58 X
	{0x07, 0x08, 0x70, 0x08, 0x07},    // 0x59 Y
	{0x61, 0x51, 0x49, 0x45, 0x43},    // 0x5A Z
	{0x00, 0x7F, 0x41, 0x41, 0x00},    // 0x5B [
	{0x02, 0x04, 0x08, 0x10, 0x20},    // 0x5C (\)
	{0x00, 0x41, 0x41, 0x7F, 0x00},    // 0x5D ]
	{0x04, 0x02, 0x01, 0x02, 0x04},    // 0x5E ^
	{0x40, 0x40, 0x40, 0x40, 0x40},    // 0x5F _
	{0x00, 0x01, 0x02, 0x04, 0x00},    // 0x60 `
	{0x20, 0x54, 0x54, 0x54, 0x78},    // 0x61 a
	{0x7F, 0x48, 0x44, 0x44, 0x38},    // 0x62 b
	{0x38, 0x44, 0x44, 0x44, 0x20},    // 0x63 c
	{0x38, 0x44, 0x44, 0x48, 0x7F},    // 0x64 d
	{0x38, 0x54, 0x54, 0x54, 0x18},    // 0x65 e
	{0x08, 0x7E, 0x09, 0x01, 0x02},    // 0x66 f
	{0x0C, 0x52, 0x52, 0x52, 0x3E},    // 0x67 g
	{0x7F, 0x08, 0x04, 0x04, 0x78},    // 0x68 h
	{0x00, 0x04, 0x7D, 0x00, 0x00},    // 0x69 i
	{0x20, 0x40, 0x44, 0x3D, 0x00},    // 0x6A j
	{0x7F, 0x10, 0x28, 0x44, 0x00},    // 0x6B k
	{0x00, 0x41, 0x7F, 0x40, 0x00},    // 0x6C l
	{0x7C, 0x04, 0x18, 0x04, 0x7C},    // 0x6D m
	{0x7C, 0x08, 0x04, 0x04, 0x78},    // 0x6E n
	{0x38, 0x44, 0x44, 0x44, 0x38},    // 0x6F o
	{0x7C, 0x14, 0x14, 0x14, 0x08},    // 0x70 p
	{0x08, 0x14, 0x14, 0x18, 0x7C},    // 0x71 q
	{0x7C, 0x08, 0x04, 0x04, 0x08},    // 0x72 r
	{0x48, 0x54, 0x54, 0x54, 0x20},    // 0x73 s
	{0x04, 0x3F, 0x44, 0x40, 0x20},    // 0x74 t
	{0x3C, 0x40, 0x40, 0x20, 0x7C},    // 0x75 u
	{0x1C, 0x20, 0x40, 0x20, 0x1C},    // 0x76 v
	{0x3C, 0x40, 0x30, 0x40, 0x3C},    // 0x77 w
	{0x44, 0x28, 0x10, 0x28, 0x44},    // 0x78 x
	{0x0C, 0x50, 0x50, 0x50, 0x3C},    // 0x79 y
	{0x44, 0x64, 0x54, 0x4C, 0x44},    // 0x7A z
	{0x00, 0x08, 0x36, 0x41, 0x00},    // 0x7B {
	{0x00, 0x00, 0x77, 0x00, 0x00},    // 0x7C |
	{0x00, 0x41, 0x36, 0x08, 0x00},    // 0x7D }
	{0x08, 0x04, 0x08, 0x10, 0x08}     // 0x7E ~
};
//////////////////dot Matrix
int dm_data[8][10] = {
	{0x078,0x084,0x102,0x102,0x102,0x102,0x102,0x102,0x084,0x078},
	{0x030,0x030,0x050,0x030,0x030,0x030,0x030,0x030,0x030,0x078},
	{0x078,0x084,0x084,0x004,0x008,0x010,0x020,0x040,0x080,0x0FC},
	{0x078,0x084,0x084,0x008,0x010,0x008,0x008,0x004,0x084,0x078},
	{0x030,0x030,0x050,0x090,0x130,0x130,0x1FE,0x030,0x030,0x030},
	{0x0FC,0x080,0x080,0x080,0x0F8,0x004,0x004,0x004,0x004,0x0F8},
	{0x078,0x080,0x080,0x080,0x0F8,0x084,0x084,0x084,0x084,0x078},
	{0x078,0x084,0x084,0x084,0x084,0x004,0x004,0x004,0x004,0x004},
};

int dmi = 0;

void runDotMatrix(int life)
{
	DM_SEL = 0;
	DM_DATA = dm_data[life][dmi];
	DM_SEL = 1<<dmi;
	dmi++;
	if(dmi>9) dmi = 0;
}

//////////////// GLCD

char LCD_Controller;

void port_init(void)
{
	PORTA = 0x00;
	DDRA  = 0x00;
	PORTB = 0x00;
	DDRB  = 0x00;
	PORTC = 0x00; //m103 output only
	DDRC  = 0x00;
	PORTD = 0x00;
	DDRD  = 0x00;
	PORTE = 0x00;
	DDRE  = 0x00;
	PORTF = 0x00;
	DDRF  = 0x00;
	PORTG = 0x00;
	DDRG  = 0x00;
}

void init_devices(void)
{
	//stop errant interrupts until set up
	//cli(); //disable all interrupts
	XDIV  = 0x00; //xtal divider
	XMCRA = 0x00; //external memory
	port_init();
	MCUCR = 0x80;
	EICRA = 0x00; //extended ext ints
	EICRB = 0x00; //extended ext ints
	EIMSK = 0x00;
	TIMSK = 0x45; //timer interrupt sources
	ETIMSK = 0x00; //extended timer interrupt sources
	sei(); //re-enable interrupts
	//all peripherals are now initialized
	 timer0_init();
	 timer1_init();
}

void delay_us(unsigned int dt)
{
	unsigned int dc;
	for(dc=0;dc<dt;dc++)
	{
		asm volatile("nop"::);
		asm volatile("nop"::);
		asm volatile("nop"::);
	}
}
 void timer0_init(void){
	 TCCR0= 0x00;
	 ASSR= 0x00;
	 TCNT0= 0xCE;
	 OCR0= 0x32;
	 TCCR0= 0x03;
 }

 void timer1_init(void)
 {
	 TCCR1B = 0x00; //stop
	 TCNT1H = 0xC4; //setup
	 TCNT1L = 0x40;
	 OCR1AH = 0x3B;
	 OCR1AL = 0xC0;
	 OCR1BH = 0x3B;
	 OCR1BL = 0xC0;
	 OCR1CH = 0x3B;
	 OCR1CL = 0xC0;
	 ICR1H = 0x3B;
	 ICR1L = 0xC0;
	 TCCR1A = 0x00;
	 TCCR1B = 0x01; //start Timer
 } // Normal mode, No prescaling
 
void delay(unsigned int dt)
{
	unsigned int dc;
	for(dc=0;dc<dt;dc++)
	{
		delay_us(1100);
	}
}

 ISR(TIMER0_OVF_vect)
 {
	 runDotMatrix(8-x);
	 TCNT0 = 0xCE;
	 tick++;
	 if(tick>10000){
		 tick = 0;
	 }
	 m_tick ++;
	 if(m_reset == 1){
		 m_tick = 0;
		 m_reset =0;
	 }
 }

void start_sound(void)
{
	char i;
	int period;
	for(i=0; i<4; i++){
		cf = (start_music[i]>>4) & 0x0f;
		period = (start_music[i] & 0x0f) * 1000;
		m_reset = 1;
		while(m_reset == 1);
		while(m_tick < period);
	}
	cf = 100;
}
void win_sound(void)
{
	  char i;
	  int period;
	  for(i=0; i< 4; i++){
		  cf = (win_music[i]>>4) & 0x0f;
		  period = (win_music[i] & 0x0f) * 1000;
		  m_reset = 1;
		  while(m_reset == 1);
		  while(m_tick < period);
	  }
	  cf = 100;
}

void lose_sound(void)
{
	char i;
	int period;
	for(i=0; i< 3; i++){
		cf = (lose_music[i]>>4) & 0x0f;
		period = (lose_music[i] & 0x0f) * 1000;
		m_reset = 1;
		while(m_reset == 1);
		while(m_tick < period);
	}
	cf = 100;
}
void inputsound(void)
{
	char i;
	int period;
	for(i=0; i<1; i++){
		cf = (input_sound[i]>>4) & 0x0f;
		period = (input_sound[i] & 0x0f) * 1000;
		m_reset = 1;
		while(m_reset == 1);
		while(m_tick < period);
	}
	cf = 100;
}
void errorsound(void)
{
	char i;
	int period;
	for(i=0; i<2; i++){
		cf = (error_sound[i]>>4) & 0x0f;
		period = (error_sound[i] & 0x0f) * 1000;
		m_reset = 1;
		while(m_reset == 1);
		while(m_tick < period);
	}
	cf = 100;
}

ISR(TIMER1_OVF_vect)
{
	TCNT1H = ((freq[cf]>>8) & 0xff);
	TCNT1L = ((freq[cf]) & 0xff);
	if(cf < 9){
		PORTG = PORTG ^ (0x10);
		}else{
		PORTG = 0;
	}
}
 
void E_Pulse(void){
	EN_ON;
	delay(delaytime);
	EN_OFF;
}

void BackLight(int onoff){
	if(onoff==ON)
	LCD_Control |= 0x40;
	else
	LCD_Control &= 0xbf;
}


void DataInput(char data){
	RS_OFF;
	RW_OFF;
	LCD_Control = data;
	E_Pulse();
}



void Reset(void){
	DataInput(0x10);
}



void Write_Control(char CS, char cmd){
	delay(delaytime);
	EN_OFF;
	delay(delaytime);
	RS_OFF;
	delay(delaytime);
	RW_OFF;
	delay(delaytime);
	LCD_Control = CS;
	delay(delaytime);
	EN_ON;
	delay(delaytime);
	LCD_Data = cmd;
	delay(delaytime);
	EN_OFF;
	delay(delaytime);
}

void Write_Data(char CS, char data){
	delay(delaytime);
	EN_OFF;
	delay(delaytime);
	RS_ON;
	delay(delaytime);
	RW_OFF;
	delay(delaytime);
	LCD_Control = CS;
	delay(delaytime);
	EN_ON;
	delay(delaytime);
	LCD_Data = data;
	delay(delaytime);
	EN_OFF;
	delay(delaytime);
}

void Lcd_Clear(){
	int i, j, x, y;
	LCD_Control = 0x00;
	Write_Control(CS_ALL, 0x3f);
	Write_Control(CS_ALL, 0xc0);
	x=0xb8;
	y=0x40;
	for(i=0; i<=7; i++){
		Write_Control(CS_ALL, x);
		Write_Control(CS_ALL, y);
		for(j=0; j<64; j++){
			Write_Data(CS_ALL, 0x00);
		}
		x++;
	}
}

void Lcd_init(){
	RESET_OFF;
	RESET_ON;
	CS_ALL;
	Write_Control(CS_ALL, 0x3f);
	Write_Control(CS_ALL, 0xc0);
	Write_Control(CS_ALL, 0xb8);
	Write_Control(CS_ALL, 0x40);
}

void GLCD_gotoXY(unsigned char x, unsigned char y)		//커서 좌표 이동
{
	Write_Control(CS_ALL,0xb8 + x);
	Write_Control(CS_ALL,0x40 + y);
}


void GLCD_character (char data, char CS)				//CS에 입력된 Char 출력
{
	if (CS  == 1) CS = CS2_OFF;
	else if (CS == 2) CS = CS1_OFF;
	for (int icnt = 0; icnt < 5; icnt ++)
	{
		Write_Data(CS, GLCD_Font[data-32][icnt]);
	}
}


void GLCD_String(unsigned char *str, char CS)			//CS에 입력된 str 출력
{
	while (*str)
	{
		GLCD_character(*str, CS);
		str++;
	}
}

void print_GLCD(unsigned char x)				//Print UserInput at CS1, Print SB statuse at CS2
{
	GLCD_gotoXY(x,0);
	Write_Control(CS_ALL, 0xb8+x);
	Write_Control(CS_ALL, 0x40);
	GLCD_character(x + '0', 1);
	GLCD_String(" : ", 1);
	for (int i = 0 ; i < 4; i ++)
	{
		GLCD_character(user_answer[i], 1);
	}
	Write_Control(CS_ALL, 0xb8+x);			//뺴도 되는지 확인
	Write_Control(CS_ALL, 0x40);				//마찬가지
	GLCD_String("S: ", 2);
	GLCD_character(strike + '0', 2);
	GLCD_String(" B: ", 2);
	GLCD_character(ball + '0', 2);
}

void WinGame(void)							//승리경우
{
	usart0_transstring("\nWIN\n");
	Lcd_Clear();
	GLCD_gotoXY(2, 4);
	GLCD_String("Congarturati", 1);
	GLCD_gotoXY(2, 0);
	GLCD_String("on !!", 2);
	GLCD_gotoXY(4, 4);
	GLCD_String("You Win!! :)", 1);
	GLCD_gotoXY(6, 0);
	GLCD_String("<PRESS PINB",1);
	GLCD_gotoXY(6,0);
	GLCD_String("to Restart!>",2);
	GLCD_gotoXY(0,0);
	GLCD_String("<SCORE: ",1);
	GLCD_character('9' - x,1);
	GLCD_character('>',1);
	win_sound();
}

void LoseGame(void)							//패배경우
{
	usart0_transstring("\nLOSE\n");
	Lcd_Clear();
	GLCD_gotoXY(2, 3);
	GLCD_String("YOU LOSE :(", 1);
	GLCD_gotoXY(5, 0);
	GLCD_String("<PRESS PINB",1);
	GLCD_gotoXY(5,0);
	GLCD_String("to Restart!>",2);
	GLCD_gotoXY(3,14);
	GLCD_String("Answer is ", 1);
	GLCD_gotoXY(3,0);
	GLCD_character(answer[0], 2);
	GLCD_character(answer[1], 2);
	GLCD_character(answer[2], 2);
	GLCD_character(answer[3], 2);
	
	lose_sound();
}
/////////////////////////////////////USART

unsigned char usart0_receive()
{
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void usart0_transmit(unsigned char data)
{
	while(!(UCSR0A & 0x20));
	UDR0 = data;
}

void usart0_init(void)
{
	UCSR0A = 0x00;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	UBRR0H = ((USARTBoudRate)>>8) & 0x0F;
	UBRR0L = (USARTBoudRate) & 0xFF;
}

void usart0_transstring(char *str)
{
	while(*str)
	{
		usart0_transmit(*str);
		str++;
	}
}

/////////////////////////////////////////RANDOM
int readADC( char ch )
{
	int temp;
	ADMUX = (char)ch | 0xC0;	//1100_0000		[7:6]2.56을 참조전압, 우측정렬, [4:3]차동 컨버젼,[2:0] 채널
	ADCSRA=ADCSRA|(0x01<<6);	//컨버전 시작
	temp = ADCL;				//ADCL값 저장
	return ((ADCH<<8)|temp);	//High, Low 합치기
}

void initADC(void)
{
	ADCSRA = ADCSRA | (0x01<<7) | (0x07);	//1000_1111	EN/StartConver/Start->Auto/Endflag/interen/[2:0]clk
}

void initAnswer(void)
{
	srand(readADC(0));
	
	int i = 0;
	int check;
	while (i < 4)
	{
		answer[i] = rand() % 10 + '0';
		for (check = 0; check < i; check ++)
		{
			if (answer[check] == answer[i])
			break;
		}
		if (check == i) i ++;
	}
}


////////////////////////////BASEBALL GAME

void usart0_printSB(unsigned strike, unsigned ball)
{
	if(strike == 4) 
	{
		usart0_transstring("WIN");
		return ; 
	}
	usart0_transmit('s');
	usart0_transmit(strike+'0');
	usart0_transmit('b');
	usart0_transmit(ball+'0');
	usart0_transmit('\n');
}

void check_answer(void)
{
	for(int i = 0; i < 4; i ++)
	for(int check = 0; check < 4; check++)
	{
		if (user_answer[i] == answer[check])
		{
			if (i == check)
			strike++;
			else
			ball ++;
		}
	}
}

unsigned char check_input(void)							//IF INPUT ERROR return 0, else return 1
{
	int cnt = 0;
	for(int i = 0; i < 4; i ++)
	{
		if (user_answer[i] < '0' || user_answer[i] >'9') return 2;
		for(int check = 0; check < 4; check++)
		{
			if (user_answer[i] == user_answer[check])
			{
				cnt ++;
			}
			if (cnt > 4){
				return 1;
			}
		}
	}
	return 0;
}


void recieve_input(void)
{
	for(int i = 0; i < 4 ; i++)
	{
		user_answer[i] = usart0_receive();
		usart0_transmit(user_answer[i]);
	}
	usart0_transmit('\n');
}

void start_screen()
{
	GLCD_gotoXY(0,0);
	GLCD_String("<Number Base", 1);
	GLCD_gotoXY(0,0);
	GLCD_String(" Ball GAME!>", 2);
	GLCD_gotoXY(1,0);
	GLCD_String("How To Play?", 1);
	GLCD_gotoXY(2,9);
	GLCD_String("INPUT A and", 1);
	GLCD_gotoXY(2,0);
	GLCD_String(" 4number!", 2);
	GLCD_gotoXY(3,9);
	GLCD_String("IF you Wron", 1);
	GLCD_gotoXY(3,0);
	GLCD_String("g S,B return", 2);
	GLCD_gotoXY(4,9);
	GLCD_String("You'r Life ", 1);
	GLCD_gotoXY(4,0);
	GLCD_String("is 8", 2);
	GLCD_gotoXY(5,9);
	GLCD_String("GOOD LUCK!", 1);
	GLCD_gotoXY(5,0);
	GLCD_String("HAVE FUN :)", 2);
	GLCD_gotoXY(7,4);
	GLCD_String(" <PRESS PIN ", 1);
	GLCD_gotoXY(7,0);
	GLCD_String("B to START>", 2);
	
}
void main (void)
{	
	init_devices();
	initADC();
	usart0_init();
	

	initAnswer();
	unsigned char temp, index;
	usart0_init();

	while (1)
	{
		START:
		start_sound();
		Lcd_init();
		BackLight(ON);
		Lcd_Clear();
		start_screen();
		initADC();
		initAnswer();
		x = 0;
		while(1)
		{
			if(PINB & 0x01)
			{
				inputsound();
				Lcd_Clear();
				break;
			}
		}
		unsigned char error_flag = 0;
		while( x < 9)									//ingame
		{
			index = 0;
			temp = usart0_receive();
			usart0_transmit(temp);
			if (temp == 'A')							//A ????? 4??????? user_answer[]?? ???? ????
			{
				recieve_input();
				error_flag = check_input();
				if(error_flag == 0) 
				{
					inputsound();
					strike = 0;
					ball = 0;
					check_answer();
					usart0_printSB(strike, ball);
					print_GLCD(x);
					if( strike == 4) 
					{
						WinGame();
						break;
					}
						
					x++;
				}
				else if(error_flag == 1) {
					usart0_transstring ("\nERORR CASE! SAME NUMBER!\n");
					errorsound();
				}
				else if(error_flag == 2) {
					usart0_transstring ("\nERORR CASE! Input Can't be alphabet\n");
					errorsound();
				}
			}
		}
		if(x > 8 ) LoseGame();
		
		while(1)
		{
			if(PINB & 0x01) goto START;
		}
		
	}
}
