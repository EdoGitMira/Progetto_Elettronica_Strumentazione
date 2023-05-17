#ifndef __KART_GUI_H__
#define __KART_GUI_H__

#include "mbed.h"
#include "roobokart_def.h"
#include "KartTypes.h"
#include "KartError.h"

#if defined(ROOBOKART_V1)
#include "KartV1Display.h"
#elif defined(ROOBOKART_V3)  
#include "ST7735.h"
#include "74HC595.h"
#else
#endif

class KartGUI 
{
    typedef enum 
    {
        DebugClear,
        DebugMessage,
        DebugReset,
        LogoKart,
        ScreenClear,
        ScreenSplash,
        ScreenTitle
    }
    po_t;

    public:
    typedef enum : uint32_t
    {
        Black   = 0x00000000,
        Blue    = 0x000000FF,
        Clear   = 0xFF000000,
        Cyan    = 0x0000FFFF,
        Green   = 0x0000FF00,
        Magenta = 0x00FF00FF,
        Red     = 0x00FF0000,
        White   = 0x00FFFFFF,
        Yellow  = 0x00FFFF00
    }
    color_t;

    private:
    typedef struct
    {
        po_t operation;
        char * text;
        color_t foreground;
        color_t background;
    }
    mail_t;   
    
    Mail<mail_t, 10> mail_box;
    Thread thread;
    Mutex mutex;

    #if defined(ROOBOKART_V1)    
    KartV1Display display;
    #elif defined(ROOBOKART_V3)   
    ST7735 display;
    DigitalOut display_led;
    DigitalOut74HC595 display_reset;
    #else
    #endif
    
    public:
    #if defined(ROOBOKART_V1)
    KartGUI(KartV1GPIOE * gpioe1, KartV1GPIOE * gpioe2);
    #elif defined(ROOBOKART_V3)
    KartGUI(_74HC595 * shift_register);
    #else
    KartGUI();
    #endif   

    int init();
    
    void screenClear(const color_t & color = color_t::Black);
    void screenTitle(const char * text,
                     const color_t & foreground = color_t::White,
                     const color_t & background = color_t::Blue);
    void screenSplash(const char * text,
                      const color_t & foreground = color_t::White,
                      const color_t & background = color_t::Clear);
    void kartLogo();
    void kartMode(const KartMode & mode);
    void kartError(const KartError & error);
    void debugReset();
    void debugClear(const color_t & color = color_t::Black);
    void debugMessage(const char * text,
                      const color_t & foreground = color_t::White,
                      const color_t & background = color_t::Clear);
    void debugMessage(const char * text,
                      const int & value,                      
                      const color_t & foreground = color_t::White,                      
                      const color_t & background = color_t::Clear);
    void debugMessage(const char * text,
                      const float & value,
                      const color_t & foreground = color_t::White,
                      const color_t & background = color_t::Clear);

    private:    
    //void eofLock();
    //void eofUnlock(const bool & eof);
    int queue(const po_t & operation, const char * text, const color_t & foreground, const color_t & background = color_t::Clear);
    int private_init();
    void thread_main();
    void thread_switch(mail_t *mail);

    
};

#endif