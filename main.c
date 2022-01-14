#pragma once
#include "adc.h"
#include "dotmatrix.h"
#include "font.h"
#include "glcd.h"
#include "init.h"
#include "isr.h"
#include "sound.h"
#include "usart.h"

#include <avr/io.h>
#include <avr/interrupt.h>

////////////////// FOR GAME
unsigned char answer[4];
unsigned char user_answer[4];
unsigned char strike = 0;
unsigned char ball = 0;
unsigned char x = 0;

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

void delay(unsigned int dt)
{
	unsigned int dc;
	for(dc=0;dc<dt;dc++)
	{
		delay_us(1100);
	}
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
	return 0;
}
