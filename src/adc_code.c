//#ifdef SIMULATE
ISR(ADC_vect)
{
    static uint8_t prev_pos = 0;
    uint8_t cur_pos=0;
    uint8_t adc = ADCH;
    cur_adc = ADCH;
    if(adc > 184)      // 4 < Vin < 5
    {
        cur_pos = 4;
        
    }else if(adc > 149)// 3 < Vin < 4
    {
        cur_pos = 3;
       
    }else if(adc > 114)// 2 < Vin < 3
    {
        cur_pos = 2;
      
    }else if(adc >80) // 1 < Vin < 2
    {
        cur_pos = 1;
        
    }else if(adc > 42) // 0 < Vin < 1
    {
        cur_pos = 0;
        
    }

    prev_pos = cur_pos;
    tach.rpms_hist[tach.index] = tach.rpms;

    if(++tach.index == 10)
        tach.index = 0;
}
//#endif /* SIMULATE */


