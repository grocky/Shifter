/**
 *  @file usr_button.c
 *  @brief This file defines the usr_button object and related functions.
 *
 *  @author  Rocky Gray Jr.
 *
 *  @date    4/21/2012
 */
#include "usr_button.h"

void btn_init(Usr_Btns *u_btn)
{
    u_btn->state = 0;
}
//return btn state
uint8_t btn_state(Usr_Btns *u_btn)
{
    return u_btn->state;
}

//Wait for button release
uint8_t onRelease(Usr_Btns *btn)
{
    if(btn->state == RELEASED)
        return 0;
    if(btn->state == PRESSED)
        while(btn->state == PRESSED)
            delay_us(1);
    
    return 1;
}
