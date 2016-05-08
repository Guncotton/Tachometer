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
   
   Timer0 - Allocated to return the needle to zero when engine is turned off.
   
   Timer1 - Allocated to capture mode on CCP1. 4uS tick rate @ 8Mhz.
   
   Timer2 - Allocated to PWM mode CCP2. 4,132Hz frequency
*/

<<<<<<< HEAD
#DEFINE FAN_ON       125
#DEFINE FAN_OFF      150
#DEFINE PWM_ZERO     24
=======
#DEFINE FAN_ON    125 //210 on gauge
#DEFINE FAN_OFF   170
>>>>>>> 5339bf93a1edfc4c17082308e5c822d9ccffc8f6

#include <Tachometer-v2.h>

#USE FAST_IO(ALL)


int16 CCP1_Delta = 0;
int16 CLT_Value = 1023;
int16 RPM = 0;
int1 CCP1_Flag;

<<<<<<< HEAD
=======

>>>>>>> 5339bf93a1edfc4c17082308e5c822d9ccffc8f6
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
/*
Timer0 Interrupt sets the gauge needle to zero
*/
#INT_TIMER0
void Timer0_ISR(void){
   set_pwm2_duty(PWM_ZERO);
}

void main(){

<<<<<<< HEAD
   int16 PWM_Value;
   
   set_tris_a(0b00000001);
   set_tris_b(0x00);          
   set_tris_c(0b11000100); // C6/C7=RS-232. C2=CCP1. C0=Fan Rly.
   
   output_b(0x01);         // LED On.  
   
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);
   
   setup_ccp2(CCP_PWM);
   setup_timer_2(T2_DIV_BY_4, 120, 1);
   set_pwm2_duty(PWM_ZERO);

   setup_ccp1(CCP_CAPTURE_DIV_16);
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
  
   clear_interrupt(INT_TIMER0);
   clear_interrupt(INT_CCP1);
   
   enable_interrupts(INT_TIMER0);   
   enable_interrupts(INT_CCP1);
   enable_interrupts(GLOBAL);             // Enable interrupt servicing.
   
   printf("\033[2J\r\nMain Loop\r\n");    // Clear Hyperterminal screen.
   
   setup_timer_0(T0_INTERNAL | T0_DIV_4); // Used to return tachometer to zero when engine is off.
   set_timer0(0);
   
   delay_ms(1);                           // Delay to prevent spiking fan on during POR.
=======
   int8 PWM_Value = 23;
>>>>>>> 5339bf93a1edfc4c17082308e5c822d9ccffc8f6
   
   MCU_Init();
        
   while (TRUE){
/*   
   Tachometer code.
      1. Calc duty cycle based on time period.
      2. Set duty cycle.
      3. Reset no signal timer.
      4. Clear service flag.
*/
      if (CCP1_Flag)
      {
         PWM_Value = Compute_Duty_Cycle(CCP1_Delta);
         set_pwm2_duty(PWM_Value);   
         set_timer0(0);
         CCP1_Flag = False;
      }    
/*
      Coolant temperature code.
*/
      //TAD delay.
      delay_us(10);
      CLT_Value = read_adc();
      
<<<<<<< HEAD
      if (CLT_Value < FAN_ON) {
         
         if (input_state(PIN_C0) == 0) printf("ON:%Lu\r\n", CLT_Value);
         output_high(PIN_C0);         
      }
      
      if (CLT_Value > FAN_OFF) {
         
         if (input_state(PIN_C0) == 1) printf("OFF:%Lu\r\n", CLT_Value);
               
=======
      //Turn fan on.
      if (CLT_Value < FAN_ON){
         output_high(PIN_C0);
         if (input_state(PIN_C0) == 1) printf("\033[1;37H ON");
      }
      
      //Turn fan off.
      if (CLT_Value > FAN_OFF){
>>>>>>> 5339bf93a1edfc4c17082308e5c822d9ccffc8f6
         output_low(PIN_C0);
         if (input_state(PIN_C0) == 0) printf("\033[1;37HOFF");
      }
<<<<<<< HEAD
=======
      
      //Compute rom to nearest integer.
      RPM = Calc_RPM(CCP1_Delta);
      
      //Update serial display.
      printf("\033[1;6H%Lu \033[1;26H%Lu", RPM, CLT_Value);
>>>>>>> 5339bf93a1edfc4c17082308e5c822d9ccffc8f6
      
      sleep(SLEEP_IDLE);
   }
}
