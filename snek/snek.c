#include "snek.h"


struct snek_pix snek[MAX_SIZE];
volatile uint8_t actual_dir;
volatile uint8_t last_dir;
volatile uint8_t snek_size;
struct food_pix food;

void snek_init(uint8_t init_size, uint8_t x, uint8_t y){
    uint8_t i;

    snek[0].pix_y = y;
    snek[0].pix_x = x;

    snek_size = init_size;

    for(i=1;i<=init_size;i++){
        snek[i].pix_y = snek[i+1].pix_y + 1;
        snek[i].pix_x = snek[0].pix_x;
    }

}

void snek_shift(){
    uint8_t i;

    //shift body
    for(i=snek_size;i>0;i--){
        snek[i] = snek[i-1];
    }   

    //head direction
    switch (actual_dir){
    case UP:
        snek[0].pix_y = snek[1].pix_y + 1;
        snek[0].pix_x = snek[1].pix_x;
    break;

    case DOWN:
        snek[0].pix_y = snek[1].pix_y - 1;
        snek[0].pix_x = snek[1].pix_x;
    break;

    case LEFT:
        snek[0].pix_y = snek[1].pix_y;
        snek[0].pix_x = snek[1].pix_x + 1;
    break;

    case RIGTH:
        snek[0].pix_y = snek[1].pix_y;
        snek[0].pix_x = snek[1].pix_x - 1;
    break;
    }

    last_dir = actual_dir;

    /*
    for(i=0;i<snek_size;i++){

        
        if(snek[i].pix_x >= 32)
            snek[i].pix_x = 0;
        if(snek[i].pix_y >= 16)
            snek[i].pix_y = 0;

        if(snek[i].pix_x < 0)
            snek[i].pix_x = 32;
        if(snek[i].pix_y < 0)
            snek[i].pix_y = 16;
        

    }
    */

}

void snek_change_dir(uint8_t dir){
    
    actual_dir = dir;

}


void snek_gen_food(){

    food.pix_x = rand()%33;

    food.pix_y = rand()%17;

}