/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; 

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

unsigned char threeLEDs;
unsigned char blinkingLEDs;
unsigned char tempA;

unsigned char arr[3] = {0x01, 0x02, 0x04};
unsigned int i; //this is for interating through the array; allows for easy light switching
enum ThreeLEDs_States {threeStart, threeNext} ThreeLEDs_state;
void ThreeLEDs_Tick(){
	switch(ThreeLEDs_state){
		case threeStart:
			ThreeLEDs_state = threeNext;
			i = 0;
			threeLEDs = arr[i];
			i++;
			break;

		case threeNext:
			ThreeLEDs_state = threeNext;

			threeLEDs = arr[i];
			if(i < 2){
				i++;
			}
			else{
				i = 0;
			}
			break;

		default:
			break;
	}
}

enum BlinkingLEDs_States {blinkStart, Blink1, Blink2} BlinkingLEDs_state;
void BlinkingLEDs_Tick(){
	switch(BlinkingLEDs_state){
		case blinkStart:
			BlinkingLEDs_state = Blink1;
			break;

		case Blink1:
			BlinkingLEDs_state = Blink2;
			break;

		case Blink2:
			BlinkingLEDs_state = Blink1;
			break;

		default:
			break;
	}

	switch(BlinkingLEDs_state){
		case Blink1:
			blinkingLEDs = 0;
			break;

		case Blink2:
			blinkingLEDs = 1;
			break;

		default:
			break;
	}
}

enum CombinedLEDs_States {combinedStart, combinedNext} CombinedLEDs_state;
void CombinedLEDs_Tick(){
	switch(CombinedLEDs_state){
		case combinedStart:
			CombinedLEDs_state = combinedNext;
			tempA = 0;
			break;

		case combinedNext:
			CombinedLEDs_state = combinedNext;
			break;

		default:
			break;
	}

	switch(CombinedLEDs_state){
		case combinedNext:
			tempA = (threeLEDs & 0x07) | (blinkingLEDs << 3);
			break;

		default:
			break;
	}
}

void TimerOn() {
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80; 
}

void TimerOff() {
	TCCR1B = 0x00; 
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;	

	if (_avr_timer_cntcurr == 0) {
		TimerISR();	
		_avr_timer_cntcurr = _avr_timer_M;

	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
     
    unsigned long ThreeLEDs_elapsedTime = 0;
    unsigned long BlinkingLEDs_elapsedTime = 0;
    unsigned long CombinedLEDs_elapsedTime = 0;
    const unsigned long timerPeriod = 1;

    ThreeLEDs_state = threeStart;
    BlinkingLEDs_state = blinkStart;
    CombinedLEDs_state = combinedStart;

    tempA = 0x00;

    TimerSet(timerPeriod);
    TimerOn();
    /* Insert your solution below */
    while (1) {
	    if(ThreeLEDs_elapsedTime >= 1000){
	    	ThreeLEDs_Tick();
		ThreeLEDs_elapsedTime = 0;
	    }
	    if(BlinkingLEDs_elapsedTime >= 1000){
		BlinkingLEDs_Tick();
		BlinkingLEDs_elapsedTime = 0;
	    }
	    if(CombinedLEDs_elapsedTime >= 1000){
	    	CombinedLEDs_Tick();
		CombinedLEDs_elapsedTime = 0;
	    }

	    PORTB = tempA;

	    while(!TimerFlag) {}
	    TimerFlag = 0;
	    ThreeLEDs_elapsedTime += timerPeriod;
	    BlinkingLEDs_elapsedTime += timerPeriod;
	    CombinedLEDs_elapsedTime += timerPeriod;

    }
    return 1;
}
