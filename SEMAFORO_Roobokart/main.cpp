/*----------------------------------------------------------------------------------------------------------------------------------------
PROGETTO: ROBOKART-SEMAFORO
CORSO: LABORATORIO ELETTORNICA
UNIVERSITA DEGLI STUDI DI BRESCIA
A.A.:2022/23
VERSION: 3.1
STUDENTI :  - MIRANDOLA EDOARDO -> e.mirandola@studenti.unibs.it
            - PENNACCHIO MANUELE -> m.pennacchio006@studenti.unibs.it
            - FARES PETER WILLIAM GUIRGUIS -> p.fares001@studenti.unibs.it
----------------------------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------------------------
=====================INCLUDE=====================
----------------------------------------------------------------------------------------------------------------------------------------*/
#include "mbed.h"
#include "roobokart.h"
#include <cstdio>
#include "PID.h"

/*----------------------------------------------------------------------------------------------------------------------------------------
=====================DEFINIZIONE ROBOKART=====================
----------------------------------------------------------------------------------------------------------------------------------------*/
Roobokart kart;

/*----------------------------------------------------------------------------------------------------------------------------------------
=====================DEFINIZIONE COSTANTI GLOBALI=====================
----------------------------------------------------------------------------------------------------------------------------------------*/
const float SOGLIA_BLU = 0.5;       //soglia della banda blu  del codice a barre
const float SOGLIA_BIANCO = 0.1;    //soglia del colore bianco 
const float SOGLIA_NERO = 0.4;      //soglia del colore nero all'interno del codice a barre
const float SOGLIA_BARCODE = 0.3;   //soglia del colore bianco all'interno del codice a barre
const float SOGLIA_GREEN = 0.4;     //soglia del colore verde della striscia luminosa del semaforo
const float SOGLIA_GREEN_MIN = 0.1; //soglia del colore verde della striscia luminosa del semaforo minimo

/*----------------------------------------------------------------------------------------------------------------------------------------
=====================DEFINIZIONE VARIABILI GLOBALI=====================
----------------------------------------------------------------------------------------------------------------------------------------*/
bool b0 = false; // bit 0 del cpodice a barre
bool b1 = false; // bit 1 del cpodice a barre

/*----------------------------------------------------------------------------------------------------------------------------------------
=====================DEFINIZIONE PROTOTIPI FUNZIONE=====================
----------------------------------------------------------------------------------------------------------------------------------------*/
void setup();                 //funzione per eseguire le funzioni di set up e attendere la stabilizzazione del giroscopio
void aspetta_pulsante();      //funzione per leggere il pulsante per avviare la macchina a stati
void attraversamento();       //funzione che gestisce la movimentazione in open loop dell'incrocio utilizzando il gyroscopio
void barcode();               //funzione per la lettura del codice a barre e come feedback restituisce due segnali acustici
void semaforo();              //funzione per la gestione del semaforo
void line_assist();           //funzione utilizzata per eseguire il line assist
float lettura_ir_stab_dx();   //funzione per la lettura stabilizzazata del sensore di destra più esterno
float lettura_ir_stab_sx();   //funzione per la lettura stabilizzazata del sensore di sinistra

/*----------------------------------------------------------------------------------------------------------------------------------------
=====================CODICE MAIN=====================
----------------------------------------------------------------------------------------------------------------------------------------*/
int main(){

    setup();//inizializzazione della machcina  allo sttao base

    aspetta_pulsante();//attesa della pressione del pulsante  per avviare la gestione del robot

    int statofun = 1;//settaggio dello stato iniziale del robot
    
    //ciclo infinito per eseguire il clock
    while(true){
        switch(statofun){
            case 1://segui la linea fino al segnale
                line_assist();
                statofun = 2;//passaggio allo stato per la lettura del a barcode
                break;

            case 2://lettura codice barre
                barcode();
                statofun = 3;//passaggio a stato per identificazione semaforo
                break;

            case 3://attesa semaforo verde
                semaforo();
                statofun = 4;//passaggio allo stato 4 per attraversare il semaforo
                /*
                if(b0 ==1 && b1 ==1){
                    statofun = 4
                }else if(b0 ==0 && b1 == 1){
                    statofun = 5
                }
                else if(b0 ==0 && b1 == 1){
                    statofun = 6
                }
                else{
                    break;
                }
                */
                break;
            
            case 4://vai dritto oltre il semaforo
                attraversamento();
                statofun = 1;
                break;

            case 5://curva a sinistra
                attraversamento();
                statofun = 1;
                break;

            case 6://curva a destra
                attraversamento();
                statofun = 1;
                break;
        }
    }
}

