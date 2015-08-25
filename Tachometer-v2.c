/*
   Squarewave pulses from Alternator are monitored via CCP1 peripheral. 16 sample average is
   taken and period of pulses is used to derive frequency. Alternator frequency is used to 
   display engine RPM via PWM of an air core meter.
   
   Coolant temperature is input via a thermistor connected to AN0. 

   AN0 = Coolant Thermistor
   RB0 = Green LED
   RB3 = PWM Output to Tach
   RB6 = PGC
   RB7 = PGD
   RC0 = Output to Fan Relay
   RC2 = Input from Alternator
   RC6 = TxD
   RC7 = RxD

*/

#DEFINE FAN_ON    128   //190F
#DEFINE FAN_OFF   150   //180F

#include <Tachometer-v2.h>

#USE FAST_IO(ALL)

int16 CCP1_Delta = 0;
int16 CLT_Value = 1023;

int1 CCP1_Flag;

int8 Compute_Duty_Cycle(int16 Ticks);

#INT_CCP1
void Capture1_ISR(void){

   static int16 CCP1_Previous_Value;
   int16 CCP1_Current_Value;
   
   CCP1_Current_Value = CCP_1;                              // Get capture value.
   
   CCP1_Delta = CCP1_Current_Value - CCP1_Previous_Value;   // Delta T between captures.
   
   CCP1_Previous_Value = CCP1_Current_Value;                // Store current capture value for next delta calc.
   
   CCP1_Delta >>= 4;                                        // Divide time period by 16 for average.
   
   CCP1_Flag = True;                                        // Set new data flag.
}

#INT_TIMER0
void Timer0_ISR(void){
   
   set_pwm2_duty(24);
}

void main(){

   int8 PWM_Value;
   
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
     
   enable_interrupts(GLOBAL);                                  // Enable interrupt servicing.
   
   printf("\033[2J\r\n");                                    // Clear Hyperterminal screen.
   
   printf("Main Loop\r\n");
   
   setup_timer_0(T0_INTERNAL | T0_DIV_4);                      // Used to return tachometer to zero when engine is off.
   
   set_timer0(0);
   
   delay_ms(1);                                          // Delay to prevent spiking fan on during POR.
   
   while (TRUE){
   
      //Tachometer code.
      if (CCP1_Flag)
      {
         PWM_Value = Compute_Duty_Cycle(CCP1_Delta);     // Calculate duty cycle based on frequency.
   
         set_pwm2_duty(PWM_Value);                       // Set new duty cycle output.   
         
         set_timer0(0);                                  // Reset timer.
         
         CCP1_Flag = False;                              // Clear new data flag.
      }
      
      // Coolant temperature code.
      delay_us(10);                                      // TAD delay.
      
      CLT_Value = read_adc();
      
      if (CLT_Value < FAN_ON) {
         
         if (input_state(PIN_C0) == 0) printf("FAN ON ");
         
         output_high(PIN_C0);         
      }
      
      if (CLT_Value > FAN_OFF) {
         
         if (input_state(PIN_C0) == 1) printf("FAN OFF ");
      
         output_low(PIN_C0);
      }
         
      printf("%Lu\r\n", CLT_Value);
      
      sleep(SLEEP_IDLE);
   }
}
