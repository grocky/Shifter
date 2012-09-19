/**
 *  @file
 *  @brief This file defines the main task and ISRs
 * 
 *  @author  Rocky Gray Jr.
 *
 *  @date    2/6/2012
 */
#include "SAE_AutoShifter.h"

// Update Button States (1ms)
ISR(TIMER0_COMPA_vect)
{
    // Upshift button
    if(!(BTN_IP_PIN & _BV(USHIFT_PIN)))
    {
        if(++up_shift.count >= DB_DELAY && up_shift.state == RELEASED)
            up_shift.state = PRESSED;
    }else
    {
        up_shift.count = 0;
        up_shift.state = RELEASED;
    }
    
    // Downshift button
    if(!(BTN_IP_PIN & _BV(DSHIFT_PIN)))
    {
        if(++dn_shift.count >= DB_DELAY && dn_shift.state == RELEASED)
            dn_shift.state = PRESSED;
    }else
    {
        dn_shift.count = 0;
        dn_shift.state = RELEASED;
    }

    // Mode button
    if(!(BTN_IP_PIN & _BV(SEMIAUTO_PIN)))
        mode = semi_man;
    else if(!(BTN_IP_PIN & _BV(AUTOMATIC_PIN)))
        mode = automated;
    else
        mode = manual;
}

ISR(ADC_vect)
{
    uint8_t adc = ADCH;
    cur_adc = ADCH;
    if(adc <= 50)
        throttle_pos = 0;
    else if(adc <= 80)
        throttle_pos = 1;
    else if(adc <= 114)
        throttle_pos = 2;
    else if(adc <= 149)
        throttle_pos = 3;
    else if(adc <= 184)
        throttle_pos = 4;
    else
        throttle_pos = 5;
}

// Tachometer sample time (1s)
ISR(TIMER1_COMPA_vect)
{
//#ifdef DEBUG
    static uint8_t tick=0;///<DEBUG variable - system tick
    if(++tick >= 2)
    {
        canPrint = 1;
        tick = 0;
    }
  static uint8_t prev_pos = 0;
//#endif
/*
  switch(throttle_pos)
  {
      case 4:
        tach.rpms += 30;
      case 3:
        tach.rpms += 20;
      case 2:
        tach.rpms += 10;
      case 1:
        tach.rpms += 5;
      case 0:
        if(tach.rpms <=30)
            tach.rpms = 0;
        else
            tach.rpms -= 30;
  }
*/

    switch(throttle_pos)
    {
        case 4:
            if(throttle_pos < prev_pos)
                if(tach.rpms > gear_->decrease[throttle_pos])
                    tach.rpms -= gear_->decrease[throttle_pos];
            else
            //if(tach.rpms <= gear_upper())
                    tach.rpms += gear_->increase[throttle_pos];
            break;
        case 3:
            if(throttle_pos==0 || throttle_pos < prev_pos)
            {
                if(tach.rpms > gear_->decrease[throttle_pos])
                    tach.rpms -= gear_->decrease[throttle_pos];
            }else
            {
                //if(tach.rpms <= gear_upper())
                    tach.rpms += gear_->increase[throttle_pos];
            }
            break;
        case 2:
            if(throttle_pos==0 || throttle_pos < prev_pos)
            {
                if(tach.rpms > gear_->decrease[throttle_pos])
                    tach.rpms -= gear_->decrease[throttle_pos];
            }else
            {
                //if(tach.rpms <= gear_upper())
                    tach.rpms += gear_->increase[throttle_pos];
            }   
            break;
        case 1:
            if(throttle_pos==0 || throttle_pos < prev_pos)
            {
                if(tach.rpms > gear_->decrease[throttle_pos])
                    tach.rpms -= gear_->decrease[throttle_pos];
            }else
            {
                //if(tach.rpms <= gear_upper())
                    tach.rpms += gear_->increase[throttle_pos];
            }
            break;
        case 0:
                if(tach.rpms > gear_->decrease[throttle_pos])
                    tach.rpms -= gear_->decrease[throttle_pos];
            break;
        default:
            break;
    }
    prev_pos = throttle_pos;
  /*
//#endif
    //rpms = pulses/[pulses/rotation]*[sample freq]*[60s/min]
    tach.rpms = tach.pulse*60*TIMER1_FREQ/PULSE_ROT; //rot/min
    tach.rpms_hist[tach.index] = tach.rpms;
    
    if(++tach.index == 10)
        tach.index = 0;
   
    //reset pulse so we can start count over
    tach.pulse = 0;
    */
}
// Pulse counter
ISR(INT0_vect)
{/*
//    BOARD_PIN |= _BV(BOARD_LIGHT);
    ++tach.pulse;
    */
}
