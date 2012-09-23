/**
 *  @file
 *  @brief Shifter defines the functions and logic for the AutoShifter
 * 
 *  @author  Rocky Gray Jr.
 *
 *  @date    2/6/2012
 */
#include "SAE_AutoShifter.h"

void update_ave_rpms()
{
    uint32_t sum = 0;

    for(int i = 0; i < RPM_HIST_LEN; ++i)
        sum += tach.rpms_hist[i];

    tach.ave = sum/RPM_HIST_LEN;
}

uint16_t average_rpms()
{
    return tach.ave;
}

uint16_t cur_rpms()
{
    return tach.rpms;
}

uint8_t btn_state(Usr_Btns *u_btn)
{
    return u_btn->state;
}

uint16_t btn_count(Usr_Btns *u_btn)
{
    return u_btn->count;
}

uint8_t onRelease(Usr_Btns *btn)
{
    if(btn->state == RELEASED)
        return 0;
    if(btn->state == PRESSED)
        while(btn->state == PRESSED)
            delay_us(1);
    
    return 1;
}

void Gear_Construct(Gear *this, uint8_t num, uint16_t lower,
                        uint16_t upper, uint16_t cruise, Gear *p,Gear *n)
{
    this->g_num = num;
    this->lowerB = lower;
    this->upperB = upper;
    this->cruise = cruise;
    this->prev = p;
    this->next = n;
}

void Gear_IncreaseLevels(Gear *this, uint16_t levels[])
{
    for(int i=0; i<MAX_GEARS;++i)
        this->increase[i] = levels[i];
}

void Gear_DecreaseLevels(Gear *this, uint16_t levels[])
{
    for(int i=1; i<MAX_GEARS;++i)
        this->decrease[i] = levels[i];
}

uint8_t gear_num()
{
    return gear_->g_num;
}

uint16_t gear_upper()
{
    return gear_->upperB;
}

uint16_t gear_lower()
{
    return gear_->lowerB;
}

void shift(uint8_t direction)
{
    ECU_PORT |= _BV(IGNITION_INT);
    delay_ms(IGNITION_DLY);
    SOLEN_OP_PORT |= _BV(direction);
    delay_ms(SOLEN_DLY);
    SOLEN_OP_PORT &= ~_BV(direction);
    delay_ms(IGNITION_DLY);
    ECU_PORT &= ~_BV(IGNITION_INT);

    cli();
//#ifdef SIMULATE
    int next_rpms;
    switch(direction)
    {
        case SOLEN_UP:
            switch(gear_->g_num)
            {
                case 1:
                    tach.rpms -= 60;
                    break;
                case 2:
                    tach.rpms -= 60;
                    break;
                case 3:
                    tach.rpms -= 60;
                    break;
                case 4:
                    tach.rpms -= 60;
                    break;
                default:
                    //Shifting up in 5th gear not allowed...
                    break;
            }
        case SOLEN_DN:
            switch(gear_->g_num)
            {
                case 1:
                case 2:
                    tach.rpms += 60;
                    break;
                case 3:
                    tach.rpms += 60;
                    break;
                case 4:
                    tach.rpms += 60;
                    break;
                case 5:
                    tach.rpms += 60;
                    break;
                default:
                    break;
            }
    }
    sei();
//#endif  /* SIMULATE */
}
