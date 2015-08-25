#include <Tachometer.h>

#define Alternator
#define Crankshaft
//define Tachometer

int16 CCP1_Delta = 0;
int16 CCP2_Delta = 0;

int1 CCP1_Flag, CCP2_Flag;

void Initialize_PIC(void);
int16 RPM(int16 Period);
int32 Frequency(int16 Period);
int8 Compute_Duty_Cycle(int16 Ticks);


#INT_CCP1
void Capture1_ISR(void)
{
   static int16 CCP1_Previous_Value;
   int16 CCP1_Current_Value;
   
   CCP1_Current_Value = CCP_1;
   
   CCP1_Delta = CCP1_Current_Value - CCP1_Previous_Value;
   
   CCP1_Previous_Value = CCP1_Current_Value;
   
   CCP1_Flag = True;
}

#INT_CCP2
void Capture2_ISR(void)
{
   static int16 CCP2_Previous_Value;
   int16 CCP2_Current_Value;   
   
   CCP2_Current_Value = CCP_2;      
   
   CCP2_Delta = CCP2_Current_Value - CCP2_Previous_Value;      // Calculate period of signal.
   
   CCP2_Previous_Value = CCP2_Current_Value;      // Store current CCP value for next capture event.
   
   CCP2_Flag = True;    // Set flag to process new data in main loop.
}

void main() {
   
   Initialize_Pic();
   
   printf("\033[2J");                           // Clear Hyperterminal screen.
   
   enable_interrupts(GLOBAL);                   // Enable interrupt servicing.
   
   // CCP2 = RB3     
   // CCP1 = RC2     16 sample average.
   
   while (TRUE){
   
      int16 Data1;
      int32 Data2;
      int8  PWM_Value;
      
      if (CCP2_Flag == True){
         
         Data1 = RPM(CCP2_Delta);
         
         printf("%LU\r", Data1);
         
         CCP2_Flag = False;
      }
      
      if (CCP1_Flag == True){
         
         Data2 = Frequency(CCP1_Delta);
         
         printf("%5.2w\r", Data2);
         
         CCP1_Flag = False;
      }
      
      #ifdef Tachometer
         
         PWM_Value = Compute_Duty_Cycle(CCP2_Delta);
         
         set_pwm1_duty(PWM_Value);
         
      #endif
   }
}


void Initialize_PIC(void)
{
   set_tris_a(0x00);
   
   set_tris_b(0x00);          // 
   
   set_tris_c(0b11000100);          // C6/C7=RS-232. C2=CCP1
   
   setup_ccp2(CCP_PWM);
   
   setup_timer_2(T2_DIV_BY_1, 255, 1);
   
   set_pwm2_duty(26);
   
   setup_ccp1(CCP_CAPTURE_DIV_16);
   
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
   
   clear_interrupt(INT_CCP1);
   
   enable_interrupts(INT_CCP1);
}


int16 RPM(int16 Period)
{
   int16 cycles_per_min;
   
   cycles_per_min = (int16)(37500000 / Period);
         
   return cycles_per_min;
}


int32 Frequency(int16 Period)
{
   int32 Frequency;
   
   Period >>= 4;
         
   Frequency = (int32)(62500000 / Period);
         
   return Frequency;
}


int8 Compute_Duty_Cycle(int16 Ticks)
{
   int8 Duty_Cycle;
   
   Duty_Cycle = (95409 / Ticks) + 25;
   
   return Duty_Cycle;
}

/*

   Duty Cycle = 95,409.38 / Ticks + 25.19
   
   */
