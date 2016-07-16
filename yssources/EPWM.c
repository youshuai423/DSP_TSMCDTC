/******************************************************************************
| includes
|-------------------------------------------------------------------------------------------*/
#include "EPWM.h"

/******************************************************************************
| variables
|--------------------------------------------------------------------------------------------*/
Uint16 invprd1 = period / 2, invprd2 = period / 2;
Uint16 Tinv1[3] = {period / 4, period / 4, period / 4};
Uint16 Tinv2[3] = {period / 4, period / 4, period / 4};
double mSample[4] = {0, 0, 0, 0};  // [0]-Uab, [1]-Uca, [2]-ia, [3]-ib
double Uab = 0, Uca = 0;
//double Uac = 0, Uba = 0;
double Ua = 0, Ub = 0, Uc = 0;
double ia = 0, ib = 0, ic = 0;
unsigned int dutycycle = 0;
unsigned int sector = 1;

/******************************************************************************
| functions
|-------------------------------------------------------------------------------------------*/
void InitPWM() {

	EALLOW;

	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // GPIO 初始化为epwm输出
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;

	EDIS;

/* Grid Side */

	// ----------------EPwm1---------------------
	EPwm1Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	EPwm1Regs.TBCTR = 0;  // 时基计数寄存器置零
	EPwm1Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm1Regs.TBCTL.bit.CLKDIV = prediv;  // 时钟预分频
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

	EPwm1Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
	EPwm1Regs.CMPB = period / 2;
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // TBCTR = 0时装载
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	EPwm1Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
	EPwm1Regs.AQCTLB.bit.CBU = AQ_TOGGLE;
	EPwm1Regs.AQCTLA.bit.PRD = AQ_CLEAR;
	EPwm1Regs.AQCTLB.bit.PRD = AQ_CLEAR;

	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;  // 上下不互补，死区关闭

	EPwm1Regs.ETSEL.bit.INTEN = 1;  // 使能EPwm1中断
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;  // TBCTR = 0触发中断
	EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;  // 每次中断都响应

	   // ----------------EPwm2---------------------
	EPwm2Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	EPwm2Regs.TBCTR = 0;  // 时基计数寄存器置零
	EPwm2Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm2Regs.TBCTL.bit.CLKDIV = prediv;
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;

	EPwm2Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
	EPwm2Regs.CMPB = period / 2;
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	EPwm2Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
	EPwm2Regs.AQCTLB.bit.CBU = AQ_TOGGLE;
	EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;
	EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;

	EPwm2Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;

	// ----------------EPwm3---------------------
	EPwm3Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	EPwm3Regs.TBCTR = 0;  // 时基计数寄存器置零
	EPwm3Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm3Regs.TBCTL.bit.CLKDIV = prediv;
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;

	EPwm3Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
	EPwm3Regs.CMPB = period / 2;
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	EPwm3Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
	EPwm3Regs.AQCTLB.bit.CBU = AQ_TOGGLE;
	EPwm3Regs.AQCTLA.bit.PRD = AQ_CLEAR;
	EPwm3Regs.AQCTLB.bit.PRD = AQ_CLEAR;

	EPwm3Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;

/* Load Side */

	// ----------------EPwm4---------------------
	EPwm4Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	EPwm4Regs.TBCTR = 0;  // 时基计数寄存器置零
	EPwm4Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm4Regs.TBCTL.bit.CLKDIV = prediv;  // 时钟预分频
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm4Regs.TBCTL.bit.PHSEN = TB_ENABLE;

	EPwm4Regs.CMPA.half.CMPA = period / 4; // duty_cycle = 0.5
	EPwm4Regs.CMPB = period / 4;
	EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // TBCTR = 0时装载
	EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	EPwm4Regs.AQCTLA.bit.CAU = AQ_TOGGLE;

	//EPwm4Regs.DBCTL.bit.IN_MODE = DBB_RED_DBA_FED;
	EPwm4Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A不翻转，B翻转
	EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm4Regs.DBRED = 300; // Deadzone
	EPwm4Regs.DBFED = 300;

	EPwm4Regs.ETSEL.bit.INTEN = 1;  // 使能EPwm1中断
	EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;  // TBCTR = 0触发中断
	EPwm4Regs.ETPS.bit.INTPRD = ET_1ST;  // 每次中断都响应

	// ----------------EPwm5---------------------
	EPwm5Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	EPwm5Regs.TBCTR = 0;  // 时基计数寄存器置零
	EPwm5Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm5Regs.TBCTL.bit.CLKDIV = prediv;  // 时钟预分频
	EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm5Regs.TBCTL.bit.PHSEN = TB_ENABLE;

	EPwm5Regs.CMPA.half.CMPA = period / 4; // duty_cycle = 0.5
	EPwm5Regs.CMPB = period / 4;
	EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // TBCTR = 0时装载
	EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;  // TBCTR = 0时装载

	EPwm5Regs.AQCTLA.bit.CAU = AQ_TOGGLE;

	//EPwm5Regs.DBCTL.bit.IN_MODE = DBB_RED_DBA_FED;
	EPwm5Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A不翻转，B翻转
	EPwm5Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm5Regs.DBRED = 300; // Deadzone
	EPwm5Regs.DBFED = 300;

	// ----------------EPwm6---------------------
	EPwm6Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	EPwm6Regs.TBCTR = 0;  // 时基计数寄存器置零
	EPwm6Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm6Regs.TBCTL.bit.CLKDIV = prediv;  // 时钟预分频
	EPwm6Regs.TBCTL.bit.HSPCLKDIV = 0;
	EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm6Regs.TBCTL.bit.PHSEN = TB_ENABLE;

	EPwm6Regs.CMPA.half.CMPA = period / 4; // duty_cycle = 0.5
	EPwm6Regs.CMPB = period / 4;
	EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // TBCTR = 0时装载
	EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;  // TBCTR = 0时装载

	EPwm6Regs.AQCTLA.bit.CAU = AQ_TOGGLE;

	//EPwm6Regs.DBCTL.bit.IN_MODE = DBB_RED_DBA_FED;
	EPwm6Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A不翻转，B翻转
	EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm6Regs.DBRED = 300; // Deadzone
	EPwm6Regs.DBFED = 300;

	// ----------------开始计时---------------------
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;  // 向上计数
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	//EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm1Regs.TBPRD = period;  // 周期设置
	EPwm2Regs.TBPRD = period;
	EPwm3Regs.TBPRD = period;
	//EPwm4Regs.TBPRD = period / 2;
	EPwm5Regs.TBPRD = period / 2;
	EPwm6Regs.TBPRD = period / 2;
	EPwm4Regs.TBPRD = period / 2;

	EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
	EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
	EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
}


