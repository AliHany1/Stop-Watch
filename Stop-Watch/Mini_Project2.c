/*
 ============================================================================
 Name        : C_Project.c
 Author      : Ali Hany Ahmed
 Description : Mini Project 2
 ============================================================================
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;
void Timer1_CTC_Init(void)
{
	TCNT1 = 0;
	OCR1A = 1000;                        /* Set the Compare value to 1sec */
	TIMSK |= (1<<OCIE1A);                   /* Enable Timer1 Compare A Interrupt */
	TCCR1A = (1<<FOC1A);
	/* Configure timer control register TCCR1B CTC Mode (Number 4) & Pre-scaler of 1024 CS10 = 1 CS11 = 0 CS12 = 1 */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
	SREG |=(1<<7);
}
ISR(TIMER1_COMPA_vect)
{
	seconds ++;
	if (seconds == 60){
		seconds = 0;
		minutes ++;
	}
	if (minutes == 60){
		seconds = 0;
		minutes = 0;
		hours ++;
	}
	if (hours == 24)
	{
		seconds = 0;
		minutes = 0;
		hours = 0;
	}

}
void INT2_Init_Resume(void)
{
	DDRB &= (~(1<<PB2));
	PORTB |= (1<<PB2);                      // Enable the internal pull up resistor at PB2 pin
	GICR |= (1<<INT2);                      // Enable external interrupt pin INT2
	MCUCSR &= (~(1<<ISC2));                 // Trigger INT2 with the falling edge
	SREG |=(1<<7);
}
ISR(INT2_vect)
{
	TCCR1B = (1<<WGM12)|(1<<CS10)|(1<<CS12);
}
void INT1_Init_Pause(void)
{
	DDRD  &= (~(1<<PD3));
	GICR  |= (1<<INT1);                     // Enable external interrupt pin INT1
	MCUCR |= (1<<ISC11) | (1<<ISC10);       // Trigger INT1 with the raising edge
	SREG |=(1<<7);
}
ISR(INT1_vect)
{
	/* This ISR is to stop the clock by switching timer1 CTC mode, interrupt OFF */
	TCCR1B &= ~(1<<CS10)&~(1<<CS11)&~(1<<CS12);
}
void INT0_Init_Reset(void)
{
	DDRD  &= (~(1<<PD2));
	PORTD |= (1<<PD2);                  // Enable the internal pull up resistor at PD2 pin
	GICR  |= (1<<INT0);                 // Enable external interrupt pin INT0
	MCUCR |= (1<<ISC01);                // Trigger INT0 with the falling edge
	MCUCR &= (~(1<<ISC00));
	SREG |=(1<<7);
}
ISR(INT0_vect)
{
	seconds = 0;
	minutes = 0;
	hours = 0 ;
}
int main(void)
{
	DDRA = 0x3F;
	DDRC = 0x0F;
	PORTA = 0x3F;
	PORTC = 0x00;
	INT0_Init_Reset();
	INT1_Init_Pause();
	INT2_Init_Resume();
	Timer1_CTC_Init();
	while(1)
	{
		PORTA = (1<<0);
		PORTC = seconds % 10;
		_delay_ms(2);
		PORTA = (1<<1);
		PORTC = seconds / 10;
		_delay_ms(2);
		PORTA = (1<<2);
		PORTC = minutes % 10;
		_delay_ms(2);
		PORTA = (1<<3);
		PORTC = minutes / 10;
		_delay_ms(2);
		PORTA = (1<<4);
		PORTC = hours % 10;
		_delay_ms(2);
		PORTA = (1<<5);
		PORTC = hours / 10;
		_delay_ms(2);
	}
}
