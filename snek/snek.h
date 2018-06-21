#include <stdint.h>
#include <stdlib.h>

#define MAX_SIZE 15

struct snek_pix{
    int8_t pix_x;
    int8_t pix_y;
};

struct food_pix{
    uint8_t pix_x;
    uint8_t pix_y;
};

enum{
    RIGTH,
    LEFT,
    UP,
    DOWN
    
};

extern struct food_pix food;
extern struct snek_pix snek [MAX_SIZE];
extern volatile uint8_t snek_size;
extern volatile uint8_t actual_dir;


void snek_init(uint8_t init_size, uint8_t x, uint8_t y);
void snek_shift();
void snek_change_dir(uint8_t dir);
void snek_gen_food();