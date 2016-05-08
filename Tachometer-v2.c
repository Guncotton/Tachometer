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

#DEFINE FAN_ON    125 //210 on gauge
#DEFINE FAN_OFF   170

#include <Tachometer-v2.h>

#USE FAST_IO(ALL)


int16 CCP1_Delta = 0;
int16 CLT_Value = 1023;
int16 RPM = 0;
int1 CCP1_Flag;


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

   int8 PWM_Value = 23;
   
   MCU_Init();
        
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
      
      //Turn fan on.
      if (CLT_Value < FAN_ON){
         output_high(PIN_C0);
         if (input_state(PIN_C0) == 1) printf("\033[1;37H ON");
      }
      
      //Turn fan off.
      if (CLT_Value > FAN_OFF){
         output_low(PIN_C0);
         if (input_state(PIN_C0) == 0) printf("\033[1;37HOFF");
      }
      
      //Compute rom to nearest integer.
      RPM = Calc_RPM(CCP1_Delta);
      
      //Update serial display.
      printf("\033[1;6H%Lu \033[1;26H%Lu", RPM, CLT_Value);
      
      sleep(SLEEP_IDLE);
   }
}