interrupt void epwm1_timer_isr(void)
{
	int Sa, Sb, Sc;

	//GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1;

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
	ia = mSample[2] * HallRatioC;
	ib = mSample[3] * HallRatioC;
	ic = -ia - ib;
	Ua = (Uab - Uca) / 3.0;
	Ub = -(Uab * 2 + Uca) / 3.0;
	Uc = -Ua - Ub;

	/* ====扇区判断====*/
	Sa = sign(Ua);
	Sb = sign(Ub);
	Sc = sign(Uc);

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
    	   dutycycle = (int)(-Ub / Ua * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;
    	   else if (period - dutycycle <= usclk)
    		   dutycycle = period - usclk;

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
     	   dutycycle = (int)(-Ub / Uc * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;
    	   else if (period - dutycycle <= usclk)
    		   dutycycle = period - usclk;

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
     	   dutycycle = (int)(-Uc / Ub * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;
    	   else if (period - dutycycle <= usclk)
    		   dutycycle = period - usclk;

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
     	   dutycycle = (int)(-Uc / Ua * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;
    	   else if (period - dutycycle <= usclk)
    		   dutycycle = period - usclk;

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
     	   dutycycle = (int)(-Ua / Uc * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;
    	   else if (period - dutycycle <= usclk)
    		   dutycycle = period - usclk;

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
     	   dutycycle = (int)(-Ua / Ub * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;
    	   else if (period - dutycycle <= usclk)
    		   dutycycle = period - usclk;

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