/*----------------------------------------------------------------------------------------------------------------------------------------
aspetta_pulsante => funzione utilizzata per dare il consenso all'avvio della macchina a stati
----------------------------------------------------------------------------------------------------------------------------------------*/
void aspetta_pulsante() {
    //========== Definizione delle costanti e degli oggetti====================
    const int BUZZ = 1000;
    DigitalIn button(BUTTON1);
    //==========ciclo infinito fino alla pressione del pulsante ===============
    while (button.read()) {
       ;
    }
    // ==========buzzer che segnala l'inizio del main ========================
    kart.buzzer.tone(BUZZ, 50);
}

/*----------------------------------------------------------------------------------------------------------------------------------------
setup => funzione per l'inizzializzazione del robokart e per settaggio del gyroscopio
----------------------------------------------------------------------------------------------------------------------------------------*/
void setup() {
    //========== Definizione delle costanti ====================
    const int BUZZ = 1000;
    //===================== Inizializzazione====================
    kart.init();
    kart.gyro.init();
    kart.gyro.waitForYawStab();
    kart.gyro.resetYaw();
    kart.gyro.resetRoll();
    kart.gyro.resetPitch();
    kart.gyro.setYawOffset(-90);
    kart.gyro.setRollOffset(-90);
    kart.gyro.setPitchOffset(-90);
    //========= Buzzer alla fine dell'inizializzazione===========
    kart.buzzer.tone(BUZZ, 20);
}

/*----------------------------------------------------------------------------------------------------------------------------------------
semaforo => funzione semaforo che permette di eseguire una funzione per la lettura e fermata ddel robot al semaforo
----------------------------------------------------------------------------------------------------------------------------------------*/
void semaforo(){
    kart.movements.stop();
    kart.trafficlight.read();
    while(kart.trafficlight.getGreenPercent() < SOGLIA_GREEN && kart.trafficlight.getGreenPercent() > SOGLIA_GREEN_MIN){
       kart.buzzer.tone(2000, 1);
       kart.trafficlight.read();
       wait_ms(5);
  }
}

/*----------------------------------------------------------------------------------------------------------------------------------------
attraversamento => unzione utilizzata per l'attraversamento rettilineo dopo aver letto il semaforo
                #la sfrutta la libreria pre-esistente per <PID.h>
                #per mantenere l'orientamento viene utilizzato il giroscopio
                #la velocità e il tempo della movimentazione vengono definite dalle costanti SPEED e SOGLIA_TEMPO
----------------------------------------------------------------------------------------------------------------------------------------*/
void attraversamento(){
    //=========================Dichiarazione delle costanti =========================
    const int SOGLIA_TEMPO = 2100;
    const float SPEED = 40;
    const float SETPOINT = kart.gyro.getYaw(); //get the set point 
    const float MINDIR = -100; //necessario per il metodo dirPID->evaluate
    const float MAXDIR = 100; //necessario per il metodo dirPID->evaluate
    //parametri PID
    const int Kp = 1;
    const int Ki = 0;
    const int Kd = 0;

    //=========================Oggetti e variabili =========================
    Timer pid_timer;
    pid_timer.start();
    Timer mov_timer;
    mov_timer.start();
    PID < float > * dirPID = new PID < float > (Kp, Ki, Kd); //definizione del controllore
    dirPID -> reset();
    float direction;

    //================Calcolo e applicazione dell'azione di controllo============
    while (mov_timer.read_ms() < SOGLIA_TEMPO) {
        pid_timer.reset();
        direction = dirPID -> evaluate(pid_timer.read(), SETPOINT, kart.gyro.getYaw(), MINDIR, MAXDIR);
        kart.movements.tank(direction, SPEED);
        wait_ms(10);
    }

    //====== Frenata per consentire un migliore ingresso del line-assist ==========
    kart.movements.brake();

    //========================= reset e stop dei timer ============================
    mov_timer.stop();
    mov_timer.reset();
    pid_timer.stop();
    pid_timer.reset();
}

