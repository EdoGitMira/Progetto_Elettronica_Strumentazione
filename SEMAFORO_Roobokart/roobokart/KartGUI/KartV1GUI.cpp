#include "KartGUI.h"

#ifdef ROOBOKART_V1

KartGUI::KartGUI(KartV1GPIOE * gpioe1, KartV1GPIOE * gpioe2)
: display(gpioe1, gpioe2)
{

}

/* SAFE */

void KartGUI::screenClear(const color_t & color)
{
    
}

void KartGUI::screenTitle(const char * text, const color_t & foreground, const color_t & background)
{
    mutex.lock();
    queue(po_t::ScreenTitle, text, foreground, background);
    mutex.unlock();
}

void KartGUI::screenSplash(const char * text, const color_t & foreground, const color_t & background)
{
    
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
    
    int code = (int) error;
    char str[10];

    if ((code >= 0) && (code < 1000))
    {
        sprintf(str, "E%03d", code);
        display.setText(str);
    }
    else display.setText("Erro"); 
    
    str[5] = 0;
    queue(po_t::ScreenTitle, str, color_t::Clear);
    
    mutex.unlock();
}

void KartGUI::kartMode(const KartMode & mode)
{
    mutex.lock();

    int modeID = (int)mode;
    char str[10];

    if ((modeID >= 0) && (modeID < 100))
    {
        sprintf(str, "{%02d}", modeID);
        str[5] = 0;
        //display.setText(str);
        screenTitle(str);
    }
    else screenTitle("{NR}");    
    //queue(po_t::ScreenTitle, str, color_t::Clear);

    mutex.unlock();
}

void KartGUI::debugReset()
{
    
}

void KartGUI::debugClear(const color_t & color)
{
    
}
void KartGUI::debugMessage(const char * text,
                           const color_t & foreground,
                           const color_t & background)
{
     
}

void KartGUI::debugMessage(const char * text,
                           const int & value,                           
                           const color_t & foreground,
                           const color_t & background)
{    
    
}

void KartGUI::debugMessage(const char * text,
                           const float & value,
                           const color_t & foreground,
                           const color_t & background)
{
    
}

/* UNSAFE */

int KartGUI::private_init()
{
    return display.init();
}

void KartGUI::thread_switch(mail_t *mail)
{   
    char str[5];
    switch (mail->operation)
    { 
        case ScreenTitle:
            display.setText(mail->text);
            break;
        
        case LogoKart:
            display.setText("Robo");
            ThisThread::sleep_for(2000);
            display.setText("Kart");
            ThisThread::sleep_for(2000);         
            display.setText("PLT");
            break;
        
        default:
            break;
    }

    delete[] mail->text;
}

#endif