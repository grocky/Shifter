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
        if(cur_pos > prev_pos)
        {
            if(tach.rpms <= gear_upper())
                tach.rpms += gear_->increase[cur_pos];
        }else
        {
            if(tach.rpms > gear_->decrease[cur_pos])
                tach.rpms -= gear_->decrease[cur_pos];
        }
    }else if(adc > 149)// 3 < Vin < 4
    {
        cur_pos = 3;
        if(cur_pos > prev_pos)
        {
            if(tach.rpms <= gear_upper())
                tach.rpms += gear_->increase[cur_pos];
        }else
        {
            if(tach.rpms > gear_->decrease[cur_pos])
                tach.rpms -= gear_->decrease[cur_pos];
        }
    }else if(adc > 114)// 2 < Vin < 3
    {
        cur_pos = 2;
        if(cur_pos > prev_pos)
        {
            if(tach.rpms <= gear_upper())
                tach.rpms += gear_->increase[cur_pos];
        }else
        {
            if(tach.rpms > gear_->decrease[cur_pos])
                tach.rpms -= gear_->decrease[cur_pos];
        }
    }else if(adc >80) // 1 < Vin < 2
    {
        cur_pos = 1;
        if(cur_pos > prev_pos)
        {
            if(tach.rpms <= gear_upper())
                tach.rpms += gear_->increase[cur_pos];
        }else
        
            if(tach.rpms > gear_->decrease[cur_pos])
                tach.rpms -= gear_->decrease[cur_pos];
        }
    }else if(adc > 42) // 0 < Vin < 1
    {
        cur_pos = 0;
        if(cur_pos > prev_pos)
        {
            if(tach.rpms <= gear_upper())
                tach.rpms += gear_->increase[cur_pos];
        }else
        {
            if(tach.rpms > gear_->decrease[cur_pos])
                tach.rpms -= gear_->decrease[cur_pos];
        }
    }

    prev_pos = cur_pos;
    tach.rpms_hist[tach.index] = tach.rpms;

    if(++tach.index == 10)
        tach.index = 0;
}
//#endif /* SIMULATE */


