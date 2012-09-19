/**
 *  @file tachometer.c
 *  @brief This file defines the tachometer object and related functions.
 *
 *  @author  Rocky Gray Jr.
 *
 *  @date    4/21/2012
 */
#include "tachometer.h"

struct tachometer
{
    uint16_t pulse;
    uint16_t rpms;
    uint8_t  index;
    uint16_t rpms_hist[RPM_HIST_LEN];
    uint16_t ave;
} tach_singleton;

//Return the tach singleton
Tach * tachometer_singleton()
{
    return &tach_singleton;
}
//Update the rpm average
void update_ave_rpms()
{
    uint32_t sum = 0;

    for(int i = 0; i < RPM_HIST_LEN; ++i)
        sum += tach_singleton.rpms_hist[i];

    tach_signelton.ave = sum/RPM_HIST_LEN;
}

void set_rpms(uint16_t x)
{
    tach_singleton.rpms = x;
}

void set_ave_rpms(uint16_t x)
{
    tach_singleton.ave = x
}
//Return current rpms
uint16_t get_rpms()
{
    return tach_singleton.rpms;
}

//Return ave rpms
uint16_t get_ave_rpms()
{
    return tach_singleton.ave;
}

ISR(INT0_vect)
{
//    BOARD_PIN |= _BV(BOARD_LIGHT);
    ++tach.pulse;
}
