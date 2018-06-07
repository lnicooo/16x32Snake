#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart/uart.h"
#include "mma8452/mma8452.h"
#include "snek/snek.h"

volatile uint32_t buffer[16];

#define OE PB0
#define R PB1
#define G PB2
#define SCK PB4
#define SCL PB3

volatile char color;

void timer_init(){

    TCCR1B |= (1 << WGM12) |(1 << CS10) |(1 << CS12); //CTC Mode no prescaler

		TCNT1 = 0;

    OCR1A = 1400;

    TIMSK1 |= (1<<OCIE1A);

    sei(); //enable interrupt

}

void ck_tick(){
	PORTB |= (1<<SCK);
	PORTB &= ~(1<<SCK);
	PORTB |= (1<<SCK);
}

void ck_latch(){
	PORTB |= (1<<SCL);
	PORTB &= ~(1<<SCL);
	PORTB |= (1<<SCL);
}


ISR(TIMER1_COMPA_vect){
  uint8_t x;

	//get orientation
	x = mma_get_PL();

	if(x != 0x40){
		//snek change diraction
		snek_change_dir(x);
	}

	//snake clear secreen
	for(x=0;x<16;x++){
		buffer[x] = (uint32_t)0;
	}

	//move snake
	snek_shift();

	for(x=0;x<snek_size;x++){
		
		buffer[snek[x].pix_y] |= (uint32_t)((uint32_t)1 << snek[x].pix_x);

	}

	

}

void changeColor(){

  if(color==R){
    PORTB |= (1<<R);
    color=G;
  }
  else{
    PORTB |= (1<<G);
    color=R;
  }
}

int main(){
	//  setup OUTPUTS
	DDRD = 0xF0;
	DDRB = 0X1F;

	uint8_t y;
	uint8_t x;

	srand(3);
	timer_init();
  uart_init();
	snek_init(5,4,4);
	mma_init();

  stdout = &uart_output;
  stdin  = &uart_input;

  printf("Hey\n");

  PORTB |= (1<<R);
  color=G;

	while(1){

		//send rows and columns to display
		for(y=0;y<16;y++){

      for(x=0;x<32;x++){

        if(buffer[y] & ((uint32_t)1<<x)){

    			PORTB &= ~(1<<color);

    		}
    		else{

    			PORTB |= (1<<color);

    		}

    		ck_tick();

      }

			PORTB |= (1<<OE);

			PORTD = (y<<4);

			ck_latch();

			PORTB &= ~(1<<OE);
		}

	}

}
