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

uint16_t ee_max_points EEMEM = 0;
uint16_t points;

bool pause = false;
bool end = false;

volatile char color;

volatile uint8_t fase=1;

void timer_init(){

	//Timer 1;

    TCCR1B |= (1 << WGM12) |(1 << CS10) |(1 << CS12); //CTC Mode 64 prescaler

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
	uint16_t max_points;
	
	if(mma_get_tap()){
		if(pause)
			pause = false;
		else
			pause = true;
	}

	if(!pause){
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

		for(x=0;x<snek_size;x++){
			if(snek[x].pix_x<0 || snek[x].pix_x>32 || snek[x].pix_y<0 || snek[x].pix_y>16){
			end = true;
			}
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

	//end of 
	if(end){
		
		points = (snek_size*fase);
		//save biggest score
		max_points = eeprom_read_word(&ee_max_points);

		if(points > max_points)
			eeprom_update_word(&ee_max_points,points);
		
		for(x=0;x<16;x++){
			buffer[x] = (uint32_t)0;
		}

		for(x=4;x<12;x++){
			
			buffer[x] |= ((uint32_t)pgm_read_byte(&font[(points/100)+48][x-4]));
			buffer[x] |= ((uint32_t)pgm_read_byte(&font[(points/10)+48][x-4])<<8);
			buffer[x] |= ((uint32_t)pgm_read_byte(&font[(points%10)+48][x-4])<<16);
  		}
		
		pause=true;
	}

	if(snek_size >= 5){

        snek_size = 2;

		fase++;

		OCR1A = 2000-(100*fase);

   		TIMSK1 |= (1<<OCIE1A);	

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
	snek_init(2,4,4);
	mma_init();
	snek_gen_food();

	stdout = &uart_output;
	stdin  = &uart_input;

	printf("Hey\n");

	if(!(TIMSK1&0x32)){
		for(x=4;x<12;x++){
			
			buffer[x] |= ((uint32_t)pgm_read_byte(&font['E'][x-4]));
			buffer[x] |= ((uint32_t)pgm_read_byte(&font[49][x-4])<<8);
			
  		}
		cli(); 
	}

	if(mma_available()){

		for(x=4;x<12;x++){
			
			buffer[x] |= ((uint32_t)pgm_read_byte(&font['E'][x-4]));
			buffer[x] |= ((uint32_t)pgm_read_byte(&font[50][x-4])<<8);
			
  		}
		cli(); 
	}

	PORTB |= (1<<R);
	color=G;
	pause = true;
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
