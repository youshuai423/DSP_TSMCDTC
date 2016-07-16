/******************************************************************************
| includes
|----------------------------------------------------------------------------*/
#include "ysPORT.h"

void InitPORT()
{
	EALLOW;

	//epwm1-12
	GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;   // disable pullup on GPIO0
	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;   // disable pullup on GPIO1
	GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;   // disable pullup on GPIO2
	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;   // disable pullup on GPIO3
	GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;   // disable pullup on GPIO4
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;   // disable pullup on GPIO5
	GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;   // disable pullup on GPIO0
	GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;   // disable pullup on GPIO1
	GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;   // disable pullup on GPIO2
	GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;   // disable pullup on GPIO3
	GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;   // disable pullup on GPIO4
	GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;   // disable pullup on GPIO5

	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // GPIO 初始化为epwm输出
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1; // GPIO 初始化为epwm输出
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;

/*
	GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;  //
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  //
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  //

	GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;  //
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;  //
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  //

	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;  //
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;  //
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  //

	GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;  //
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;  //
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  //

	GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;  //
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;  //
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  //

	GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;  //
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;  //
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  //
*/

	//ad7606
	GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up for GPIO14 (SCITXDB)
	GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up for GPIO15 (SCIRXDB)
	GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3;  // Asynch input GPIO15 (SCIRXDB)
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;   // Configure GPIO14 for SCITXDB operation
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;   // Configure GPIO15 for SCIRXDB operation
	GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;  //
	GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;  //
	GpioCtrlRegs.GPADIR.bit.GPIO14 = 0;  //

	//xd15-0
	GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3;  // XD15
	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3;  // XD14
	GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3;  // XD13
	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 3;  // XD12
	GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 3;  // XD11
	GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;  // XD10
	GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;  // XD19
	GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;  // XD8
	GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;  // XD7
	GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 3;  // XD6
	GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 3;  // XD5
	GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 3;  // XD4
	GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 3;  // XD3
	GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 3;  // XD2
	GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 3;  // XD1
	GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 3;  // XD0

	//xa0-19
	GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 3;  // XA0/XWE1n
	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 3;  // XA1
	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;  // XA2
	GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;  // XA3
	GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 3;  // XA4
	GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 3;  // XA5
	GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 3;  // XA6
	GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 3;  // XA7
	GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 3;  // XA8
	GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 3;  // XA9
	GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 3;  // XA10
	GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 3;  // XA11
	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 3;  // XA12
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 3;  // XA13
	GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 3;  // XA14
	GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 3;  // XA15
	GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 3;  // XA16
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 3;  // XA17
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 3;  // XA18
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 3;  // XA19

	GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0
	GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;  // XZCS7
	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 3;  // XZCS6

	EDIS;
}
