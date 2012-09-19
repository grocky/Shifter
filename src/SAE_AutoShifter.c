/**
 *  @file SAE_AutoShifter.c
 *  @brief This file defines the main task and ISRs
 * 
 *  @author  Rocky Gray Jr.
 *
 *  @date    2/6/2012
 */
#include "SAE_AutoShifter.h"

struct shifter
{
    Tach     *tach;
    Gear     *cur_gear;
    Usr_Btns *up_shift,  ///< Upshift button
             *dn_shift;  ///< Downshift button
}shiftr_singleton;

Usr_Btns ushift, dshift;

Shifter *shifter_singleton()
{
    return &shiftr_singleton;
}

void shifter_init(Shifter *shifter, Gear *g)
{
    shifter->tach = tachometer_singleton();
    shifter->cur_gear = g;
    shifter->up_shift = &ushift;
    shifter->dn_shift = &dshift;
    btn_init(shifter->up_shift);
    btn_init(shifter->dn_shift);
}

void t_set_rpms(Shifter *shifter, uint16_t x)
{
    shifter->tach->rpms = x;
}

void t_set_ave(Shifter *shifter, uint16_t x)
{
    shifter->tach->ave = x;
}

uint16_t g_upper(Shifter *shifter)
{
    return gear_upper(shifter->cur_gear);
}

uint16_t g_lower(Shifter *shifter)
{
    return gear_lower(shifter->cur_gear);
}

Gear *g_prev(Shifter *shifter)
{
    return shifter->cur_gear->prev;
}

Gear *g_next(Shifter *shifter)
{
    return shifter->cur_gear->next;
}

Usr_Btn *down_btn(Shifter *shifter)
{
    return shifter->dn_shift;
}

Usr_Btn *up_btn(Shifter *shifter)
{
    return shifter->up_shift;
}
//Move gear shift in the desired direction
void shift(uint8_t direction, Shifter *s)
{
    ECU_PORT |= _BV(IGNITION_INT);
    delay_ms(IGNITION_DLY);
    SOLEN_OP_PORT |= _BV(direction);
    delay_ms(SOLEN_DLY);
    SOLEN_OP_PORT &= ~_BV(direction);
    delay_ms(IGNITION_DLY);
    ECU_PORT &= ~_BV(IGNITION_INT);

#ifdef SIMULATE
    cli();
    switch(direction)
    {
        case SOLEN_UP:
            switch(s->cur_gear->g_num)
            {
                case 1:
                    tach.rpms -= 500;
                    break;
                case 2:
                    tach.rpms -= 400;
                    break;
                case 3:
                    tach.rpms -= 300;
                    break;
                case 4:
                    tach.rpms -= 200;
                    break;
                default:
                    //Shifting up in 5th gear not allowed...
                    break;
            }
        case SOLEN_DN:
            switch(s->cur_gear->g_num)
            {
                case 1:
                case 2:
                    tach.rpms += 400;
                    break;
                case 3:
                    tach.rpms += 300;
                    break;
                case 4:
                    tach.rpms += 200;
                    break;
                case 5:
                    tach.rpms += 100;
                    break;
                default:
                    break;
            }
    }
    sei();
#endif  /* SIMULATE */
    
    Gear *temp=0;
    if(direction == SOLEN_UP)
        temp = g_next(s);
    else if(direction == SOLEN_DN)
        temp = g_prev(s);

    s->cur_gear = temp;

}
