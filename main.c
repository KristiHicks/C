/************************************************************************/
/*																		*/
/*	main.c	--	Main program module for project							*/
/*																		*/
/************************************************************************/
/*	Author: Yangjie Qi/VenkataRamesh B/Chong Chen/Julian D John			*/
/*	Copyright 2010 University of Dayton									*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	 11/29/10(JulianJ): created	
 * 										*/
/*	 04/02/12(CChen): modefied		
	 04/06/2015  : modified
	 04/07/2017  : modified
									*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <plib.h>
#include "stdtypes.h"
#include "config.h"
#include "spi2.h"
#pragma config FPBDIV = DIV_1, FWDTEN = OFF, FNOSC = FRC, POSCMOD = OFF

#define	stPressed	1
#define	stReleased	0


//PmodCLS instructions
static	char	szClearScreen[] = { 0x1B, '[', 'j'};
static	char	szGotoLine1[]	= { 0x1B,'[', '0', ';', '0', 'H'};
static	char	szGotoLine2[]	= { 0x1B,'[', '1', ';', '0', 'H'};
static	char	szGoToLipo[] 	= { 0x1B,'[', '0', ';', '7', 'H'};
static	char	szGoToImage1[] 	= { 0x1B,'[', '0', ';', '1', '2', 'H'};
static	char	szGoToImage2[] 	= { 0x1B,'[', '1', ';', '1', '2', 'H'};
static	char	szBacklightOn[] = { 0x1B,'[','3', 'e'};
static	char	szCursorOff[] 	= { 0x1B,'[', '0', 'c'};

/* ------------------------------------------------------------ */
/*				Global Variables				                */
/* ------------------------------------------------------------ */
BYTE flag;
// To be used to define the state of the robot
// state=1 when robot in motion, 0 when not in motion
int state=0;



/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */

void	DeviceInit(void);
void	AppInit(void);
void	Wait_ms(WORD ms);
void 	DelayUs(WORD tusDelay );
void	PutStrUart1( const char* sz );
void	PutChUart1( char ch );

/* ------------------------------------------------------------ */
/*				Interrupt Service Routines						*/
/* ------------------------------------------------------------ */
/***	Timer5Handler
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Interrupt service routine for Timer 5 interrupt. Timer 5
**		is used to perform software debouncing of the on-board
*/

void __ISR(_TIMER_5_VECTOR, ipl7) Timer5Handler(void)
{
    int Px,Py;
    int origX=490;
    int origY=517;
    HWORD posX;
        HWORD posY;
        BYTE btns;
    BOOL fLD2;
        char szTemp[20];
        char szTemp2[20];
    int TH=100;
	


		
		// Turn on LD1 of the PmodJSTK and get the position and button status.
		Spi2MaEnableSlave();				// enable the PmodJSTK
		DelayUs(15);
		
		// get lower byte of X position
		posX = Spi2MaTxByte(0x81);	   
		DelayUs(10);
        // get upper byte of X position, write your code here
	    	posX = posX+(Spi2MaTxByte(0x82)<<8);
            DelayUs(10);
		// get lower byte of Y position, write your code here
			posY = Spi2MaTxByte(0x85);
			DelayUs(10);
		// get upper byte of Y position, write your code here
			posY = posY + (Spi2MaTxByte(0x87)<<8);
			DelayUs(10);
		// get button status, write your code here
			btns=Spi2MaTxByte(0x87);
			DelayUs(15);
		
		// disable the PmodJSTK
		Spi2MaDisableSlave();				
		
	 	// Display the PmodJSTK position on line 1 using the PmodCLS, write your code here
		sprintf(szTemp, "X: %d, Y: %d", posX, posY);
        PutStrUart1(szGotoLine1);	
        PutStrUart1(szTemp);				
		
		
	// Px, Py are the new co-ordinates with respect to your rest position 	
    Px=posX-origX;
    Py=posY-origY;
    
	// Write your code to display the status of robot on line 2
	// state depends on the PMOD JSTK buttons
	
	if (btns==2){
 		state =1;
	}
	else if (btns==4){
 			state =0;
	}
	
	// Write your code for all moving combinations
	if (state==1){
if ((Py>TH)&&(Px<TH)&&(Px>-TH)) 
{
             flag = 1;
       PutStrUart1(szGotoLine2);
       sprintf(szTemp2,"straight");
       PutStrUart1(szTemp2);
}


else if ((Px>TH)&&(Py<TH)&&(Py>-TH))   
{
             flag = 3;
       PutStrUart1(szGotoLine2);
       sprintf(szTemp2,"right");
       PutStrUart1(szTemp2);
}

else if ((Py<-TH)&&(Px<TH)&&(Px>-TH))   
{
             flag = 5;
       PutStrUart1(szGotoLine2);
       sprintf(szTemp2,"backup");
       PutStrUart1(szTemp2);
}

else if ((Px<-TH)&&(Py<TH)&&(Py>-TH))   
{
             flag = 7;
       PutStrUart1(szGotoLine2);
       sprintf(szTemp2,"left");
       PutStrUart1(szTemp2);
}

else if  ((Py>TH)&&(Px>TH)) 
{
          flag = 2;
      PutStrUart1(szGotoLine2);
      sprintf(szTemp2, "straight right");
      PutStrUart1(szTemp2);
}
else if ((Py<-TH)&&(Px > TH)) 
{
           flag = 4;
       PutStrUart1(szGotoLine2);
      sprintf(szTemp2, "back right");
        PutStrUart1(szTemp2);
                }

else    if  (Py<-TH && Px<-TH) 
   {   
		flag = 6; PutStrUart1(szGotoLine2);
 sprintf(szTemp2, "back left"); PutStrUart1( szTemp2 );
   }

else if  (Py>TH && Px<-TH)   
{ flag = 8; PutStrUart1(szGotoLine2);
 sprintf(szTemp2, "forward left"); PutStrUart1( szTemp2 );
}
else
{
flag=0;
}
}
    if(state==0){
    flag=0;
        PutStrUart1(szGotoLine2);
		sprintf(szTemp2, "STOP");
        PutStrUart1(szTemp2);
    }
	
	
	
	mT5ClearIntFlag();

    	
}

