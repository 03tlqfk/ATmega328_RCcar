#if 1
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

unsigned int ch1_value1=0;
unsigned int ch1_value2=0;
unsigned int ch1_value3=0;
unsigned int ch1_temp_value3 = 0;

unsigned int ch2_value1=0;
unsigned int ch2_value2=0;
unsigned int ch2_value3=0;
unsigned int ch2_temp_value3 = 0;

unsigned int ch3_value1=0;
unsigned int ch3_value2=0;
unsigned int ch3_value3=0;
unsigned int ch3_temp_value3 = 0;

unsigned int count = 0;
unsigned int LED_cnt = 0;
unsigned int blink_cnt = 0;
unsigned short is_turnon = 0;

void INIT_PORT(void)   //pin setting
{
	DDRB = 0x00;  
	PORTB = 0x01;
	
	DDRC = 0x00;  
	PORTC = 0x01;
	
	DDRD = 0x00;   
	PORTD = 0x04;
	
	DDRB |= (1<<PINB3);  
	DDRD |= (1<<PIND3);
	
	DDRD |= (1<<PIND7); 
	
	DDRD |= (1<<PIND6); 

}

void INIT_PCINT(void)   //pin interrupt setting
{
	PCICR |= (1<<PCIE0) | (1<<PCIE1) | (1<<PCIE2);  
	PCMSK0 |= (1<<PCINT0);  
	PCMSK1 |= (1<<PCINT8);   
	PCMSK2 |= (1<<PCINT18);  
}

void Timer_init(void)   //timer setting
{
	TCCR0B |= /*(1 << CS00) |*/ (1 << CS00); 
	
	TCCR2A |= (1<<WGM21) | (1<<WGM20);
	TCCR2A |= (1<<COM2A1) | (1<<COM2B1);
	TCCR2B |= (1<<CS22);
}

ISR(TIMER0_COMPA_vect)   //timer interrupt
{
	count++;
	TCNT0 = 0;

}

ISR(PCINT0_vect)   //pin interrupt
{
	if(PINB & 0x01)
	{
		ch1_value1 = count;
	}
	else
	{
		ch1_value2 = count;
	}
	ch1_value3 = ch1_value2-ch1_value1;
	
	if(ch1_value3 > 1000)
	{
		ch1_value3 = ch1_temp_value3;
	}
	else
	{
		ch1_temp_value3 = ch1_value3;
	}
}

ISR(PCINT1_vect)   //pin interrupt
{
	if(PINC & 0x01)
	{
		ch2_value1 = count;
	}
	else
	{
		ch2_value2 = count;
	}
	ch2_value3 = ch2_value2-ch2_value1;
	
	if(ch2_value3 > 1000)
	{
		ch2_value3 = ch2_temp_value3;
	}
	else
	{
		ch2_temp_value3 = ch2_value3;
	}
}

ISR(PCINT2_vect)   //pin interrupt
{
	if(PIND & 0x04)
	{
		ch3_value1 = count;
	}
	else
	{
		ch3_value2 = count;
	}
	ch3_value3 = ch3_value2-ch3_value1;
	
	if(ch3_value3 > 1000)
	{
		ch3_value3 = ch3_temp_value3;
	}
	else
	{
		ch3_temp_value3 = ch3_value3;
	}
}

void motor_ctrl()
{
	if(ch1_value3 > 298 && ch1_value3 < 395)   //motor control
	{
		OCR2A = 255 * (ch1_value3 - 290)/100;
		OCR2B = 0;
	}
	else if(ch1_value3 < 285 && ch1_value3 > 185)
	{
		OCR2B = 255 * (280 - ch1_value3)/100;
		OCR2A = 0;
	}
	else
	{
		OCR2A = 0;
		OCR2B = 0;
		PORTB &= ~ (1<<PINB3);
		PORTD &= ~ (1<<PIND3);
	}
}

void buzzer_ctrl()
{
	if(ch2_value3 >300 )   //buzzer
	{
		PORTD |= (1<<PORTD7);
	}
	else
	{
		PORTD &= ~(1<<PORTD7);
	}
}

void led_val()
{
	LED_cnt = count;
	if(LED_cnt>40000)
	{
		blink_cnt++;
		LED_cnt=0;
	}
}

void led_blink()
{
	if(ch2_value3==0)
	{
		if(blink_cnt>2)
		{
			if(is_turnon)
			{
				PORTD |= (1<<PORTD6);
				is_turnon = 0;
			}
			else
			{
				PORTD &= ~(1<<PORTD6);
				is_turnon = 1;
			}
			blink_cnt = 0;
		}
		
	}
	else
	{
		PORTD |= 0x40;
	}
}

int main(void)
{
	INIT_PORT();
	Timer_init();
	INIT_PCINT();

	OCR0A = 50;
	
	TIMSK0 |= (1<<OCIE0A);
	
	sei();
	
	
	while (1)
	{

		motor_ctrl();
		
		buzzer_ctrl();
		
		led_val();
		led_blink();
		
		ch1_value3=0;
		ch2_value3=0;
		ch3_value3=0;
		
		_delay_ms(50);


	}
}

#endif


