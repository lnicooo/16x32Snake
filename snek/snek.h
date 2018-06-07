#include <stdint.h>

#define MAX_SIZE 15

struct snek_pix{
    int8_t pix_x;
    int8_t pix_y;
};

enum{
    RIGTH,
    LEFT,
    UP,
    DOWN
    
};

extern struct snek_pix snek [MAX_SIZE];
extern volatile uint8_t snek_size;
extern volatile uint8_t actual_dir;

void snek_init(uint8_t init_size, uint8_t x, uint8_t y);
void snek_shift();
void snek_change_dir(uint8_t dir);
void snek_eat();