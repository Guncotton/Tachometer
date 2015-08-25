//------------------------------------------------------------ 
// MACROS 
// This macro allows us to insert a byte into the specified 
// offset of a 16 or 32 bit variable. Example: 
// *BytePtr(temp_long, 2) = 0x55; 
// If temp_long is 0, the above line will make it become: 
// 0x00550000 
#define BytePtr(var, offset) (char *)(&var + offset) 

// The Timer1 interrupt increments an 8-bit variable which 
// extends the timer to 24 bits. We need this so we can 
// avoid having to switch the Timer pre-scaler between "low" 
// and "high" rpm ranges. 

#int_timer1 
void timer1_isr(void) 
{ 
   gc_timer1_extension++; 
} 

//------------------------------------------------------ 
// When we get a CCP interrupt, read the CCP register and 
// save it in a global variable. 

#int_ccp1 
void ccp1_isr(void) 
{ 
   char timer_ext_copy; 
   int32 current_ccp; 
   static int32 old_ccp = 0; 

   // Set flag to indicate that we did a capture. 
   gc_capture_flag = TRUE; 

   current_ccp = (int32)CCPR1_REG; // Read the current CCP 

   // Get local copy of the timer ext. 
   timer_ext_copy = gc_timer1_extension; 

   // Check if a Timer1 interrupt is pending. If so, check if 
   // the CCP capture occurred before or after the Timer rolled 
   // over. We can tell if it occurred after it rolled over, if 
   // the CCP's MSB is zero. ie., if the CCP is somewhere between 
   // 0x0000 and 0x00FF. 
   // We know that we can just check if the MSB = 0x00, because 
   // it takes about 30 us to get into this ISR. (Using a 4 Mhz 
   // crystal on a 16F628). The timer increments at 1 us per 
   // count, so 0xFF = 255 us. 
   // Actually, to be safer, I'll give it 2 MSB counts, which is 
   // 511 us. That way, if I lengthen any of the other ISR's, 
   // we'll still be able to detect the roll-over OK. 
   // If the timer did roll over after we got a CCP interrupt, 
   // then we need to increment the timer extension byte, that we 
   // save. We have to do that because the CCP interrupt has 
   // priority, and so it executes before the Timer isr can 
   // execute and increment the extension. 
   // (Designing the code with the priority switched doesn't help. 
   // You still have the same type of problem. With CCP first, 
   // the fix is easier). 


   // Was CCP captured after Timer1 wrapped ? 
   // If so, increment the copy of the timer ext. 
   if(TMR1IF_BIT) 
   { 
      if(*BytePtr(current_ccp, 1) < 2) 
      {
         timer_ext_copy++;
      }
   // Since we know a timer interrupt is pending, let's just 
   // handle it here and now. That saves a little load off 
   // the processor. 
      gc_timer1_extension++; // Increment the real timer extension 
      
      TMR1IF_BIT = 0; // Then clear the Timer1 interrupt
   } 
   
   // Insert the timer extension into the proper place in the 
   // 32-bit CCP value. 
   // ie., Insert it into location "EE" as follows: 0x00EEnnnn 
   // (nnnn = the CCP). 
      *BytePtr(current_ccp, 2) = timer_ext_copy; 
   
   // Because we're using unsigned math, we don't have to worry 
   // if the current value is less than the old. The result is 
   // always the absolute value of the difference. The only way 
   // there could be a problem is if the new CCP value had rolled 
   // over twice. But with a 24-bit value, and a Timer 
   // pre-scalar of 1, that's 16.7 seconds. That's way beyond any 
   // practical value. 
   
   // Edited on Jan. 2, 2004: There was a bug in this routine, 
   // because I was promoting a 24-bit value to a 32-bit data type, 
   // but the upper byte was always left = 0. This caused a problem 
   // with the 32-bit subtraction when the 24-bit value rolled over past 0. 
   // Kenny spotted this error and provided a fix in a PM to me. 
   // I have commented out the original line, and inserted his fix, below. 
   //g32_ccp_delta = current_ccp - old_ccp; 
      
      g32_ccp_delta = (current_ccp > old_ccp) ? current_ccp - old_ccp : current_ccp + (0x1000000 - old_ccp);
   
      // Save the current ccp value for next time. 
      old_ccp = current_ccp; 
   
}
