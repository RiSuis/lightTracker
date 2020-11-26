/*
/////////////////// DATOS DEL PROGRAMA ////////////////////

//  TTITULO: Seguidor de luz
//  MICRO:PIC16F15244
//  ESTUDIANTES: Steve Rincón, Julián Salgado
//  Profesor: Harold F MURCIA
//  FECHA: 26 de noviembre de 2020

///////////// CONFIGURACIï¿½N del MCU ////////////////// */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>


#pragma config FEXTOSC = OFF    // External Oscillator Mode Selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINTOSC_1MHZ    // Power-up Default Value for COSC bits->HFINTOSC (1 MHz)
#pragma config CLKOUTEN = OFF    // Clock Out Enable bit->CLKOUT function is disabled; I/O function on RA4
#pragma config VDDAR = HI    // VDD Range Analog Calibration Selection bit->Internal analog systems are calibrated for operation between VDD = 2.3V - 5.5V

// CONFIG2
#pragma config MCLRE = EXTMCLR    // Master Clear Enable bit->If LVP = 0, MCLR pin is MCLR; If LVP = 1, RA3 pin function is MCLR
#pragma config PWRTS = PWRT_OFF    // Power-up Timer Selection bits->PWRT is disabled
#pragma config WDTE = OFF    // WDT Operating Mode bits->WDT disabled; SEN is ignored
#pragma config BOREN = ON    // Brown-out Reset Enable bits->Brown-out Reset Enabled, SBOREN bit is ignored
#pragma config BORV = LO    // Brown-out Reset Voltage Selection bit->Brown-out Reset Voltage (VBOR) set to 1.9V
#pragma config PPS1WAY = ON    // PPSLOCKED One-Way Set Enable bit->The PPSLOCKED bit can be cleared and set only once in software
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a reset

// CONFIG4
#pragma config BBSIZE = BB512    // Boot Block Size Selection bits->512 words boot block size
#pragma config BBEN = OFF    // Boot Block Enable bit->Boot Block is disabled
#pragma config SAFEN = OFF    // SAF Enable bit->SAF is disabled
#pragma config WRTAPP = OFF    // Application Block Write Protection bit->Application Block is not write-protected
#pragma config WRTB = OFF    // Boot Block Write Protection bit->Boot Block is not write-protected
#pragma config WRTC = OFF    // Configuration Registers Write Protection bit->Configuration Registers are not write-protected
#pragma config WRTSAF = OFF    // Storage Area Flash (SAF) Write Protection bit->SAF is not write-protected
#pragma config LVP = ON    // Low Voltage Programming Enable bit->Low Voltage programming enabled. MCLR/Vpp pin function is MCLR. MCLRE Configuration bit is ignored.

// CONFIG5
#pragma config CP = OFF    // User Program Flash Memory Code Protection bit->User Program Flash Memory code protection is disabled



///////////// DEFINICIONES  //////////////////
#define _XTAL_FREQ 1000000




///////////////


///////////// VARIABLES GLOBALES  //////////////////

int resultHigh,resultLow,cicloutil,frecuencia,estado;
uint16_t Lectura,LecA4,LecA5,LecB7; 

///////////// DECLARACIï¿½N DE FUNCIONES Y PROCEDIMIENTOS ///////////////////
void PIN_MANAGER_Initialize(void)
{
    // LATx registers
    LATA = 0xFF;
    LATB = 0x00;
    LATC = 0x00;

    // TRISx registers
    TRISA = 0x10;
    TRISB = 0xF0;
    TRISC = 0xFF;

    // ANSELx registers
    ANSELC = 0xFB;
    ANSELB = 0xF0;
    ANSELA = 0x10;

    // WPUx registers
    WPUB = 0x00;
    WPUA = 0x00;
    WPUC = 0x04;

    // ODx registers
    ODCONA = 0x04;
    ODCONB = 0x00;
    ODCONC = 0x00;

    // SLRCONx registers
    SLRCONA = 0x37;
    SLRCONB = 0xF0;
    SLRCONC = 0xFF;

    // INLVLx registers
    INLVLA = 0x3F;
    INLVLB = 0xF0;
    INLVLC = 0xFF;

// TRISx registers
    TRISA2 = 0;          // Definiendo puerto A2 como salida digital
    TRISB5 = 0;          // Definiendo puerto A2 como salida digital
    TRISA5 = 1;          
    TRISC2 = 1;// Definiendo puerto C2 como entada digital
    TRISC0 = 0;          // Definiendo puerto A2 como salida digital
    TRISC7 = 0;
    ANSELAbits.ANSA4 = 1;// Definiendo entrada como analoga
    ANSELAbits.ANSA5 = 1;// Definiendo entrada como analoga
    ANSELBbits.ANSB7 = 1;
    
    RA2PPS = 0x03;                                                           // RA2->PWM3:PWM3OUT; 
}