/*----------------------------------------------------------------------------------------------------------------------------------------
barcode => per leggere il codice a barre posto prima del semaforo
        # per la lettura vengono utilizzate le funzioni lettura_ir_stab_dx e lettura_ir_stab_sx
        # la lettura del codice avviene in corrispondenza delle fasce nere
        # a seconda del valore letto viene emesso un suono differente dal buzzer (grave=0, acuto=1)
        # alla fine della lettura la velocità viene abbassata per passare alla lettura del semaforo
----------------------------------------------------------------------------------------------------------------------------------------*/
void barcode(){
    //=======================Dichiarazione delle costanti=======================
    const float SPEED_1=25;
    const float SPEED_2=20;
    const int BUZZ_TRUE=1500;
    const int BUZZ_FALSE=500;
    const float MINDIR=-5;//necessario per il metodo dirPID->evaluate
    const float MAXDIR=5;//necessario per il metodo dirPID->evaluate
    const float SETPOINT = kart.gyro.getYaw();//get the set point 
    //parametri PID
    const int Kp = 1;
    const int Ki = 0;
    const int Kd=0;

    PID<float> *dirPID_barcode = new PID<float>(Kp,Ki,Kd);//definizione del controllore
    dirPID_barcode->reset();
    Timer pid_timer;
    pid_timer.start();


    //======================= Set velocità iniziale =======================
    //kart.motorLeft.setSpeed(SPEED_1);
    //kart.motorRight.setSpeed(SPEED_1);

    //======================== Lettura del codice =========================
    //avanti fino alla riga bianca
    while (lettura_ir_stab_dx()>SOGLIA_BIANCO){
        pid_timer.reset();
        kart.movements.tank(dirPID_barcode->evaluate(pid_timer.read(),SETPOINT,kart.gyro.getYaw(),MINDIR,MAXDIR), SPEED_1);
        wait_ms(5);
    }
    //avanti oltre la riga bianca
    while (lettura_ir_stab_dx()<SOGLIA_BIANCO){
        pid_timer.reset();
        kart.movements.tank(dirPID_barcode->evaluate(pid_timer.read(),SETPOINT,kart.gyro.getYaw(),MINDIR,MAXDIR), SPEED_1);
        wait_ms(5);
    }
    //=============lettura in prossimità della riga nera ==================
    while (lettura_ir_stab_dx()<SOGLIA_NERO){
        pid_timer.reset();
        kart.movements.tank(dirPID_barcode->evaluate(pid_timer.read(),SETPOINT,kart.gyro.getYaw(),MINDIR,MAXDIR), SPEED_1);
        wait_ms(5);
    }
    //lettura b0
    if(lettura_ir_stab_sx()<SOGLIA_BARCODE){
        kart.buzzer.tone(BUZZ_TRUE,20);
        b0 = true;
    }
    else{
        kart.buzzer.tone(BUZZ_FALSE,20);
        b0 = false;
    }

    //avanti oltre la riga nera
    while (lettura_ir_stab_dx()>SOGLIA_NERO){
        pid_timer.reset();
        kart.movements.tank(dirPID_barcode->evaluate(pid_timer.read(),SETPOINT,kart.gyro.getYaw(),MINDIR,MAXDIR), SPEED_1);
        wait_ms(5);
    }
    //avanti oltre la linea bianca
    while (lettura_ir_stab_dx()<SOGLIA_BIANCO){
        pid_timer.reset();
        kart.movements.tank(dirPID_barcode->evaluate(pid_timer.read(),SETPOINT,kart.gyro.getYaw(),MINDIR,MAXDIR), SPEED_1);
        wait_ms(5);
    }
    //=============lettura in prossimità della riga nera =================
    while(lettura_ir_stab_dx()<SOGLIA_NERO){
        pid_timer.reset();
        kart.movements.tank(dirPID_barcode->evaluate(pid_timer.read(),SETPOINT,kart.gyro.getYaw(),MINDIR,MAXDIR), SPEED_1);
        wait_ms(5);
    }
    //lettura b0
    if(lettura_ir_stab_sx()<SOGLIA_BARCODE){
        kart.buzzer.tone(BUZZ_TRUE,20);
        b1 = true;
    }
    else{
        kart.buzzer.tone(BUZZ_FALSE,20);
        b1 = false;
    }

    //============= Avanti lento dopo la lettura del segnale =================
    pid_timer.reset();
    kart.movements.tank(dirPID_barcode->evaluate(pid_timer.read(),SETPOINT,kart.gyro.getYaw(),MINDIR,MAXDIR), SPEED_2);
    return;
}

