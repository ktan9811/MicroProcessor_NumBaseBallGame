#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned int freq[9] = {0xc440, 0xcac4, 0xd095, 0xd33b, 0xd817, 0xdc7e, 0xe100, 0xe300, 0xe562};
char win_music[4] = { 0x04, 0x24, 0x44, 0x74};
char lose_music[3] = { 0x64, 0x34, 0x08};
char start_music[4] = { 0x02, 0x22, 0x42, 0x54};
char error_sound[2] = { 0x81,0x62};
char input_sound[1] = {0x44};
volatile int sec = 0, tick = 0, m_tick = 0, m_reset = 0, s_tick = 0;

volatile char cf = 100;


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
