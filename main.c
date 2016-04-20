#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "math.h"

#define pi 3.1415926
#define digit 1000000  // for roundn
#define period 7500  // represent 10KHz, TBCLK = SYSCLKOUT

void ePWMSetup(void);
double roundn(double);
interrupt void epwm1_timer_isr(void);

void main(void)
{
    InitSysCtrl();
    
    DINT;
    
    InitPieCtrl();
    
    IER = 0X0000;
    IFR = 0X0000;
    
    InitPieVectTable();    
    
    EALLOW;
    PieVectTable.EPWM1_INT = &epwm1_timer_isr;
    EDIS;
    
    ePWMSetup();
    
    IER |= M_INT3;  // enable ePWM1 CPU_interrupt
    PieCtrlRegs.PIEIER3.BIT.INTx1 = 1;  // enable ePWM1 pie_interrupt
    
    EINT;  // enable INTM
    ERTM;  // Enable Global realtime interrupt DBGM
    
    while(1){}
}

void ePWMSetup(void)
{
    EALLOW;
    
    // init GPIO as PWM output for grid side
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;
    
    EDIS;
    
    // ----------------EPwm1---------------------
    EPwm1Regs.TBPRD = period;
    EPwm1Regs.TBPHS.half.TBPHS = 0; 
    EPwm1Regs.TBCTR = 0; 
    EPwm1Regs.TBCTL.bit.PHSDIR = TB_UP;
    EPwm1Regs.TBCTL.bit.CLKDIV = 0;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;

    EPwm1Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
    EPwm1Regs.CMPB = 0;
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
    EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR; 

    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A不翻转，B翻转
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm1Regs.DBRED = 800; // Deadzone
    EPwm1Regs.DBFED = 800;

    EPwm1Regs.ETSEL.bit.INTEN = 1;  // 使能ePWM中断
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;
    EPwm1Regs.ETPS.all = 0x01; // interrupt on first event
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;

    // ----------------EPwm2---------------------
    EPwm2Regs.TBPRD = period;
    EPwm2Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
    EPwm2Regs.TBCTR = 0;  // 时基计数寄存器置零
    EPwm2Regs.TBCTL.bit.PHSDIR = TB_UP;
    EPwm2Regs.TBCTL.bit.CLKDIV = 0;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;

    EPwm2Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
    EPwm2Regs.CMPB = 0;
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

    EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;
    EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A不翻转，B翻转
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm2Regs.DBRED = 800; // Deadzone
    EPwm2Regs.DBFED = 800;

    // ----------------EPwm3---------------------
    EPwm3Regs.TBPRD = period;
    EPwm3Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
    EPwm3Regs.TBCTR = 0;  // 时基计数寄存器置零
    EPwm3Regs.TBCTL.bit.PHSDIR = TB_UP;
    EPwm3Regs.TBCTL.bit.CLKDIV = 0;
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;

    EPwm3Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
    EPwm3Regs.CMPB = 0;
    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

    EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;
    EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;  // A不翻转，B翻转
    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm3Regs.DBRED = 800; // Deadzone
    EPwm3Regs.DBFED = 800;
}