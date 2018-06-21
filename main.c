#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "LCD-fonts/8x8_horizontal_LSB_2.h"
#include "uart/uart.h"
#include "mma8452/mma8452.h"
#include "snek/snek.h"

#define OE PB0
#define R PB1
#define G PB2
#define SCK PB4
#define SCL PB3

volatile uint32_t buffer[16];

uint8_t ee_max_points EEMEM = 0;
bool pause = false;
bool end = false;

volatile char color;

volatile uint8_t fase=1;

void timer_init(){

	//Timer 1;

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

ISR(TIMER1_COMPA_vect){
  	uint8_t x;
	uint8_t max_points;

	if(fase == 1){
		max_points = eeprom_read_byte(&ee_max_points);

		for(x=0;x<16;x++){
			buffer[x] = (uint32_t)0;
		}

		for(x=4;x<12;x++){
			buffer[x] |= ((uint32_t)pgm_read_byte(&font[max_points/10][x-4])<<10);
			buffer[x] |= ((uint32_t)pgm_read_byte(&font[max_points%10][x-4])<<14);
			
  		}
	}
		
	if(mma_get_tap()){
		if(pause == true) 
			pause = false;	
		else
			pause = true;
	}

	if(!pause || !end){
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

		//draw food
		buffer[food.pix_y] |= ((uint32_t)1 << food.pix_x); 

		//draw snake
		for(x=0;x<snek_size;x++){

			buffer[snek[x].pix_y] |= ((uint32_t)1 << snek[x].pix_x);

		}
		
		if(snek[0].pix_x == food.pix_x && snek[0].pix_y == food.pix_y){
			snek_gen_food();
			snek_size++;
		}

		if(snek[0].pix_x<0 || snek[0].pix_x>32 || snek[0].pix_y<0 || snek[0].pix_y>16){
			end = true;
		}
			
	}

	//pause
	else{
		
		for(x=0;x<16;x++){
			buffer[x] = (uint32_t)0;
		}

		for(x=4;x<12;x++){
			buffer[x] |= ((uint32_t)pgm_read_byte(&font['|'][x-4])<<10);
			buffer[x] |= ((uint32_t)pgm_read_byte(&font['|'][x-4])<<14);
			
  		}
	}

	if(end){
		
		for(x=4;x<12;x++){
			buffer[x] |= ((uint32_t)pgm_read_byte(&font['E'][x-4])<<0);
			buffer[x] |= ((uint32_t)pgm_read_byte(&font['n'][x-4])<<8);
			buffer[x] |= ((uint32_t)pgm_read_byte(&font['d'][x-4])<<16);
			
  		}
		//save biggest score
		max_points = eeprom_read_byte(&ee_max_points);

		if((snek_size*fase) > max_points)
			eeprom_update_byte(&ee_max_points,(snek_size*fase));
		//wait for tap
		if(mma_get_tap())
			end != end;

		fase=1;
	}

	if(snek_size > MAX_SIZE){

        snek_size = 3;
		fase++;

    }

	
}



int main(){
	//  setup OUTPUTS
	DDRD = 0xF0;
	DDRB = 0X1F;

	uint8_t y;
	uint8_t x;

	timer_init();
	uart_init();
	snek_init(10,4,4);
	mma_init();
	snek_gen_food();

	stdout = &uart_output;
	stdin  = &uart_input;

	printf("Hey\n");

	PORTB |= (1<<R);
	color=G;

	actual_dir = LEFT;
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