/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */
/***	main
**
**	Synopsis:
**		st = main()
**
**	Parameters:
**		none
**
**	Return Values:
**		does not return
**
**	Errors:
**		none
**
**	Description:
**		Main program module. Performs basic board initialization
**		and then enters the main program loop.
*/

int main(void) {

	int	X;
	int	Y;
	char *str;
	DeviceInit();
	AppInit();
	flag=0;
    PutStrUart1(szClearScreen);
	while (1)
	{   	
		INTDisableInterrupts();
        //int flag1=1;
		int flag1=flag;
		INTEnableInterrupts();
		//configure OCR to go forward
		//Check the direction
        
       // Write your code here for all other movement combinations
	   if (flag1==1) 
		{
			prtMtrLeftDirClr  = ( 1 << bnMtrLeftDir );   
            prtMtrRightDirSet = ( 1 << bnMtrRightDir );  
			SetDCOC2PWM(0x3FFF);
			SetDCOC3PWM(0x3FFF);
		}
		else if (flag1==3)
            {
               prtMtrLeftDirSet = ( 1 << bnMtrLeftDir ); 
               prtMtrRightDirSet= ( 1 << bnMtrRightDir);
               SetDCOC2PWM(0x0000);
               SetDCOC3PWM(0x3FFF);
            }
		else if (flag1==5)
           {
              prtMtrLeftDirSet  = ( 1 << bnMtrLeftDir ); 
              prtMtrRightDirClr = ( 1 << bnMtrRightDir );
              SetDCOC2PWM(0x3FFF);
              SetDCOC3PWM(0x3FFF);
           }
		else if (flag1==7)
            {
               prtMtrLeftDirClr = ( 1 << bnMtrLeftDir ); 
               prtMtrRightDirClr= ( 1 << bnMtrRightDir );
               SetDCOC2PWM(0x3FFF);
               SetDCOC3PWM(0x0000);
            }
       else if (flag1==2)
		   {
			prtMtrLeftDirClr = ( 1 << bnMtrLeftDir );
			prtMtrRightDirSet= ( 1 << bnMtrRightDir );	
			SetDCOC2PWM(0x3FFF);
			SetDCOC3PWM(0x6FFF);
			}
       else	if (flag1==4)
		   {
			prtMtrLeftDirSet = ( 1 << bnMtrLeftDir );	
			prtMtrRightDirClr= ( 1 << bnMtrRightDir );	
			SetDCOC2PWM(0x3FFF);
			SetDCOC3PWM(0x6FFF);
			}
	  else if (flag1==6)
	          {
            prtMtrLeftDirSet = ( 1 << bnMtrLeftDir );   
            prtMtrRightDirClr= ( 1 << bnMtrRightDir );  
            SetDCOC2PWM(0x6FFF);
            SetDCOC3PWM(0x3FFF);
				}
	 else if (flag1==8)
			   {
            prtMtrLeftDirClr = ( 1 << bnMtrLeftDir );  
            prtMtrRightDirSet= ( 1 << bnMtrRightDir );  
            SetDCOC2PWM(0x6FFF);
            SetDCOC3PWM(0x3FFF);
                }

	 else{   
		SetDCOC2PWM(0x0000);
        SetDCOC3PWM(0x0000);
		 }
		

	}  //end while
}  //end main

