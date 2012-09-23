/**
 *  @file
 *  @brief This file contains device specific initialization tasks and the
 *  main function.
 * 
 *  @see defines.h
 *  @author  Rocky Gray Jr.
 *
 *  @date    3/11/2012
 */
#include <stdio.h>
#include "SAE_AutoShifter.h"
#include "serial.h"

//#define F_CPU 16000000L
///Uncomment #DEBUG to print out system information.
#define DEBUG 1
///Uncomment #SIMULATE in order to use the rpm regulator simulation
#define SIMULATE 1

uint8_t canPrint;   ///<DEBUG variable - print flag
/**
 * @brief Initialize i/o ports and timer.
 *
 * @section init2 Outputs:
 *  - Solenoid Output
 *      - #SOLEN_OP_DDR   => set #SOLEN_DN and #SOLEN_UP as output 
 *      - #SOLEN_OP_PORT  => initialize #SOLEN_DN and #SOLEN_UP to 0
 *  - Board Light Output
 *      - #BOARD_DDR      => set #BOARD_LIGHT as output
 *      - #BOARD_PORT     => initialize BOARD_LIGHT to 0  
 * @section init1 Inputs:
 *  - User Buttons
 *      - #BTN_IP_DDR     => set #USHIFT_PIN and #DSHIFT_PIN as input
 *      - #BTN_IP_PORT    => set pullups for #USHIFT_PIN and #DSHIFT_PIN
 *  - Tachometer Input
 *      - #TACH_IP_DDR    => set #TACH_PIN as input
 *      - Enable external Interrupt 
 *  - Gas Pedal Input
 *      - Enable ADC
 *          - 128 Prescaler
 *          - AREF = AVCC
 *          - Left align result
 *          - Free Running mode */
inline void io_init(void)
{
    //Setup outputs
    //Solenoid output
    SOLEN_OP_DDR |= _BV(SOLEN_DN)|_BV(SOLEN_UP);
    SOLEN_OP_PORT &= ~(_BV(SOLEN_DN)|_BV(SOLEN_UP));

    //Setup board light
    BOARD_DDR |= _BV(BOARD_LIGHT);
    BOARD_PORT &= ~_BV(BOARD_LIGHT);

    init_usart(Baud9600);    
    
    //Setup inputs
    //User buttons
    BTN_IP_DDR  &= ~(_BV(USHIFT_PIN)|_BV(DSHIFT_PIN));
    BTN_IP_PORT |= _BV(USHIFT_PIN)|_BV(DSHIFT_PIN);
    BTN_IP_DDR  &= ~(_BV(AUTOMATIC_PIN)|_BV(SEMIAUTO_PIN));
    BTN_IP_PORT |= _BV(AUTOMATIC_PIN)|_BV(SEMIAUTO_PIN);
    
    //Ignition Interrupt
    ECU_DDR |= _BV(IGNITION_INT);
    ECU_PORT &= ~_BV(IGNITION_INT);
    //#else
    //Tachometer input
    ECU_DDR &= ~_BV(TACH_PIN);
    EICRA |= _BV(ISC01)|_BV(ISC00); //Interrupt0 Rising Edge
    EIMSK |= _BV(INT0);             //enable Interrupt0
//#endif
   
//#ifdef SIMULATE
    //Gas Pedal input (ADC)
    ADCSRA |= _BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0); //select 128 prescaler
    ADCSRA |= _BV(ADATE);
    ADMUX |= _BV(REFS0);    //Set ADC reference to AVCC
    ADMUX |= _BV(ADLAR);    //Left align result
    ADMUX &= ~(_BV(MUX3)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0));  //Select ADC0
    ADCSRB &= ~(_BV(ADTS2)|_BV(ADTS1)|_BV(ADTS0)); //Select Free Running
    ADCSRA |= _BV(ADEN);     //Enable ADC
    ADCSRA |= _BV(ADIE);     //Enable ADC Interrupt
    sei();
}

/**
 * @brief Initialize Timer 0.
 *
 * Timer 0:
 *  - Configure Timer 0 for CTC (Clear Timer on Compare)
 *  - Prescaler set to #PRESCALER0 (16MHz/#PRESCALER0)
 *  - Fires every #TIMER0_FREQ Hz                   */
inline void timer0_init(void)
{
    TCCR0A |= _BV(WGM01);   // Configure Timer 0 for CTC mode
    TIMSK0 |= _BV(OCIE0A);  // Enable Output Compare interrupt

    // OCR0A = 16M/64/1k-1 = 249 < 256
    OCR0A = F_CPU/PRESCALER0/TIMER0_FREQ-1;

    TCCR0B |= _BV(CS01)|_BV(CS00);    // Start Timer0 at F_CPU/64
}

/**
 * @brief Initialize Timer 1.
 *
 * Timer 1:
 *  - Configure Timer 1 for...
 *  - Prescaler set to #PRESCALER1 (16MHz/#PRESCALER1)
 *  - Fires every #TIMER1_FREQ Hz               */
inline void timer1_init(void)
{
    TCCR1B |= _BV(WGM12);   // Configure Timer 0 for CTC mode
    TIMSK1 |= _BV(OCIE1A);  // Enable Output compare interrupt

    // 0CR1A = 16M/256/1-1 = 62,499 < 65,536
    OCR1A |= F_CPU/PRESCALER1/TIMER1_FREQ-1;

    TCCR1B |= _BV(CS12);
}

/** 
 *  @brief   The main task for the autoshifter      */