/*----------------------------------------------------------------------------------------------------------------------------------------
lettura_ir_stab_dx => Funzione per stabilizzare la lettura del sensore IR destro più esterno IR_right_0:
                    # La funzione termina nel caso in cui 3 letture consecutive (5ms l'una dall'altra)
                      presentano una differenza inferiore al valore pre-stabilito ERR_MAX_irLeft
                    # in alternativa continua continua a leggere dall'IR
RETURN => il valore float di ritorno corrisponde alla media nunmerica delle tre letture
----------------------------------------------------------------------------------------------------------------------------------------*/
float lettura_ir_stab_dx(){
    //====================Dichiarazione delle costanti=====================
    const float ERR_MAX_irRight = 0.05;
    //====================Dichiarazione delle variabili=====================
    float letture_ir[3] = {
        ERR_MAX_irRight + 1,
        0,
        0
    };

    //====================Lettura e controllo dei valori=====================
    while ((abs(letture_ir[0] - letture_ir[1]) > ERR_MAX_irRight) || (abs(letture_ir[2] - letture_ir[1]) > ERR_MAX_irRight) || (abs(letture_ir[0] - letture_ir[2]) > ERR_MAX_irRight)) {
        wait_ms(5);
        letture_ir[0] = kart.irRight_0.read();
        wait_ms(5);
        letture_ir[1] = kart.irRight_0.read();
        wait_ms(5);
        letture_ir[2] = kart.irRight_0.read();
    }
    return ((letture_ir[0] + letture_ir[1] + letture_ir[2]) / 3);
}

/*----------------------------------------------------------------------------------------------------------------------------------------
lettura_ir_stab_sx => Funzione per stabilizzare la lettura del sensore IR di sinistra:
                    # La funzione termina nel caso in cui 3 letture consecutive (5ms l'una dall'altra)
                      presentano una differenza inferiore al valore pre-stabilito ERR_MAX_irLeft
                    # in alternativa continua continua a leggere dall'IR
RETURN => il valore float di ritorno corrisponde alla media nunmerica delle tre letture
----------------------------------------------------------------------------------------------------------------------------------------*/
float lettura_ir_stab_sx(){
    //====================Dichiarazione delle costanti=====================
    const float ERR_MAX_irLeft = 0.05;
    //====================Dichiarazione delle variabili=====================
    float letture_ir[3] = {
        ERR_MAX_irLeft + 1,
        0,
        0
    };
    //====================Lettura e controllo dei valori=====================
    while((abs(letture_ir[0] - letture_ir[1]) > ERR_MAX_irLeft) || (abs(letture_ir[2] - letture_ir[1]) > ERR_MAX_irLeft) || (abs(letture_ir[0] - letture_ir[2]) > ERR_MAX_irLeft)) {
        wait_ms(5);
        letture_ir[0] = kart.irLeft.read();
        wait_ms(5);
        letture_ir[1] = kart.irLeft.read();
        wait_ms(5);
         letture_ir[2] = kart.irLeft.read();
    }
    return ((letture_ir[0] + letture_ir[1] + letture_ir[2]) / 3);
}

/*----------------------------------------------------------------------------------------------------------------------------------------
line_assisst => funzione per eseguire il follow line della line bianca a destra
                vengo utilizzati i 3 sensori IR sulla destra per capire come il
                robot è posizionato e di conseguenza fare una correzione della 
                traiettoria mediante un controllore PID
----------------------------------------------------------------------------------------------------------------------------------------*/
void line_assist(){
    //========== Dichiarazione delle costanti ======================
    const float Kp = 27.5; // valore di KP
    const float Ki = 8.5; //valore di KI
    const float SPEED = 30; //velocità del line assist
    const float U_max = 17; //saturazione dell'azione di controllo
    const float TARGET = 0.0; //set-point (3 sensori IR sul nero)

    //========== Dichiarazione di oggetti e variabili ===============
    Timer tPid;
    float e_tot = 0.0;
    float e = 0.0;
    float i = 0.0;
    float p = 0.0;//azione proporzionale
    float u = 0.0;//azione di controllo

    while(kart.irCenter.read() > SOGLIA_BLU) {
        //======== Lettura e calcolo dell'errore ======
        e = kart.road.readLine() - TARGET;
        //============= Anti wind-up ==================
        if (abs(u) > 40 && u * e > 0) {
          e = 0;
        }
        //==== Calcolo dell'azione di constrollo u =====
        p = Kp * e;// azione proporzionale
        e_tot = e_tot + e * tPid.read();//calcolo dell'errore per l'azione integrale
        tPid.reset();
        tPid.start();
        i = Ki * e_tot; // azione integrale
        u = p + i;//azione di controllo
        //=== Saturazione dell'azione di controllo ====
        if(abs(u) > U_max) {
            if(u < 0){
                 u = -U_max;
            }else{
                u = U_max;
            }
        }
        // =========== Azione di controllo =============
        kart.movements.tank(u, SPEED);
    }
}

