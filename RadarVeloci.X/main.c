#include<proc/pic18f4520.h>
#include"adc.h"
#include"bits.h"
#include"config.h"
#include"serial.h"
#include"ds1307.h"
#include"keypad.h"
#include"lcd.h"
#include"ssd.h"
#include"timer.h"
#include"pwm.h"
#include "rgb.h"
#include"atraso.h"

#define L_ON 0x0F
#define L_OFF 0X08
#define L_CLR 0x01
#define L_L1 0x80 //defines para o LCD
#define L_L2 0xC0

unsigned int multa;
unsigned long int limite;
unsigned long int pot;
unsigned char op; //escolha do serial
unsigned long int velocidade;

void levelocidade() { //Realiza leitura do pot e atualiza LCD
    unsigned long int valantigo;
    pot = adcRead(0);
    if (valantigo != pot) { //checa se mudou
        lcdPosition(1, 7);
        lcdString("V:");
        lcdChar((((200 * pot) / 1023) + 20) / 100 % 10 + 48);
        lcdChar((((200 * pot) / 1023) + 20) / 10 % 10 + 48);
        lcdChar((((200 * pot) / 1023) + 20) % 10 + 48);
    }//end if
    valantigo = pot;
}//end velocidade

void mostraDisplay() {
    int v0, v1, v2, v3;
    v0 = ((multa / 1000) % 10);
    v1 = ((multa / 100) % 10);
    v2 = ((multa / 10) % 10);
    v3 = (multa % 10);
    ssdDigit(v0, 0);
    ssdDigit(v1, 1);
    ssdDigit(v2, 2);
    ssdDigit(v3, 3);
}//end mostra display

void apagaDisplay() {
    ssdDigit(0, 0); //desliga ssd
    ssdDigit(0, 1); //desliga ssd
    ssdDigit(0, 2); //desliga ssd
    ssdDigit(0, 3); //desliga ssd
}

void leTeclado(void) {
    unsigned char tecla;
    kpDebounce();
    if (kpRead() != tecla) {
        tecla = kpRead();
        switch (kpReadKey()) {
            case 'U': //imprime RUA 1
                lcdCommand(L_L1);
                lcdString("RUA XV  ");
                lcdCommand(L_L2);
                lcdString("L: 60 ");
                limite = 60;
                multa = 100 + ((pot * 10) / 100); //multa fixa para cada lugar
                break;
            case 'L': //imprime RUA 2
                lcdCommand(L_L1);
                lcdString("AVENIDA   ");
                lcdCommand(L_L2);
                lcdString("L: 80 ");
                limite = 80;
                multa = 200 + ((pot * 20) / 100);
                break;
            case 'D': //imprime RUA 3
                lcdCommand(L_L1);
                lcdString("BR-040  ");
                lcdCommand(L_L2);
                lcdString("L: 100 ");
                limite = 100;
                multa = 300 + ((pot * 30) / 100);
                break;
            case 'R': //imprime RUA 4
                lcdCommand(L_L1);
                lcdString("MG-301  ");
                lcdCommand(L_L2);
                lcdString("L: 110 ");
                limite = 110;
                multa = 400 + ((pot * 40) / 100);
                break;
            default:
                break;
        }//end switch
    }
}//end le teclado

serialcom() {
    op = serialRead();
    if (op != 0) {
        switch (op) {
            serialSend(op);
            case 'U': //imprime RUA 1
                lcdCommand(L_L1);
                lcdString("RUA XV  ");
                lcdCommand(L_L2);
                lcdString("L: 060");
                limite = 60;
                multa = 100 + ((pot * 10) / 100);
                break;
            case 'L': //imprime RUA 2
                lcdCommand(L_L1);
                lcdString("AVENIDA  ");
                lcdCommand(L_L2);
                lcdString("L: 080");
                limite = 80;
                multa = 200 + ((pot * 20) / 100);
                break;
            case 'D': //imprime RUA 3
                lcdCommand(L_L1);
                lcdString("BR-040  ");
                lcdCommand(L_L2);
                lcdString("L: 100");
                limite = 100;
                multa = 300 + ((pot * 30) / 100);
                break;
            case 'R': //imprime RUA 4
                lcdCommand(L_L1);
                lcdString("MG-301  ");
                lcdCommand(L_L2);
                lcdString("L: 110");
                limite = 110;
                multa = 400 + ((pot * 40) / 100);
                break;
            default:
                break;
        }//end switch
    }//end if
    pot = adcRead(0);
    serialSend('V');
    serialSend(':');
    serialSend(' ');
    serialSend((((200 * pot) / 1023) + 20) / 100 % 10 + 48);
    serialSend((((200 * pot) / 1023) + 20) / 10 % 10 + 48);
    serialSend((((200 * pot) / 1023) + 20) / 1 % 10 + 48);
    serialSend('\n');

}//end serial

void calcula() {

    velocidade = (200 * pot) / 1024 + 20;
    if (velocidade > limite) { //teste de velocidade vs limite
        mostraDisplay();
    } else if (velocidade < limite) {
        apagaDisplay();
    }
}

void main(void) {
    char slot;
    lcdInit(); //inicia perifericos
    kpInit();
    ssdInit();
    timerInit();
    serialInit();
    adcInit();
    lcdString("-Radar- ");
    for (;;) {
        timerReset(6000);
        switch (slot) {
            case 0:
                leTeclado();
                ssdUpdate();
                slot = 1;
                break;
            case 1:
                serialcom();
                slot = 2;
                break;
            case 2:
                levelocidade();
                slot = 3;
                break;
            case 3:
                calcula();
                ssdUpdate();
                slot = 4;
                break;
            default:
                slot = 0;
                break;
        }//end switch

        timerWait();
    }//end for
}//end main