/* ------------------------------------------------------------ */
/***	DeviceInit
**
**	Synopsis:
**		DeviceInit()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initializes on chip peripheral devices to the default
**		state.
*/

void DeviceInit() {
    // Configure UART1.
	U1BRG 		= 51;			// BAUD = 9600 bps when FPB = 8 MHz
	U1MODE		= ( 1 << 15 );	// enable UART1
	U1STASET 	= ( 1 << 10);	// enable transmitter

	// Configure SPI2.
	SPI2BRG = 7;	// Fsck = 500 KHz assuming Fpb = 8 MHz
	SPI2CON = ( 1 << SPI2_ON ) | ( 1 << SPI2_SMP ) | ( 1 << SPI2_CKE ) | ( 1 << SPI2_MSTEN );
	Spi2MaConfigSS();
	Spi2MaDisableSlave();
    

    OpenOC2( OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0);
    OpenOC3( OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0);

    OpenTimer2( T2_ON | T2_PS_1_1 | T2_SOURCE_INT, 0xFFFF);
    OpenTimer3( T3_ON | T3_PS_1_1 | T3_SOURCE_INT, 0xFFFF);


	// Configure left motor direction pin and set default direction.
	trisMtrLeftDirClr	= ( 1 << bnMtrLeftDir );
	prtMtrLeftDirClr	= ( 1 << bnMtrLeftDir );	// forward
	
	// Configure right motor diretion pin and set default direction.
	trisMtrRightDirClr	= ( 1 << bnMtrRightDir );	//modify for JD
	prtMtrRightDirSet	= ( 1 << bnMtrRightDir );	// forward


    mPORTASetPinsDigitalIn(BIT_6|BIT_7);


	// Configure Timer 5.
	TMR5	= 0;
	PR5		= 3000; // period match every 100 us
	IPC5SET	= ( 1 << 4 ) | ( 1 << 3 ) | ( 1 << 2 ) | ( 1 << 1 ) | ( 1 << 0 ); // interrupt priority level 7, sub 3
	IFS0CLR = ( 1 << 20);
	IEC0SET	= ( 1 << 20);
	
	// Start timers.
	T5CON = ( 1 << 15 ) | ( 1 << 5 ) | ( 1 << 4 ); // fTimer5 = fPb / 8
    
	// Enable multi-vector interrupts.
	INTEnableSystemMultiVectoredInt();


}

/* ------------------------------------------------------------ */
/***	AppInit
**
**	Synopsis:
**		AppInit()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Performs application specific initialization. Sets devices
**		and global variables for application.
*/

void AppInit() {



}

/* ------------------------------------------------------------ */
/***	Wait_ms
**
**	Synopsis:
**		Wait_ms(WORD)
**
**	Parameters:
**		WORD (range from 0 to 65535)
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Will wait for specified number of milliseconds.  Using a 
**		word variable allows for delays up to 65.535 seconds.  The value
**		in the for statement may need to be altered depending on how your
**		compiler translates this code into AVR assembly.  In assembly, it is
**		possible to generate exact delay values by counting the number of clock
**		cycles your loop takes to execute.  When writing code in C, the compiler
**		interprets the code, and translates it into assembly.  This process is 
**		notoriously inefficient and may vary between different versions of AVR Studio
**		and WinAVR GCC.  A handy method of calibrating the delay loop is to write a 
**		short program that toggles an LED on and off once per second using this 
**		function and using a watch to time how long it is actually taking to
**		complete. 
**
*/

void Wait_ms(WORD delay) {

	WORD i;

	while(delay > 0){

		for( i = 0; i < 375; i ++){
			;;
		}
		delay -= 1;
	}
}
void PutChUart1( char ch )
{
	while ( ! (U1STA & ( 1 << 8 )) ) {
		asm volatile("nop");
	}
	
	U1TXREG = ch;
}

void PutIntUart1( int no )
{
	while ( ! (U1STA & ( 1 << 8 )) ) {
		asm volatile("nop");
	}
	
	U1TXREG = no;
}

void DelayUs( WORD tusDelay )
{
	tusDelay -= 2;
	
    while ( 0 < tusDelay )
    {
	    asm volatile("nop");
        tusDelay--;
    }   // end while
}   // DelayUs

void DelayMs( int tmsDelay )
{	
	while ( 0 < tmsDelay ) {
		tmsDelay--;
	}
}
void	PutStrUart1( const char* sz )
{
int i=0;
while (sz[i]){
PutChUart1(sz[i]);
i++;
}
}
/************************************************************************/