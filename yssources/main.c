/******************************************************************************
| includes
|----------------------------------------------------------------------------*/
#include "main.h"

/******************************************************************************
| local variable definitions
|----------------------------------------------------------------------------*/
Uint16 invprd1 = period / 2, invprd2 = period / 2;
Uint16 Tinv1[3] = {period / 4, period / 4, period / 4};
Uint16 Tinv2[3] = {period / 4, period / 4, period / 4};
double mSample[4];
double Uab, Uca;
//extern double Uac, Uba;
/*double Ua, Ub, Uc;
double ia, ib, ic;*/
Uint16 dutycycle;
Uint16 sector;

/******************************************************************************
@brief  Main
******************************************************************************/
void main()
{
   InitSysCtrl();

   DINT;

   InitPieCtrl();

   IER = 0x0000;
   IFR = 0x0000;

   InitPieVectTable();

   EALLOW;
   PieVectTable.EPWM1_INT = &epwm1_timer_isr;  // ePWM1中断函数入口
   EDIS;

   InitPORT();
   InitPWM();
   InitADC();
   //ADCRST();
	
   IER |= M_INT3;  // enable ePWM CPU_interrupt
   PieCtrlRegs.PIEIER3.bit.INTx1 = 1;  // enable ePWM1 pie_interrupt

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

interrupt void epwm1_timer_isr(void)
{
	int Sa, Sb, Sc;

	/* 第二阶段寄存器设置， 时间 < invprd1 */
	while(EPwm4Regs.ETFLG.bit.INT == 1)
		EPwm4Regs.ETCLR.bit.INT = 1;

	EPwm4Regs.TBPRD = invprd2;
	EPwm5Regs.TBPRD = invprd2;
	EPwm6Regs.TBPRD = invprd2;
	EPwm4Regs.CMPA.half.CMPA = Tinv2[0];
	EPwm5Regs.CMPA.half.CMPA = Tinv2[1];
	EPwm6Regs.CMPA.half.CMPA = Tinv2[2];

	/* 整流部分*/

	//DELAY_US(1);  // 整流开关切换死区

	// ----------------强制输出初始值---------------------
	EPwm4Regs.AQSFRC.bit.OTSFA = 1;
	EPwm5Regs.AQSFRC.bit.OTSFA = 1;
	EPwm6Regs.AQSFRC.bit.OTSFA = 1;

	EPwm1Regs.AQSFRC.bit.OTSFA = 1;
	EPwm1Regs.AQSFRC.bit.OTSFB = 1;
	EPwm2Regs.AQSFRC.bit.OTSFA = 1;
	EPwm2Regs.AQSFRC.bit.OTSFB = 1;
	EPwm3Regs.AQSFRC.bit.OTSFA = 1;
	EPwm3Regs.AQSFRC.bit.OTSFB = 1;

	// Clear INT flag for this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

	// ----------------电压电流采样---------------------
	ParallelRD(mSample, 4);
	Uab = mSample[0] * HallRatioV1;
	Uca = mSample[1] * HallRatioV2;
	iabc.a = mSample[2] * HallRatioC;
	iabc.b = mSample[3] * HallRatioC;
	iabc.c = -iabc.a - iabc.b;
	uabc.a = (Uab - Uca) / 3.0;
	uabc.b = -(Uab * 2 + Uca) / 3.0;
	uabc.c = -uabc.a - uabc.b;

	/* ====扇区判断====*/
	Sa = sign(uabc.a);
	Sb = sign(uabc.b);
	Sc = sign(uabc.c);

	if (Sa == 1 && Sb == 0 && Sc == 0)
		sector = 1;
	else if (Sa == 1 && Sb == 1 && Sc == 0)
		sector = 2;
	else if (Sa == 0 && Sb == 1 && Sc == 0)
		sector = 3;
	else if (Sa == 0 && Sb == 1 && Sc == 1)
		sector = 4;
	else if (Sa == 0 && Sb == 0 && Sc == 1)
		sector = 5;
	else if (Sa == 1 && Sb == 0 && Sc == 1)
		sector = 6;
	else
		sector = 0;

   	switch (sector)
   	{
       case 1:
       {
    	   // 防止两段时间过短
    	   dutycycle = (int)(-uabc.b / uabc.a * period);
    	   if (dutycycle  <= limitclk)
    		   dutycycle = limitclk;
    	   else if (period - dutycycle <= limitclk || period <= dutycycle)
    		   dutycycle = period - limitclk;

    	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_SET;  // AU, BL
    	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
    	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
    	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_SET;
    	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
    	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

    	   EPwm1Regs.CMPA.half.CMPA = period + 1;  // BL CL
    	   EPwm1Regs.CMPB = period + 1;
    	   EPwm2Regs.CMPA.half.CMPA = period + 1;
    	   EPwm2Regs.CMPB = dutycycle;
    	   EPwm3Regs.CMPA.half.CMPA = period + 1;
    	   EPwm3Regs.CMPB = dutycycle + DT;  // 防止两相短路
    	   break;
       }

        case 2:
        {
     	   dutycycle = (int)(-uabc.b / uabc.c * period);
    	   if (dutycycle  <= limitclk)
    		   dutycycle = limitclk;
    	   else if (period - dutycycle <= limitclk || period <= dutycycle)
    		   dutycycle = period - limitclk;

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // BU, CL
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_SET;

     	   EPwm1Regs.CMPA.half.CMPA = dutycycle + DT;  //BU, AU
     	   EPwm1Regs.CMPB = period + 1;
     	   EPwm2Regs.CMPA.half.CMPA = dutycycle;
     	   EPwm2Regs.CMPB = period + 1;
     	   EPwm3Regs.CMPA.half.CMPA = period + 1;
     	   EPwm3Regs.CMPB = period + 1;
     	   break;
        }

        case 3:
        {
     	   dutycycle = (int)(-uabc.c / uabc.b * period);
    	   if (dutycycle  <= limitclk)
    		   dutycycle = limitclk;
    	   else if (period - dutycycle <= limitclk || period <= dutycycle)
    		   dutycycle = period - limitclk;

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // BU, CL
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_SET;

     	   EPwm1Regs.CMPA.half.CMPA = period + 1;  // CL, AL
     	   EPwm1Regs.CMPB = dutycycle + DT;
     	   EPwm2Regs.CMPA.half.CMPA = period + 1;
     	   EPwm2Regs.CMPB = period+1;
     	   EPwm3Regs.CMPA.half.CMPA = period + 1;
     	   EPwm3Regs.CMPB = dutycycle;
     	   break;
        }
        case 4:
        {
     	   dutycycle = (int)(-uabc.c / uabc.a * period);
    	   if (dutycycle  <= limitclk)
    		   dutycycle = limitclk;
    	   else if (period - dutycycle <= limitclk || period <= dutycycle)
    		   dutycycle = period - limitclk;

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // AL, CU
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm1Regs.CMPA.half.CMPA = period + 1;  // CU, BU
     	   EPwm1Regs.CMPB = period + 1;
     	   EPwm2Regs.CMPA.half.CMPA = dutycycle + DT;
     	   EPwm2Regs.CMPB = period + 1;
     	   EPwm3Regs.CMPA.half.CMPA = dutycycle;
     	   EPwm3Regs.CMPB = period + 1;
     	   break;
        }
        case 5:
        {
     	   dutycycle = (int)(-uabc.a / uabc.c * period);
    	   if (dutycycle  <= limitclk)
    		   dutycycle = limitclk;
    	   else if (period - dutycycle <= limitclk || period <= dutycycle)
    		   dutycycle = period - limitclk;

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // AL, CU
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm1Regs.CMPA.half.CMPA = period + 1;  // AL BL
     	   EPwm1Regs.CMPB = dutycycle;
     	   EPwm2Regs.CMPA.half.CMPA = period + 1;
     	   EPwm2Regs.CMPB = dutycycle + DT;
     	   EPwm3Regs.CMPA.half.CMPA = period + 1;
     	   EPwm3Regs.CMPB = period + 1;
     	   break;
        }
        case 6:
        {
     	   dutycycle = (int)(-uabc.a / uabc.b * period);
    	   if (dutycycle  <= limitclk)
    		   dutycycle = limitclk;
    	   else if (period - dutycycle <= limitclk || period <= dutycycle)
    		   dutycycle = period - limitclk;

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_SET;  // AU, BL
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm1Regs.CMPA.half.CMPA = dutycycle;  // AU, CU
     	   EPwm1Regs.CMPB = period + 1;
     	   EPwm2Regs.CMPA.half.CMPA = period + 1;
     	   EPwm2Regs.CMPB = period + 1;
     	   EPwm3Regs.CMPA.half.CMPA = dutycycle + DT;
     	   EPwm3Regs.CMPB = period + 1;
     	   break;
        }
        default:
        {
      	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // AU, BL
      	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
      	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
      	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
      	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
      	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

      	   EPwm1Regs.CMPA.half.CMPA = period + 1;  // AU, CU
      	   EPwm1Regs.CMPB = period + 1;
      	   EPwm2Regs.CMPA.half.CMPA = period + 1;
      	   EPwm2Regs.CMPB = period + 1;
      	   EPwm3Regs.CMPA.half.CMPA =  period + 1;
      	   EPwm3Regs.CMPB = period + 1;
        }
   	}

   	/* 逆变部分 */
   	invprd1 = dutycycle;
   	invprd2 = period - invprd1;

    Ud = 60;
    ualbe_cmd.al = 40 * cos(2*pi*10 * (period_count/10000.0));
    ualbe_cmd.be = 40 * cos(2*pi*10 * (period_count/10000.0) - 0.5*pi);

    period_count++;
    if (period_count > 10000)
    {
      period_count = 0;
    }

   	ualbeSVM(ualbe_cmd.al, ualbe_cmd.be, Ud, invprd1, invprd2, Tinv1, Tinv2);

   	while(EPwm4Regs.ETFLG.bit.INT != 1){}  // 第一阶段结束

	EPwm4Regs.TBPRD = invprd1;
	EPwm5Regs.TBPRD = invprd1;
	EPwm6Regs.TBPRD = invprd1;
   	EPwm4Regs.CMPA.half.CMPA = Tinv1[0];  // AU, CU
   	EPwm5Regs.CMPA.half.CMPA = Tinv1[1];
   	EPwm6Regs.CMPA.half.CMPA = Tinv1[2];

   // Clear INT flag for this timer
   	while(EPwm1Regs.ETFLG.bit.INT == 1)
   		EPwm1Regs.ETCLR.bit.INT = 1;

   // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

int sign(double param)
{
	if (param >= 0)
		return 1;
	else
		return 0;
}
