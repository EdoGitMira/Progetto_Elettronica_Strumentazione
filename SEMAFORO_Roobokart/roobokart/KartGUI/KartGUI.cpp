#include "KartGUI.h"
#include "LockGuard.h"

int KartGUI::init()
{ 
    LockGuard guard(mutex);
    int error = private_init();
    if (error) return error;
    return thread.start(callback(this, &KartGUI::thread_main));   
    return 0; 
}


/* UNSAFE */
/*
void KartGUI::eofLock()
{
    if (_eof) 
    {
        mutex.lock();
        active = ThisThread::get_id();
    }
    else
    {
        if (ThisThread::get_id() != active) mutex.lock();
    }
}

void KartGUI::eofUnlock(const bool & eof)
{
    _eof = eof;
    if (_eof) mutex.unlock();
}*/

int KartGUI::queue(const po_t & operation, const char * text, const color_t & foreground, const color_t & background)
{    
    mail_t *mail = mail_box.alloc_for(osWaitForever);
    
    if (mail == nullptr) return osErrorNoMemory;
        
    if (text) 
    {
        mail->text = new char[strlen(text) + 1];
        strcpy(mail->text, text);
    }
    else 
    {
        mail->text = new char[1];
        mail->text[0] = 0;
    }

    mail->operation = operation;
    mail->foreground = foreground;
    mail->background = background;
    
    return mail_box.put(mail);
}

void KartGUI::thread_main()
{
    while (true) 
    {
        osEvent evt = mail_box.get();
        if (evt.status == osEventMail)
        {
            mail_t *mail = (mail_t *)evt.value.p;            
            if (mail == nullptr) continue;
            thread_switch(mail);
            mail_box.free(mail);
        }
    }
}