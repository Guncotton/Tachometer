#include <Tachometer-v2.h>

#define Alternator
#define Crankshaft
//define Tachometer

int16 CCP1_Delta = 0;
int8 Command;

int1 CCP1_Flag, UART_Flag;

void Initialize_PIC(void);
int16 PRM(int16 Period);
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
   

}

#INT_RDA
void UART_Recieve(void)
{
   Command = getc();
   
   UART_Flag = True;  
}

void main() {
   
   
   int32 Data1;
      
   Initialize_Pic();
   
   printf("\033[2J");                           // Clear Hyperterminal screen.
   
   enable_interrupts(GLOBAL);                   // Enable interrupt servicing.
   
   // CCP2 = pwm RB3     
   // CCP1 = RC2     16 sample average.
   
   while (TRUE)
   {
      if (UART_Flag)
      {
      }
      if (CCP1_Flag)
      {
         Data1 = Frequency(CCP1_Delta);
         
         printf("%5.2w\r", Data2);
         
         CCP1_Flag = False;
      }
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
         
   Frequency = (int32)(500000 / Period);
         
   return Frequency;
}


int8 Compute_Duty_Cycle(int16 Period)
{
   // 16 sample period only.
   int8 Duty_Cycle;
   
   Duty_Cycle = (4769 / Period) + 25;
   
   return Duty_Cycle;
}
