#include <18F2420.h>
#device ICD=TRUE
#device adc=10

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES WDT128                   //Watch Dog Timer uses 1:128 Postscale
#FUSES INTRC                    //Internal RC Osc,
#FUSES NOFCMEN                  //Fail-safe clock monitor disabled
#FUSES NOIESO                   //Internal External Switch Over mode disabled
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOPBADEN                 //PORTB pins are configured as digital I/O on RESET
#FUSES NOLPT1OSC                //Timer1 configured for higher power operation
#FUSES NOMCLR                   //Master Clear pin used for I/O
#FUSES NOSTVREN                 //Stack full/underflow will not cause reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#FUSES CCP2B3                   //CCP2 input/output multiplexed with RB3.
#FUSES DEBUG                    //Debug mode for use with ICD

#use delay(internal=8000000)

#use rs232(baud=57600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)



int8 Compute_Duty_Cycle(int16 Period){

   int8 Duty_Cycle;
   
   // y = 15981(x) + 23; R^2 = 0.9997
   
   Duty_Cycle = 15981 / Period + 23;
   
   return Duty_Cycle;
}
