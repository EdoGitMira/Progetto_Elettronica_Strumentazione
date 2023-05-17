#ifndef __KART_V1_DISPLAY_H__
#define __KART_V1_DISPLAY_H__

#include "KartV1GPIOE.h"
#include <cstdint>

#define KV1DM_MASK (KartV1GPIOE::GPIOName)0b0011111111111111

/** decimal point bit mapping*  */
#define DP  (1 << 7)

/** sgement s0 bit mapping*/
#define S0 (1 << 3)
/** sgement s1 bit mapping*/
#define S1 (1 << 5)
/** sgement s2 bit mapping*/
#define S2 (1 << 6)
/** sgement s3 bit mapping*/
#define S3 (1 << 4)
/** sgement s4 bit mapping*/
#define S4 (1 << 0)
/** sgement s5 bit mapping*/
#define S5 (1 << 1)
/** sgement s6 bit mapping*/
#define S6 (1 << 2)
#define NOT_7_NO_DP( ... ) (uint8_t) ~( __VA_ARGS__ + DP )

class KartV1Display
{
    KartV1GPIOE * dev1;
    KartV1GPIOE * dev2;
    
    // https://en.wikipedia.org/wiki/Seven-segment_display_character_representations
    static constexpr uint8_t ascii_to_display_flip[95] = 
    {
        0, // space
        0, // !
        0b1000001, // "
        0, // #
        0, // $
        0, // %
        0, // &
        0b1000000, // '
        0b1111000, // (
        0b0011001, // )
        0b0001100, // *
        0b0000111, // +
        0b0100000, // ,
        0b0000100, // -
        0b0001000, // .
        0b0000011, // /
        0b1111011, // 0
        0b0000011, // 1
        0b0111101, // 2
        0b0011111, // 3
        0b1000111, // 4
        0b1011110, // 5
        0b1111110, // 6
        0b0010011, // 7
        0b1111111, // 8
        0b1011111, // 9
        0b0011000, // :
        0b0110000, // ;
        0b0000111, // <
        0b0001100, // =
        0b1100100, // >
        0b0110101, // ?
        0b0011110, // @
        0b1110111, // A
        0b1111111, // B
        0b1111000, // C
        0b0111011, // D
        0b1111100, // E
        0b1110100, // F
        0b1111010, // G
        0b1100111, // H
        0b1100000, // I
        0b0001011, // J
        0b1110110, // K
        0b1101000, // L
        0b1011001, // M
        0b1110011, // N
        0b1111011, // O
        0b1110101, // P
        0b1011101, // Q
        0b1111101, // R
        0b1011110, // S
        0b1110000, // T
        0b1101011, // U
        0b1001011, // V
        0b0111010, // W
        0b0011100, // X
        0b1001101, // Y
        0b0000011, // Z
        0b1111000, // [
        0b1100000, // opp /
        0b0011001, // ]
        0b0010000, // ^
        0b0001000, // _
        0b1000000, // `
        0b0001110, // a
        0b1101110, // b
        0b1111000, // c
        0b0101111, // d
        0b0101000, // e
        0b1100100, // f
        0b0111100, // g
        0b1100110, // h
        0b0111000, // i
        0b0011010, // j
        0b1011100, // k
        0b1100000, // l
        0b0110110, // m
        0b0100110, // n
        0b0101110, // o
        0b1110101, // p
        0b1010111, // q
        0b0100100, // r
        0b0001010, // s
        0b1101100, // t
        0b0101010, // u
        0b0001010, // v
        0b1001001, // w
        0b0001100, // x
        0b1001111, // y
        0b0001100, // z
        0b0000111, // {
        0b1100000, // |
        0b1100100, // }
        0b0100101  // ~ 


    };

    public: 
    KartV1Display(KartV1GPIOE * gpioe1, KartV1GPIOE * gpioe2) : dev1(gpioe1), dev2(gpioe2)
    {

    }

    int init()
    {
        if (dev1==nullptr) return 1;
        if (dev2==nullptr) return 1;

        dev1->setMode(KV1DM_MASK, KartV1GPIOE::OUTPUT);
        dev2->setMode(KV1DM_MASK, KartV1GPIOE::OUTPUT);
        dev1->write(KV1DM_MASK, 0);
        dev2->write(KV1DM_MASK, 0);

        return 0;
    }

    int setText(const char * text)
    {
        int error = 0;
        uint16_t c1, c2;

        c1 = getChar(text[0]);
        c1 <<= 7;
        c1 |= 0b01111111 & getChar(text[1]);
        
        c2 = getChar(text[2]);
        c2 <<= 7;
        c2 |= 0b01111111 & getChar(text[3]);
        
        error = dev1->writeMask(KV1DM_MASK, c1);
        if (error) return error;

        error = dev2->writeMask(KV1DM_MASK, c2);
        return error;
    }

    private:
    uint8_t getChar(uint8_t c)
    {
        if (c<32) return 255;
        if (c>126) return 255;
        c-=32;
        return ~ascii_to_display_flip[c];
    }
};

#endif