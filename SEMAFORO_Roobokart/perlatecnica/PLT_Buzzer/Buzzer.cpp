#include "Buzzer.h"

Buzzer::Buzzer(const PinName & pin)
: buzzer(pin)
{

}  

void Buzzer::tone(uint32_t note_frequency, uint32_t duration_ms)
{
    mutex.lock();

    if (note_frequency)
    {
        buzzer.period(1.0f/(float)note_frequency);
        duration = duration_ms;
        timeout.attach_us(callback(this, &Buzzer::start), 1);
    }
    else
    {
        timeout.attach(callback(this, &Buzzer::stop), 0.001f * duration_ms);
    }

    semaphore.acquire();
    mutex.unlock();
}

void Buzzer::start()
{
    buzzer.write(0.5f);
    timeout.attach(callback(this, &Buzzer::stop), 0.001f * duration);
}

void Buzzer::stop()
{
    buzzer.write(0.0f);
    semaphore.release();
}

/*
void Buzzer::play(float period, float duty_cycle, uint32_t duration)
{
    buzzer.period(period);
    buzzer.write(duty_cycle);
    ThisThread::sleep_for(duration);
    buzzer.write(0.0f);
}

void Buzzer::playTone(Note note, uint32_t beat, uint8_t volume)
{   
    play(1.0f/(float)note, (float)volume/255.0f, 1000/beat);
    ThisThread::sleep_for(50);
}

void Buzzer::playSong(const Note * notes, const uint32_t * beats, uint32_t len, uint8_t volume)
{        
    for (uint32_t i = 0; i < len; i++)
    {
        playTone(notes[i], beats[i], volume);
    }
}*/