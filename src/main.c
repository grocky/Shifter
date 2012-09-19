/**
 *  @file main.c
 *  @brief This file contains device specific initialization tasks and the
 *  main function.
 *
 *  If you want to test the system using a 5V analog signal applied to 
 *  ADC0, uncomment \#define SIMULATE in the defines.h file.
 * 
 *  @see defines.h
 *  @author  Rocky Gray Jr.
 *
 *  @date    3/11/2012
 */
#include <stdio.h>
#include "SAE_AutoShifter.h"
#include "serial.h"

///Mode of the system
enum Mode {manual, semi_man, automated}mode;///< Mode switch

enum{ushift=0,dshift,bpress,sampleTach,canPrint};

uint8_t status_reg = 0x00;
Shifter *s;

inline void io_init(void);
inline void timer0_init(void);
inline void timer1_init(void);

void update_tach(Shifter *s);
/** 
 *  @brief   The main task for the autoshifter
 *
 *  This task initializes the timers and io ports. 
 *  It instantiates the @ref gears objects.
 *  It controls the mode selection.
 *  It responds to button presses from the user.
 *  It activates the shifting process
 */
int main(void)
{   

#ifdef DEBUG
    puts("DEBUG is on.\r\n");
#endif

    io_init();
    timer0_init();
    timer1_init();
  
    s = shifter_singleton();
    
    //Instantiate gears
    Gear g1,g2,g3,g4,g5;
    //Gear_Const(Gear *this, uint8_t num, uint8_t lower, uint8_t upper,
    //           Gear *p, Gear* n)
    Gear_Construct(&g1,1,0,1000,500,0,&g2);
    Gear_Construct(&g2,2,400,2000,1500,&g1,&g3);
    Gear_Construct(&g3,3,1500,3000,2500,&g2,&g4);
    Gear_Construct(&g4,4,2600,4000,3500,&g3,&g5);
    Gear_Construct(&g5,5,3700,5000,4500,&g4,0);

#ifdef SIMULATE
#ifdef DEBUG
    puts("SIMULATE is on.\r\n");
#endif
    /// @note The arrays for the increase and decrease deltas could be 
    ///  changed to a multi-dimensional array. Using a m-d array would 
    ///  require the Gear_IncreaseLevels() and Gear_DecreaseLevels() 
    ///  functions to be changed as well.
    uint16_t inc1[MAX_GEARS+1] = {0,20,30,40,100,200};
    uint16_t dec1[MAX_GEARS+1] = {200,100,30,20,10,1};
    Gear_IncreaseLevels(&g1,inc1);
    Gear_DecreaseLevels(&g1,dec1);

    uint16_t inc2[MAX_GEARS+1] = {0,20,40,60,90,160};
    uint16_t dec2[MAX_GEARS+1] = {400,90,40,20,10,1};
    Gear_IncreaseLevels(&g2,inc2);
    Gear_DecreaseLevels(&g2,dec2);
   
    uint16_t inc3[MAX_GEARS+1] = {0,40,60,80,100,120};
    uint16_t dec3[MAX_GEARS+1] = {140,80,50,30,10,1};
    Gear_IncreaseLevels(&g3,inc3);
    Gear_DecreaseLevels(&g3,dec3);
    
    uint16_t inc4[MAX_GEARS+1] = {0,20,40,60,80,100};
    uint16_t dec4[MAX_GEARS+1] = {100,60,40,20,10,1};
    Gear_IncreaseLevels(&g4,inc4);
    Gear_DecreaseLevels(&g4,dec4);
    
    uint16_t inc5[MAX_GEARS+1] = {0,20,30,40,50,60};
    uint16_t dec5[MAX_GEARS+1] = {50,40,30,20,10,1};
    Gear_IncreaseLevels(&g5,inc5);
    Gear_DecreaseLevels(&g5,dec5);
#endif /* SIMULATE */

    shifter_init(s, &g1);

#ifdef DEBUG
    printf("\rStarting main task...\r\r");
#endif

    delay_ms(200);
    sei();

    //Main task
    for(;;)
    {
        switch(mode)
        {
            case semi_man:
                // Upshift 
                if(get_rpms() >= g_upper(s))
                {
                    if(g_next(s) != 0)
                        shift(SOLEN_UP,s);
                }
                // Downshift
                if(onRelease(down_btn(s)))
                {
                    if(g_prev(s) != 0 && get_rpms() < g_upper(s))
                        shift(SOLEN_DN,s);
                }
                break;
            case automated:
                // Upshift
                if(get_rpms() >= g_upper(s))
                {
                    if(g_next(s) != 0)
                        shift(SOLEN_UP,s);
                }
                // Downshift
                else if(get_rpms() < g_lower(s))
                {
                    if(g_prev(s) != 0)
                        shift(SOLEN_DN,s);
                }
                break;
            default:
                case manual:         //Manual
                //If up_shift is pressed...
                if(status_reg & _BV(ushift))
                {
                    //if a btn is not already pressed AND not in top gear
                    if(!(status_reg & _BV(bpress)) && g_next(s) != 0)
                    {
                        status_reg |= _BV(bpress);
                        shift(SOLEN_UP,s);
                    }
                }
                //If dn_shift is pressed...
                else if(status_reg & _BV(dshift))
                {
                    //btn not already pressed...
                    if(!(status_reg & _BV(bpress)))
                    {
                        if(g_prev(s) != 0 && get_rpms() < g_upper(s))
                        {
                            status_reg |= _BV(bpress);
                            shift(SOLEN_DN,s);
                        }
                    }
                }
                else
                    status_reg &= ~_BV(bpress);
                break; 
        }
        update_ave_rpms();

        if(status_reg & _BV(sampleTach))
            update_tach(s);
#ifdef DEBUG
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
#endif
       //Give'em a break.
        delay_ms(1);
    }
    // Should never get here
    return 0;
}

