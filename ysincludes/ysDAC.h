/******************************************************************************
| includes
|----------------------------------------------------------------------------*/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define DA_ADD0				((Uint16 *)0x1B0000)						//输出地址0
#define DA_ADD1				((Uint16 *)0x1B0001)						//输出地址1
#define DA_ADD2				((Uint16 *)0x1B0002)						//输出地址2
#define DA_ADD3				((Uint16 *)0x1B0003)			 			//输出地址3

/******************************************************************************
| defines
|----------------------------------------------------------------------------*/

/******************************************************************************
| variables
|----------------------------------------------------------------------------*/

/******************************************************************************
| local functions prototypes
|----------------------------------------------------------------------------*/

/******************************************************************************
| exported functions
|----------------------------------------------------------------------------*/
extern void DACout(Uint16 index, double data);
