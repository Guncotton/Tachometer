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

/*
Function Prototypes
*/

int8 Compute_Duty_Cycle(int16 Period);
void MCU_Init();
int16 Calc_RPM(int16 Period);

/*
      Functions
*/

int8 Compute_Duty_Cycle(int16 Period){

   int8 Duty_Cycle;
   
   // y = 15981(x) + 23; R^2 = 0.9997
   
   Duty_Cycle = 15981 / Period + 23;
   
   return Duty_Cycle;
}

int16 Calc_RPM(int16 input){

   int16 result;
   
   result = 883375 / input;
   
   return result;
}

 void MCU_Init(){

   set_tris_a(0b00000001);
   set_tris_b(0x00);          
   set_tris_c(0b11000100);                               // C6/C7=RS-232. C2=CCP1. C0=Fan Rly.
   
   output_b(0x01);                                       // LED On.  
   
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);
   
   setup_ccp2(CCP_PWM);
   setup_timer_2(T2_DIV_BY_1, 120, 1);
   set_pwm2_duty(24);

   setup_ccp1(CCP_CAPTURE_DIV_16);
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
  
   clear_interrupt(INT_TIMER0);
   clear_interrupt(INT_CCP1);
   
   enable_interrupts(INT_TIMER0);   
   enable_interrupts(INT_CCP1);
   enable_interrupts(GLOBAL);             // Enable interrupt servicing.
   
   setup_timer_0(T0_INTERNAL | T0_DIV_4); // Used to return tachometer to zero when engine is off.
   set_timer0(0);
         
   printf("\033[2J\033[H"); // Clear Hyperterminal screen.
   printf("RPM: 0\t\tCoolant: 0\tFan:OFF");   
   
   delay_ms(1);   // Delay to prevent spiking fan on during POR.
}  
