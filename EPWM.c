/******************************************************************************
| includes
|-------------------------------------------------------------------------------------------*/
#include "EPWM.h"

/******************************************************************************
| variables
|--------------------------------------------------------------------------------------------*/
double mSample[4] = {0, 0, 0, 0};  // [0]-Uab, [1]-Uca, [2]-ia, [3]-ib
//double Uab = 0, Uca = 0;
double Uac = 0, Uba = 0;
double Ua = 0, Ub = 0, Uc = 0;
double ia = 0, ib = 0, ic = 0;
unsigned int dutycycle = 0;
unsigned int sector = 1;

/******************************************************************************
| functions
|-------------------------------------------------------------------------------------------*/
void ePWMInit() {

	   EALLOW;

	   GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1; // GPIO 初始化为epwm输出
	   GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;
	   GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;
	   GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;
	   GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
	   GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;
	   //GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;  // IO输出测试周期
	  //GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;

	   EDIS;

	   // ----------------EPwm4---------------------
	   EPwm4Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	   EPwm4Regs.TBCTR = 0;  // 时基计数寄存器置零
	   EPwm4Regs.TBCTL.bit.PHSDIR = TB_UP;
	   EPwm4Regs.TBCTL.bit.CLKDIV = prediv;  // 时钟预分频
	   EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0;
	   EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

	   EPwm4Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
	   EPwm4Regs.CMPB = period / 2;
	   EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	   EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	   EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // TBCTR = 0时装载
	   EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	   EPwm4Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
	   EPwm4Regs.AQCTLB.bit.CBU = AQ_TOGGLE;
	   EPwm4Regs.AQCTLA.bit.PRD = AQ_CLEAR;
	   EPwm4Regs.AQCTLB.bit.PRD = AQ_CLEAR;

	   EPwm4Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;  // 上下不互补，死区关闭

	   EPwm4Regs.ETSEL.bit.INTEN = 1;  // 使能ePWM4中断
	   EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;  // TBCTR = 0触发中断
	   EPwm4Regs.ETPS.bit.INTPRD = ET_1ST;  // 每次中断都响应

	   // ----------------EPwm5---------------------
	   EPwm5Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	   EPwm5Regs.TBCTR = 0;  // 时基计数寄存器置零
	   EPwm5Regs.TBCTL.bit.PHSDIR = TB_UP;
	   EPwm5Regs.TBCTL.bit.CLKDIV = prediv;
	   EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0;
	   EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

	   EPwm5Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
	   EPwm5Regs.CMPB = period / 2;
	   EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	   EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	   EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	   EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	   EPwm5Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
	   EPwm5Regs.AQCTLB.bit.CBU = AQ_TOGGLE;
	   EPwm5Regs.AQCTLA.bit.PRD = AQ_CLEAR;
	   EPwm5Regs.AQCTLB.bit.PRD = AQ_CLEAR;

	   EPwm5Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;

	   // ----------------EPwm6---------------------
	   EPwm6Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
	   EPwm6Regs.TBCTR = 0;  // 时基计数寄存器置零
	   EPwm6Regs.TBCTL.bit.PHSDIR = TB_UP;
	   EPwm6Regs.TBCTL.bit.CLKDIV = prediv;
	   EPwm6Regs.TBCTL.bit.HSPCLKDIV = 0;
	   EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

	   EPwm6Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
	   EPwm6Regs.CMPB = period / 2
			   ;
	   EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	   EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	   EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	   EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	   EPwm6Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
	   EPwm6Regs.AQCTLB.bit.CBU = AQ_TOGGLE;
	   EPwm6Regs.AQCTLA.bit.PRD = AQ_CLEAR;
	   EPwm6Regs.AQCTLB.bit.PRD = AQ_CLEAR;

	   EPwm6Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;

	   // ----------------开始计时---------------------
	   EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;  // 向上计数
	   EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	   EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	   EPwm4Regs.TBPRD = period;  // 周期设置
	   EPwm5Regs.TBPRD = period;
	   EPwm6Regs.TBPRD = period;
}


