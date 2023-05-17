#include "KartGUI.h"

#ifdef ROOBOKART_M1

KartGUI::KartGUI()
{

}

/* SAFE */

void KartGUI::screenClear(const color_t & color)
{
    
}

void KartGUI::screenTitle(const char * text, const color_t & foreground, const color_t & background)
{
    
}

void KartGUI::screenSplash(const char * text, const color_t & foreground, const color_t & background)
{
    
}

void KartGUI::kartLogo()
{

}

void KartGUI::kartError(const KartError & error)
{
   
}

void KartGUI::kartMode(const KartMode & mode)
{
    
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
    return 0;
}

void KartGUI::thread_switch(mail_t *mail)
{   
    char str[5];
    switch (mail->operation)
    {           
        default:
            break;
    }

    delete[] mail->text;
}

#endif