void update_tach(Shifter *s)
{
#ifndef SIMULATE
   //rpms = pulses/[pulses/rotation]*[sample freq]*[60s/min]
    uint16_t rpms = /*tach.pulse**/60*TIMER1_FREQ/PULSE_ROT; //rot/min
    t_set_rpms(s,rpms);
  
    //reset pulse so we can start count over
    //tach.pulse = 0; 
/*
#else
    static uint8_t prev_pos = 0;
    //if we want to slow down...
    if(throttle_pos < prev_pos || throttle_pos == 0)
    {
        //make sure we can decrease rpms without going negative
        if(tach.rpms >= gear_->decrease[throttle_pos])
            tach.rpms -= gear_->decrease[throttle_pos];
        else
            tach.rpms = 0;
    }
    else if(throttle_pos == prev_pos) //if we want to cruise
    {
        //let the rpms converge to the cruise level of the gear
        if(tach.rpms < gear_->cruise)
            tach.rpms += gear_->increase[throttle_pos];
        else if(tach.rpms > gear_->cruise)
            tach.rpms -= gear_->decrease[throttle_pos];
    }
    else //throttle_pos > prev_pos
    {
        //increase rpms
        tach.rpms += gear_->increase[throttle_pos];
        if(tach.rpms >= RPM_MAX)
            tach.rpms = RPM_MAX;
    }
    
    prev_pos = throttle_pos;
    
#endif
    tach.rpms_hist[tach.index] = tach.rpms;
    
    if(++tach.index == 10)
        tach.index = 0;
        */
#endif
}
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
 *      - #ECU_DDR    => set #TACH_PIN as input
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

#ifndef SIMULATE
    //Tachometer input
    ECU_DDR &= ~_BV(TACH_PIN);
    EICRA |= _BV(ISC01)|_BV(ISC00); //Interrupt0 Rising Edge
    EIMSK |= _BV(INT0);             //enable Interrupt0
#else   
    //Gas Pedal input (ADC)
    ADCSRA |= _BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0); //select 128 prescaler
    ADCSRA |= _BV(ADATE);   //ADC auto trigger enabled
    ADMUX |= _BV(REFS0);    //Set ADC reference to AVCC
    ADMUX |= _BV(ADLAR);    //Left align result
    ADMUX &= ~(_BV(MUX3)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0));  //Select ADC0
    ADCSRB &= ~(_BV(ADTS2)|_BV(ADTS1)|_BV(ADTS0)); //Select Free Running
    ADCSRA |= _BV(ADEN);     //Enable ADC
    ADCSRA |= _BV(ADIE);     //Enable ADC Interrupt
    ADCSRA |= _BV(ADSC);     //Start ADC Conversion
#endif
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

// Update Button States (1ms)
ISR(TIMER0_COMPA_vect)
{
    static uint8_t ushift_count = 0;
    static uint8_t dshift_count = 0;

    // Upshift button
    if(!(BTN_IP_PIN & _BV(USHIFT_PIN)))
    {
        if(++ushift_count >= DB_DELAY && !(status_reg & _BV(ushift)))
            status_reg |= _BV(ushift);
    }else
    {
        ushift_count = 0;
        status_reg &= ~_BV(ushift);
    }
    
    // Downshift button
    if(!(BTN_IP_PIN & _BV(DSHIFT_PIN)))
    {
        if(++dshift_count >= DB_DELAY && !(status_reg & _BV(dshift)))
            status_reg |= _BV(dshift);
    }else
    {
        dshift_count = 0;
        status_reg &= ~_BV(dshift);
    }

    // Mode button
    if(!(BTN_IP_PIN & _BV(SEMIAUTO_PIN)))
        mode = semi_man;
    else if(!(BTN_IP_PIN & _BV(AUTOMATIC_PIN)))
        mode = automated;
    else
        mode = manual;
}

// Tachometer sample time (1s)
ISR(TIMER1_COMPA_vect)
{
#ifdef DEBUG
    static uint8_t tick=0;///<DEBUG variable - system tick
    if(++tick >= 2)
    {
        canPrint = 1;
        tick = 0;
    }
#endif

}

#ifndef SIMULATE
// Pulse counter

#else
//Update the position of the pedal
ISR(ADC_vect)
{
    //8-bit var because ADC is left aligned
    uint8_t adc = ADCH;
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
#endif