interrupt void epwm4_timer_isr(void)
{
	int Sa, Sb, Sc;

	//DELAY_US(1);  // 整流开关切换死区

	// ----------------强制输出初始值---------------------
	EPwm4Regs.AQSFRC.bit.OTSFA = 1;
	EPwm4Regs.AQSFRC.bit.OTSFB = 1;
	EPwm5Regs.AQSFRC.bit.OTSFA = 1;
	EPwm5Regs.AQSFRC.bit.OTSFB = 1;
	EPwm6Regs.AQSFRC.bit.OTSFA = 1;
	EPwm6Regs.AQSFRC.bit.OTSFB = 1;

	// Clear INT flag for this timer
	EPwm4Regs.ETCLR.bit.INT = 1;

	// ----------------电压电流采样---------------------
	ADCRDOneSpl(mSample);
	//Uab = mSample[0] * HallRatioV1;
	//Uca = mSample[1] * HallRatioV2;
	Uac = mSample[0] * HallRatioV1;
	Uba = mSample[1] * HallRatioV2;
	ia = mSample[2] * HallRatioC;
	ib = mSample[3] * HallRatioC;
	ic = -ia - ib;
	//Ua = (Uab - Uca) / 3.0;
	//Ub = -(Uab * 2 + Uca) / 3.0;
	Ua = (Uac - Uba) / 3.0;
	Ub = (Uac + 2 * Uba) / 3.0;
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
    	   dutycycle = (int)(-Ub / Ua * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;

    	   EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_SET;  // AU, BL
    	   EPwm4Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
    	   EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
    	   EPwm5Regs.AQSFRC.bit.ACTSFB = AQ_SET;
    	   EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
    	   EPwm6Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

    	   EPwm4Regs.CMPA.half.CMPA = period + 1;  // BL CL
    	   EPwm4Regs.CMPB = period + 1;
    	   EPwm5Regs.CMPA.half.CMPA = period + 1;
    	   EPwm5Regs.CMPB = dutycycle;
    	   EPwm6Regs.CMPA.half.CMPA = period + 1;
    	   EPwm6Regs.CMPB = dutycycle + DT;  // 防止两相短路
    	   break;
       }

        case 2:
        {
     	   dutycycle = (int)(-Ub / Uc * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;

     	   EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // BU, CL
     	   EPwm4Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm5Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm6Regs.AQSFRC.bit.ACTSFB = AQ_SET;

     	   EPwm4Regs.CMPA.half.CMPA = dutycycle + DT;  //BU, AU
     	   EPwm4Regs.CMPB = period + 1;
     	   EPwm5Regs.CMPA.half.CMPA = dutycycle;
     	   EPwm5Regs.CMPB = period + 1;
     	   EPwm6Regs.CMPA.half.CMPA = period + 1;
     	   EPwm6Regs.CMPB = period + 1;
     	   break;
        }

        case 3:
        {
     	   dutycycle = (int)(-Uc / Ub * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;

     	   EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // BU, CL
     	   EPwm4Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm5Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm6Regs.AQSFRC.bit.ACTSFB = AQ_SET;

     	   EPwm4Regs.CMPA.half.CMPA = period + 1;  // CL, AL
     	   EPwm4Regs.CMPB = dutycycle + DT;
     	   EPwm5Regs.CMPA.half.CMPA = period + 1;
     	   EPwm5Regs.CMPB = period+1;
     	   EPwm6Regs.CMPA.half.CMPA = period + 1;
     	   EPwm6Regs.CMPB = dutycycle;
     	   break;
        }
        case 4:
        {
     	   dutycycle = (int)(-Uc / Ua * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;

     	   EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // AL, CU
     	   EPwm4Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm5Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm6Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm4Regs.CMPA.half.CMPA = period + 1;  // CU, BU
     	   EPwm4Regs.CMPB = period + 1;
     	   EPwm5Regs.CMPA.half.CMPA = dutycycle + DT;
     	   EPwm5Regs.CMPB = period + 1;
     	   EPwm6Regs.CMPA.half.CMPA = dutycycle;
     	   EPwm6Regs.CMPB = period + 1;
     	   break;
        }
        case 5:
        {
     	   dutycycle = (int)(-Ua / Uc * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;

     	   EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // AL, CU
     	   EPwm4Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm5Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm6Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm4Regs.CMPA.half.CMPA = period + 1;  // AL BL
     	   EPwm4Regs.CMPB = dutycycle;
     	   EPwm5Regs.CMPA.half.CMPA = period + 1;
     	   EPwm5Regs.CMPB = dutycycle + DT;
     	   EPwm6Regs.CMPA.half.CMPA = period + 1;
     	   EPwm6Regs.CMPB = period + 1;
     	   break;
        }
        case 6:
        {
     	   dutycycle = (int)(-Ua / Ub * period);
    	   if (dutycycle  <= usclk)
    		   dutycycle = usclk;

     	   EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_SET;  // AU, BL
     	   EPwm4Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm5Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm6Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm4Regs.CMPA.half.CMPA = dutycycle;  // AU, CU
     	   EPwm4Regs.CMPB = period + 1;
     	   EPwm5Regs.CMPA.half.CMPA = period + 1;
     	   EPwm5Regs.CMPB = period + 1;
     	   EPwm6Regs.CMPA.half.CMPA = dutycycle + DT;
     	   EPwm6Regs.CMPB = period + 1;
     	   break;
        }
        default:
        {
      	   EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // AU, BL
      	   EPwm4Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
      	   EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
      	   EPwm5Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
      	   EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
      	   EPwm6Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

      	   EPwm4Regs.CMPA.half.CMPA = period + 1;  // AU, CU
      	   EPwm4Regs.CMPB = period + 1;
      	   EPwm5Regs.CMPA.half.CMPA = period + 1;
      	   EPwm5Regs.CMPB = period + 1;
      	   EPwm6Regs.CMPA.half.CMPA =  period + 1;
      	   EPwm6Regs.CMPB = period + 1;
        }

   	}

/*	   EPwm4Regs.CMPA.half.CMPA = period + 1;  // AL BL
	   EPwm4Regs.CMPB =  period + 1;
	   EPwm5Regs.CMPA.half.CMPA = period + 1;
	   EPwm5Regs.CMPB =  period + 1;
	   EPwm6Regs.CMPA.half.CMPA = period + 1;
	   EPwm6Regs.CMPB = period + 1;
	   EPwm4Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;  // AU, BL
	   EPwm4Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
	   EPwm5Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
	   EPwm5Regs.AQSFRC.bit.ACTSFB = AQ_SET;
	   EPwm6Regs.AQSFRC.bit.ACTSFA = AQ_SET;
	   EPwm6Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;*/

   // Clear INT flag for this timer
    EPwm4Regs.ETCLR.bit.INT = 1;

   // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

double roundn(double input)
{
	double temp;
	temp = input * digit;
	temp = floor(temp);
	temp = temp / digit;
	return temp;
}

int sign(double param)
{
	if (param >= 0)
		return 1;
	else
		return 0;
}
