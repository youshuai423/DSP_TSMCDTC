/******************************************************************************
| includes
|----------------------------------------------------------------------------*/
#include "main.h"
#include "math.h"

/******************************************************************************
| local variable definitions
|----------------------------------------------------------------------------*/
double mSample[4] = {0, 0, 0, 0};
double Uab = 0, Uca = 0;
Uint16 index = 0;
Uint16 sector = 0;
Uint16 switchstate[3] = {0, 0, 0};
int Hlamda = 0;
int HTe = 0;
double lastnerr = 0;

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
   PieVectTable.EPWM4_INT = &epwm1_timer_isr;  // ePWM1中断函数入口
   EDIS;

   InitPORT();
   GpioDataRegs.GPASET.bit.GPIO0 = 1;
   GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
   GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
   GpioDataRegs.GPASET.bit.GPIO3 = 1;
   GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
   GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
   InitPWM();
   InitADC();
   //ADCRST();
	
   IER |= M_INT3;  // enable ePWM CPU_interrupt
   PieCtrlRegs.PIEIER3.bit.INTx4 = 1;  // enable ePWM1 pie_interrupt

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
    EPwm4Regs.ETCLR.bit.INT = 1;

    /* current sampling and voltage calculation */
	ParallelRD(mSample, 4);
	Uab = mSample[0] * HallRatioV1;
	Uca = mSample[1] * HallRatioV2;
	iabc.a = mSample[2] * HallRatioC;
	iabc.b = mSample[3] * HallRatioC;
	iabc.c = -iabc.a - iabc.b;
	uabc.a = (Uab - Uca) / 3.0;
	uabc.b = -(Uab * 2 + Uca) / 3.0;
	uabc.c = -uabc.a - uabc.b;

	/* speed calculation */
	wr = wrCal_M();

	/* 3s/2r coordinate transform */
	S3toS2(&uabc, &ualbe);
	S3toS2(&iabc, &ialbe);

	/* stator flux calculation */
	lamdasalbeCal(ualbe, ialbe, &lamdasalbe);

	/* torque calculation */
	torqueCal(lamdasalbe, ialbe, &Te);

	/* sector calculation */
	sector = sectorCal(lamdasalbe);

	/* Te command value calculation */
	PImodule(Tecmd_Kp, Tecmd_Ki, ncmd - n, &lastnerr, Tecmd_Uplimit, Tecmd_Downlimit, &Tecmd);

	/* hysteresis */
	Hlamda = hysteresis(lamdascmd - sqrt(lamdasalbe.al*lamdasalbe.al + lamdasalbe.be*lamdasalbe.be), 0.01);
	HTe = hysteresis(Tecmd - Te, 0.1);

	/* table look-up */
	index = switchtable[0][sector];

	/* force out */
	if (vector[index][0] == 0)
		EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
	else
		EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_SET;

	if (vector[index][1] == 0)
		EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
	else
		EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_SET;

	if (vector[index][2] == 0)
		EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
	else
		EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_SET;

	EPwm4Regs.AQSFRC.bit.OTSFA = 1;
	EPwm5Regs.AQSFRC.bit.OTSFA = 1;
	EPwm6Regs.AQSFRC.bit.OTSFA = 1;

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