void OSCILLATOR_Initialize(void)
{
    OSCEN = 0x00;                                                               // MFOEN disabled; LFOEN disabled; ADOEN disabled; HFOEN disabled;
    OSCFRQ = 0x00;                                                              // HFFRQ0 1_MHz
    OSCTUNE = 0x00;
}

void TMR2_Initialize(int nuevafrec)
{
    T2CLKCON = 0x01;                                                            // T2CS FOSC/4;  
    T2HLT = 0x00;                                                               // T2PSYNC Not Synchronized; T2MODE Software control; T2CKPOL Rising Edge; T2CKSYNC Not Synchronized; 
    T2RST = 0x00; 
    T2PR = nuevafrec;  
    T2TMR = 0x00;
    PIR1bits.TMR2IF = 0;                                                        // Clearing IF flag.
    T2CON = 0b10000000;                                                               // T2CKPS 1:1; T2OUTPS 1:1; TMR2ON on; 
}

 void PWM3_Initialize(void)
 {     
    PWM3CON = 0x90;                                                             // PWM3POL active_low; PWM3EN enabled; 
    PWM3DCH = 0x3E;    
    PWM3DCL = 0x40;   
 }
 
void ADC (void)
{
 // Configure ADC
               
    ADCON0= 0b00010001; //canal RA4 GO=0     ON=1 
    
    ADCON1 = 0b10010000; //FM = 1 justificado a la derecha , CLOCK SELECT Fosc/8, PREF = 00 
}

void leer(void){
      if(resultHigh==0){
          Lectura=resultLow;
      }
      else if(resultHigh==1){
          Lectura=256+resultLow;
      }
      else if(resultHigh==2){ 
          Lectura=512+resultLow;
      }
      else{
          Lectura=768+resultLow;
      }
}

void Read_adc(void){
     ADCON0= 0b00010001; 
     ADCON0bits.GO = 1; // Start conversion
     while (ADCON0bits.GO); // Wait for conversion done
     resultHigh = ADRESH; // Read result
     resultLow = ADRESL; // Read result
     leer();
     LecA4 = Lectura;
     // Cambiamos a leer el A5
     ADCON0= 0b00010101; 
     ADCON0bits.GO = 1;
     while (ADCON0bits.GO); // Wait for conversion done
     resultHigh = ADRESH; // Read result
     resultLow = ADRESL; // Read result
     leer();
     LecA5 = Lectura;
     // Cambiamos a B7
     ADCON0= 0b00111101; 
     ADCON0bits.GO = 1;
     while (ADCON0bits.GO); // Wait for conversion done
     resultHigh = ADRESH; // Read result
     resultLow = ADRESL; // Read result
     leer();
     LecB7 = Lectura;
}

 
  void PWM3_LoadDutyValue(uint16_t dutyValue)
 {     
     PWM3DCH = (dutyValue & 0x03FC)>>2;                                         // Writing to 8 MSBs of PWM duty cycle in PWMDCH register     
     PWM3DCL = (dutyValue & 0x0003)<<6;                                         // Writing to 2 LSBs of PWM duty cycle in PWMDCL register
 }
  
void calcular(){
    if((LecB7>=LecA4)&&(LecB7>=LecA5)){
        LATBbits.LATB5 = 1;
        LATCbits.LATC0 = 1;
    }
    else if(LecA4<LecA5)
    {   LATBbits.LATB5 = 0;
        LATCbits.LATC0 = 1;
    }
    else{LATBbits.LATB5 = 1;
        LATCbits.LATC0 = 0;
    }
}
/////////////  INICIO DEL PROGRAMA PRINCIPAL //////////////////////////


void main(void)
{
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    TMR2_Initialize(249);
    PWM3_Initialize();
    ADC();
    estado=1;
    while(1){
      Read_adc();
      calcular();
      __delay_ms(60);
      }
}


