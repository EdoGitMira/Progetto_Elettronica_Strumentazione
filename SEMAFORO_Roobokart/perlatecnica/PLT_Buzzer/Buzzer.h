#ifndef __PLT_BUZZER_H__
#define __PLT_BUZZER_H__

#include "mbed.h"

class Buzzer                    
{
    PwmOut buzzer;
    Timeout timeout;
    Semaphore semaphore;
    Mutex mutex;
    uint32_t duration;   

    public:
    enum Frequencies
    {
        NOTE_B0  = 31  ,   // B  / Si
        NOTE_C1  = 33  ,   // C  / Do
        NOTE_CS1 = 35  ,   // C# / Do#
        NOTE_D1  = 37  ,   // D  / Re
        NOTE_DS1 = 39  ,   // D# / Re#
        NOTE_E1  = 41  ,   // E  / MI
        NOTE_F1  = 44  ,   // F  / Fa
        NOTE_FS1 = 46  ,   // F# / Fa#
        NOTE_G1  = 49  ,   // G  / Sol
        NOTE_GS1 = 52  ,   // G# / Sol#
        NOTE_A1  = 55  ,   // A  / La
        NOTE_AS1 = 58  ,   // A# / La#
        NOTE_B1  = 62  ,   // B  / Si
        NOTE_C2  = 65  ,   // C  / Do
        NOTE_CS2 = 69  ,   // C# / Do#
        NOTE_D2  = 73  ,   // D  / Re
        NOTE_DS2 = 78  ,   // D# / Re#
        NOTE_E2  = 82  ,   // E  / MI
        NOTE_F2  = 87  ,   // F  / Fa
        NOTE_FS2 = 93  ,   // F# / Fa#
        NOTE_G2  = 98  ,   // G  / Sol
        NOTE_GS2 = 104 ,   // G# / Sol#
        NOTE_A2  = 110 ,   // A  / La
        NOTE_AS2 = 117 ,   // A# / La#
        NOTE_B2  = 123 ,   // B  / Si
        NOTE_C3  = 131 ,   // C  / Do
        NOTE_CS3 = 139 ,   // C# / Do#
        NOTE_D3  = 147 ,   // D  / Re
        NOTE_DS3 = 156 ,   // D# / Re#
        NOTE_E3  = 165 ,   // E  / MI
        NOTE_F3  = 175 ,   // F  / Fa
        NOTE_FS3 = 185 ,   // F# / Fa#
        NOTE_G3  = 196 ,   // G  / Sol
        NOTE_GS3 = 208 ,   // G# / Sol#
        NOTE_A3  = 220 ,   // A  / La
        NOTE_AS3 = 233 ,   // A# / La#
        NOTE_B3  = 247 ,   // B  / Si
        NOTE_C4  = 262 ,   // C  / Do
        NOTE_CS4 = 277 ,   // C# / Do#
        NOTE_D4  = 294 ,   // D  / Re
        NOTE_DS4 = 311 ,   // D# / Re#
        NOTE_E4  = 330 ,   // E  / MI
        NOTE_F4  = 349 ,   // F  / Fa
        NOTE_FS4 = 370 ,   // F# / Fa#
        NOTE_G4  = 392 ,   // G  / Sol
        NOTE_GS4 = 415 ,   // G# / Sol#
        NOTE_A4  = 440 ,   // A  / La
        NOTE_AS4 = 466 ,   // A# / La#
        NOTE_B4  = 494 ,   // B  / Si
        NOTE_C5  = 523 ,   // C  / Do
        NOTE_CS5 = 554 ,   // C# / Do#
        NOTE_D5  = 587 ,   // D  / Re
        NOTE_DS5 = 622 ,   // D# / Re#
        NOTE_E5  = 659 ,   // E  / MI
        NOTE_F5  = 698 ,   // F  / Fa
        NOTE_FS5 = 740 ,   // F# / Fa#
        NOTE_G5  = 784 ,   // G  / Sol
        NOTE_GS5 = 831 ,   // G# / Sol#
        NOTE_A5  = 880 ,   // A  / La
        NOTE_AS5 = 932 ,   // A# / La#
        NOTE_B5  = 988 ,   // B  / Si
        NOTE_C6  = 1047,   // C  / Do
        NOTE_CS6 = 1109,   // C# / Do#
        NOTE_D6  = 1175,   // D  / Re
        NOTE_DS6 = 1245,   // D# / Re#
        NOTE_E6  = 1319,   // E  / MI
        NOTE_F6  = 1397,   // F  / Fa
        NOTE_FS6 = 1480,   // F# / Fa#
        NOTE_G6  = 1568,   // G  / Sol
        NOTE_GS6 = 1661,   // G# / Sol#
        NOTE_A6  = 1760,   // A  / La
        NOTE_AS6 = 1865,   // A# / La#
        NOTE_B6  = 1976,   // B  / Si
        NOTE_C7  = 2093,   // C  / Do
        NOTE_CS7 = 2217,   // C# / Do#
        NOTE_D7  = 2349,   // D  / Re
        NOTE_DS7 = 2489,   // D# / Re#
        NOTE_E7  = 2637,   // E  / MI
        NOTE_F7  = 2794,   // F  / Fa
        NOTE_FS7 = 2960,   // F# / Fa#
        NOTE_G7  = 3136,   // G  / Sol
        NOTE_GS7 = 3322,   // G# / Sol#
        NOTE_A7  = 3520,   // A  / La
        NOTE_AS7 = 3729,   // A# / La#
        NOTE_B7  = 3951,   // B  / Si
        NOTE_C8  = 4186,   // C  / Do
        NOTE_CS8 = 4435,   // C# / Do#
        NOTE_D8  = 4699,   // D  / Re
        NOTE_DS8 = 4978,   // D# / Re#
    };   

    Buzzer(const PinName & pin);
    void tone(uint32_t note_frequency, uint32_t duration_ms);
    
    private:
    void start();
    void stop();
};

#endif