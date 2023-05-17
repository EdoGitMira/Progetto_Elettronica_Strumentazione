#include "KartGUI.h"

#define DISPLAY_W2_MAX_CHAR 13
#define DISPLAY_BORDER_DIST 3
#define DISPLAY_H_TITLE_BOX 17

#ifdef ROOBOKART_V3

KartGUI::KartGUI(_74HC595 * shift_register)
: display_reset(shift_register, _74HC595::GPIO::GPIO_7),
  display_led(DISPLAY_LED),
  display(SPI3_MOSI, SPI3_MISO, SPI3_SCK, DISPLAY_CS, DISPLAY_A0, NC)
{
    
}

/* SAFE */


void KartGUI::screenClear(const color_t & color)
{
    mutex.lock();
    queue(po_t::ScreenClear, "", color_t::Clear, color);
    mutex.unlock();
}

void KartGUI::screenTitle(const char * text, const color_t & foreground, const color_t & background)
{
    mutex.lock();
    queue(po_t::ScreenTitle, text, foreground, background);
    mutex.unlock();
}

void KartGUI::screenSplash(const char * text, const color_t & foreground, const color_t & background)
{
    mutex.lock();
    queue(po_t::ScreenSplash, text, foreground, background);
    mutex.unlock();
}

void KartGUI::kartLogo()
{
    mutex.lock();
    queue(po_t::LogoKart, "", color_t::Clear);
    mutex.unlock();
}

void KartGUI::kartError(const KartError & error)
{
    mutex.lock();   
    
    char str [100];
    
    sprintf(str, "ERROR: %d", error.getCode());
    screenSplash(str, color_t::White, color_t::Blue);
    debugMessage(error.getDeviceName());

    mutex.unlock();
}

void KartGUI::kartMode(const KartMode & mode)
{
    mutex.lock();

    screenTitle(toString(mode));
    debugClear();
    
    mutex.unlock();
}

void KartGUI::debugReset()
{
    mutex.lock();
    queue(po_t::DebugReset, "", color_t::Clear);
    mutex.unlock();
}

void KartGUI::debugClear(const color_t & color)
{
    mutex.lock();
    queue(po_t::DebugClear, "", color);
    mutex.unlock();
}
void KartGUI::debugMessage(const char * text,
                           const color_t & foreground,
                           const color_t & background)
{
    mutex.lock();
    queue(po_t::DebugMessage, text, foreground, background);
    mutex.unlock();  
}

void KartGUI::debugMessage(const char * text,
                           const int & value,                           
                           const color_t & foreground,
                           const color_t & background)
{    
    mutex.lock();   
    char str[10];
    sprintf(str, "%s%d", text, value); 
    queue(po_t::DebugMessage, str, foreground, background);
    mutex.unlock();
}

void KartGUI::debugMessage(const char * text,
                           const float & value,
                           const color_t & foreground,
                           const color_t & background)
{
    mutex.lock();
    char str[10];
    sprintf(str, "%s%f", text, value);
    queue(po_t::DebugMessage, str, foreground, background);
    mutex.unlock();
}

/* UNSAFE */

int KartGUI::private_init()
{    
    int error;
    
    error = display_reset.write(0);
    if (error) return error;
    
    ThisThread::sleep_for(100);

    error = display_reset.write(1);
    if (error) return error;
    
    display.initR(ST7735::InitR::BlackTab18);
    display.setRotation(3);       
    display_led.write(0);
    return 0;
}

void KartGUI::thread_switch(mail_t *mail)
{
    switch (mail->operation)
    {        
        case DebugMessage:
            display.setTextColor(display.Color565(mail->foreground >> 16, mail->foreground >> 8, mail->foreground));
            display.setTextSize(1);
            display.setCursorX(DISPLAY_BORDER_DIST);
            display.printf("%s\r\n", mail->text);
            break;      

        case ScreenTitle:
            display.fillRect(0, 0, display.width(), DISPLAY_H_TITLE_BOX, display.Color565(mail->background >> 16, mail->background >> 8, mail->background));
            display.setTextColor(display.Color565(mail->foreground >> 16, mail->foreground >> 8, mail->foreground));
            display.setTextSize(2);
            if (strlen(mail->text) > DISPLAY_W2_MAX_CHAR) mail->text[DISPLAY_W2_MAX_CHAR] = 0; 
            display.setCursorX(6 * (DISPLAY_W2_MAX_CHAR-strlen(mail->text)) + DISPLAY_BORDER_DIST);
            display.setCursorY(0);
            display.printf("%s", mail->text);
            break;

        case DebugClear:
            display.fillRect(0, DISPLAY_H_TITLE_BOX, display.width(), 112, display.Color565(mail->background >> 16, mail->background >> 8, mail->background));            

        case DebugReset:
            display.setCursorX(DISPLAY_BORDER_DIST);
            display.setCursorY(DISPLAY_H_TITLE_BOX + DISPLAY_BORDER_DIST);
            break;
        
        case ScreenSplash:            
            display.setCursorY(30);
            if (strlen(mail->text) > DISPLAY_W2_MAX_CHAR) mail->text[DISPLAY_W2_MAX_CHAR] = 0; 
            display.setCursorX(6 * (DISPLAY_W2_MAX_CHAR-strlen(mail->text)) + DISPLAY_BORDER_DIST);
            if (mail->background != color_t::Clear) display.fillScreen(display.Color565(mail->background>>16, mail->background>>8, mail->background));
            display.setTextColor(display.Color565(mail->foreground >> 16, mail->foreground >> 8, mail->foreground));
            display.setTextSize(2);
            display.printf("%s", mail->text);
            display.setCursorY(50);
            break;
        
        case ScreenClear: 
            display.fillScreen(display.Color565(mail->background>>16, mail->background>>8, mail->background));
            break;

        case LogoKart:
            display.fillScreen(ST7735::Black);
            display.setCursorX(6*4);
            display.setCursorY(10);
            display.setTextSize(2);

            display.setTextColor(ST7735::Green);
            display.printf("Roobo");

            display.setTextColor(ST7735::White); 
            display.printf("Kart");

            display.setCursorX(6*5);
            display.setCursorY(30);

            display.setTextSize(1);
            display.printf("by ");
            display.setTextColor(ST7735::Blue);
            display.printf("PERLATECNICA");
            display.setCursorX(DISPLAY_BORDER_DIST);
            display.setCursorY(50);
            break;
        
        default:
            break;
    }

    delete[] mail->text;
}

#endif
    /*
    int temp = data;
    display.printf(" %s: %d.", label, temp);
    data -= temp;
    data = abs(data);
    
    do
    {
        data *= 10;
        temp = data;
        display.printf("%d", temp);
        temp=data;
        dec_len--;
    }
    while(dec_len>0);

    display.printf("\r\n");
    */        