int main(void)
{   
    timer0_init();
    timer1_init();
    io_init();
   
    //Create and initialize gears
    Gear g1,g2,g3,g4,g5;
    //Gear_Const(Gear *this, uint8_t num, uint8_t lower, uint8_t upper,
    //           Gear *p, Gear* n)
    
    Gear_Construct(&g1,1,0,100,50,0,&g2);
    uint16_t inc1[MAX_GEARS] = {20,30,40,100,200};
    uint16_t dec1[MAX_GEARS] = {200,100,30,20,10};
    Gear_IncreaseLevels(&g1,inc1);
    Gear_DecreaseLevels(&g1,dec1);

    Gear_Construct(&g2,2,20,200,150,&g1,&g3);
    uint16_t inc2[MAX_GEARS] = {20,40,60,90,160};
    uint16_t dec2[MAX_GEARS] = {400,90,40,20,10};
    Gear_IncreaseLevels(&g2,inc2);
    Gear_DecreaseLevels(&g2,dec2);
   
    Gear_Construct(&g3,3,120,300,250,&g2,&g4);
    uint16_t inc3[MAX_GEARS] = {40,60,80,100,120};
    uint16_t dec3[MAX_GEARS] = {140,80,50,30,10};
    Gear_IncreaseLevels(&g3,inc3);
    Gear_DecreaseLevels(&g3,dec3);
    
    Gear_Construct(&g4,4,200,400,350,&g3,&g5);
    uint16_t inc4[MAX_GEARS] = {20,40,60,80,100};
    uint16_t dec4[MAX_GEARS] = {100,60,40,20,10};
    Gear_IncreaseLevels(&g4,inc4);
    Gear_DecreaseLevels(&g4,dec4);
    
    Gear_Construct(&g5,5,300,500,450,&g4,0);
    uint16_t inc5[MAX_GEARS] = {20,30,40,50,60};
    uint16_t dec5[MAX_GEARS] = {50,40,30,20,10};
    Gear_IncreaseLevels(&g5,inc5);
    Gear_DecreaseLevels(&g5,dec5);
    
    gear_ = &g1;

#ifdef DEBUG
    puts("DEBUG is on.\r\n");
#endif

#ifdef SIMULATE
    puts("SIMULATE is on.\r\n");
#endif
    printf("\rStarting main task...\r\r");
    delay_ms(200);
    ADCSRA |= _BV(ADSC);     //Start ADC Conversion
    //sei();

    //Main task
    for(;;)
    {
        switch(mode)
        {
            case manual:         //Manual
                //If up_shift is pressed and released...
                if(onRelease(&up_shift))
                {
                    if(gear_->next != 0)
                    {
                        shift(SOLEN_UP);
                        gear_ = gear_->next;
                    }
                }

                //If dn_shift is pressed and released...
                if(onRelease(&dn_shift))
                {
                    if(gear_->prev != 0 && tach.rpms < gear_upper())
                    {
                        shift(SOLEN_DN);
                        gear_ = gear_->prev;
                    }
                }
                break;
            case semi_man:
                // Upshift 
                if(tach.rpms >= gear_upper())
                {
                    if(gear_->next != 0)
                    {
                        shift(SOLEN_UP);
                        gear_ = gear_->next;
                    }
                }
                // Downshift
                if(onRelease(&dn_shift))
                {
                    if(gear_->prev != 0 && tach.ave < gear_upper())
                    {
                        shift(SOLEN_DN);
                        gear_ = gear_->prev;
                    }
                }
                break;
            case automated:
                // Upshift
                if(tach.rpms >= gear_upper())
                {
                    if(gear_->next != 0)
                    {
                        shift(SOLEN_UP);
                        gear_ = gear_->next;
                    }
                }
                // Downshift
                else if(tach.rpms < gear_lower())
                {
                    if(gear_->prev != 0)
                    {
                        shift(SOLEN_DN);
                        gear_ = gear_->prev;
                    }
                }
                break;
            default:
                //Should not get here...
                break;
        }
        
        update_ave_rpms();

//#ifdef DEBUG
        if(canPrint)
        {
            //printf("cur pos = %d\n\r",throttle_pos);
            //printf("current ADC = %d\n\r",cur_adc);
            printf("Current mode = %d\n\r", mode);
            printf("Current gear = %d\n\r", gear_num());
            printf("Current rpms = %d\n\r",cur_rpms());
            //printf("Ave rpms = %d\r\r",average_rpms());

            canPrint = 0;
        }
//#endif
       //Give'em a break.
        delay_ms(1);
    }
    // Should never get here
    return 0;
}

/** 
 *  @brief  Update the button states.
 *
 *  Fires at #TIMER0_FREQ Hz.
 */
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

/**
 * @brief Converts position of external pedal to the change of rpms.
 *
 * This function converts the position of an external electric pedal using
 * the ADC. It uses the position of the pedal to describe the derrivative of
 * the engine's rpms over time.
 */
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

/** 
 *  @brief  Tachometer sample time. 
 *
 *  Fires at #TIMER1_FREQ Hz. (1ms) 
 *  @par Converts the number of pulses counted
 *  from the external interrupt into a value that represents the 
 *  instantaneous rpms of the engine. It also places the current rpms into 
 *  the history of rpms.
 */
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

/**
 *  @brief  Count pulses from the tachometer.
 *  Fires at the rising edge of signal
 *  @par Counts the pulses from the tachometer until TIMER1 fires.
 */
ISR(INT0_vect)
{/*
//    BOARD_PIN |= _BV(BOARD_LIGHT);
    ++tach.pulse;
    */
}
