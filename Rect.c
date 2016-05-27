#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "EPWM.h"
#include "ADC.h"
#define pi 3.1415926

void main()
{
   InitSysCtrl();

   DINT;

   InitPieCtrl();

   IER = 0x0000;
   IFR = 0x0000;

   InitPieVectTable();

   EALLOW;
   PieVectTable.EPWM4_INT = &epwm4_timer_isr;  // ePWM4中断函数入口
   EDIS;

   ePWMInit();
   ADCInit();
   ADCRST();
	
   IER |= M_INT3;  // enable ePWM CPU_interrupt
   PieCtrlRegs.PIEIER3.bit.INTx4 = 1;  // enable ePWM4 pie_interrupt

   EINT;   // 总中断 INTM 使能
   ERTM;   // Enable Global realtime interrupt DBGM

   int i;
   for(; ;)
   {
	   asm("          NOP");
	   for(i=1;i<=10;i++)
	   {}
   }

}
