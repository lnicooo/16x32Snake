#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
//#include "../TWI/TWI_Master.h"
#include "../i2c/i2cmaster.h"

#include "mma8452.h"

//v0.01 only read orientation
uint8_t twi_buffer[4];

void mma_init(){   

    //TWI_Master_Initialise();
    //sei();
    i2c_init();
    //standby on CTRL_REG1
    mma_standby();
    //set scale XYZ_DATA_CFG
    mma_scale();
    //set outputdata rate CTRL_REG1
    mma_outrate();
    //set PORTRAIT/LANDSCAPE PL_CFG
    mma_setpl();
    //activate on CTRL_REG1
    mma_activate();
}

uint8_t mma_get_PL(){

    mma_read_register(PL_STATUS);
    
    if(twi_buffer[1] & 0x40)

        return LOCKOUT;
    
    else

        return ((twi_buffer[1] & 0x6) >> 1);
    
}

void mma_standby(){
    mma_read_register(CTRL_REG1);

    mma_write_register(CTRL_REG1, twi_buffer[1] & ~(0x01));
}

void mma_activate(){
    mma_read_register(CTRL_REG1);
    
    mma_write_register(CTRL_REG1,  twi_buffer[1] | 0x01);
}

void mma_outrate(){
    
    mma_read_register(CTRL_REG1);

    char ctrl = twi_buffer[1];
    ctrl &= 0xC7;
    ctrl |= (0 << 3); //800Hz
    
    mma_write_register(CTRL_REG1, ctrl);
}

uint8_t mma_available(){
    mma_read_register(STATUS);
    return((twi_buffer[1] & 0x08)>>3);
}

void mma_scale(){
    
    mma_read_register(XYZ_DATA_CFG);
    char cfg = twi_buffer[1];
    
    cfg &= 0xFC;
    cfg |= 0; //2g 
    
    mma_write_register(XYZ_DATA_CFG, 0);
}

void mma_setpl(){

    mma_write_register(PL_CFG, 0x40);
    mma_write_register(PL_COUNT, 0x30);
}

void mma_read_register(uint8_t reg){

    

    i2c_start_wait(SLA_W);
    i2c_write(reg);
    i2c_rep_start(SLA_R);
    twi_buffer[1] = i2c_readNak();
    i2c_stop();

    /*
    twi_buffer[0] = SLA_W;
    twi_buffer[1] = reg;
    if ( !TWI_Transceiver_Busy() ) {

        TWI_Start_Transceiver_With_Data(twi_buffer, 2); 
    
        twi_buffer[0] = SLA_R;
    
        TWI_Start_Transceiver_With_Data(twi_buffer, 2); 
        
       if(TWI_Get_Data_From_Transceiver(twi_buffer, 2)){
           printf("Read\n");
       } 

    }

    int i;
    for(i=0;i<4;i++){
        printf("%x |",twi_buffer[i]);
    }
    printf("\n");
    */

}

void mma_write_register(uint8_t reg, uint8_t data){
    /*
    twi_buffer[0] = SLA_W;
    twi_buffer[1] = reg;
    twi_buffer[2] = data;
    if ( !TWI_Transceiver_Busy() ) {
        TWI_Start_Transceiver_With_Data(twi_buffer, 3); 
    }
    */
   i2c_start_wait(SLA_W);
   i2c_write(reg);
   i2c_write(data);
   i2c_stop();
}