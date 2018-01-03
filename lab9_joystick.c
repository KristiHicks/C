/************************************************************************/
/*										                        		*/
/*	main.c	-- Using PmodJSTK and PmodCLS with Cerebot 32MX4            */
/*											                        	*/
/*											                        	*/
/************************************************************************/
/*	Author: 	Chong Chen/MTA	
Modified : VenkataRamesh Bontupalli									            	*/
/*						            		*/
/************************************************************************/
/*  Module Description: 						                		*/
/*											                        	*/
/*	This module demonstrates how to communicate with the UART serial  	*/
/*	LCD interface, with the CEREBOT32MX board.   						*/
/*																		*/
/*	Required Hardware:													*/
/*		1. Cerebot 32MX4 Rev C/Rev D									*/
/*		2. PmodCLS														*/
/*													                 	*/
/*                                                                      */
/*  Physical Hardware Configuration:                                    */
/*		1. Connect PmodJSTK to the top row of header JB (the header     */
/*		   with SPI2 on it) of the Cerebot 32MX4                        */
/*		2. Connect header J2 of PmodCLS to the top row of header JE of  */
/*		   Cerebot 32MX4												*/
/*		3. Configure PmodCLS UART for 9600 baud rate					*/
/*				a. short MD0 (e.g. place jumper) of JP2 on PmodCLS      */
/*				b. open MD1 (e.g. remove jumper) of JP2 on PmodCLS      */
/*				c. short MD2 (e.g. place jumper) of JP2 on PmodCLS		*/
/*	Application Functionality:                                          */
/*	Example code for Lab10 of ECE314L									*/
/************************************************************************/
/*  Revision History:						                			*/
/*											                        	*/
/*	04/28/2009 (MTA): created											*/
/*	03/25/2012 (C Chen): modified			                			*/
/*											                        	*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions	            	    */
/* ------------------------------------------------------------ */

#include <plib.h>
#include "stdtypes.h"
#include "spi2.h"
#pragma config FPBDIV = DIV_1, FWDTEN = OFF, FNOSC = FRC, POSCMOD = OFF


/* ------------------------------------------------------------ */
/*				Local Type Definitions			                */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Global Variables				                */
/* ------------------------------------------------------------ */

//PmodCLS instructions
static	char	szClearScreen[] = { 0x1B, '[', 'j'};
static	char	szGotoLine1[]	= { 0x1B,'[', '0', ';', '0', 'H'};
static	char	szGotoLine2[]	= { 0x1B,'[', '1', ';', '0', 'H'};
static	char	szGoToLipo[] 	= { 0x1B,'[', '0', ';', '7', 'H'};
static	char	szGoToImage1[] 	= { 0x1B,'[', '0', ';', '1', '2', 'H'};
static	char	szGoToImage2[] 	= { 0x1B,'[', '1', ';', '1', '2', 'H'};
static	char	szBacklightOn[] = { 0x1B,'[','3', 'e'};
static	char	szCursorOff[] 	= { 0x1B,'[', '0', 'c'};
int n1,n2;
/* ------------------------------------------------------------ */
/*				Local Variables				                    */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Forward Declarations			                */
/* ------------------------------------------------------------ */

void	DeviceInit( void );
void	PutChUart1( char ch );
void	PutStrUart1( const char* sz );
void    DelayMs( int tmsDelay );
void 	DelayUs( WORD tusDelay );
void    PutIntUart1( int no);
int     ReadBtnPorts(void);
void 	lights_buttons();

/* ------------------------------------------------------------ */
/*				Interrupt Service Routines	            	    */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Macros											*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Procedure Definitions			                */
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

int main( void )
{
	int Px,Py, state;
    int origX=490;
    int origY=517;
    HWORD posX;
	HWORD posY;
	BYTE btns;
	BOOL fLD2;
	char szTemp[20];
	char szTemp2[20];
	

	DeviceInit();
	PutStrUart1(szClearScreen);

	while ( 1 ) {
		
		// Turn on LD1 of the PmodJSTK and get the position and button status.
		Spi2MaEnableSlave();				// enable the PmodJSTK
		DelayUs(15);
		posX = Spi2MaTxByte(0x83);		// get lower byte of X position
		DelayUs(10);
		// get upper byte of X position, write your code here
		posX = posX+(Spi2MaTxByte(0x81)<<8);
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
		Spi2MaDisableSlave();				// disable the PmodJSTK
		
		// Display the PmodJSTK position and button status using the PmodCLS, 
		//write your code here
        PutStrUart1(szClearScreen);
		sprintf(szTemp, "X: %d, Y: %d", posX, posY);
        PutStrUart1(szGotoLine1);
       
        PutStrUart1(szTemp);
        Px=posX-origX;
        Py=posY-origY;
        
        
        PutStrUart1(szGotoLine2);
        //PutStrUart1("Button: ");
        sprintf(szTemp2, "Button: %d ", btns/2);
        PutStrUart1(szTemp2);
        
		mPORTASetPinsDigitalIn(BIT_6|BIT_7);
        n1 = PORTA & (1<<6);
			n2 = PORTA & (1<<7);
        if(n1){
            
           // sprintf(btns);
        }
        else if(n2){
            PutStrUart1("0");
        }
            DelayMs(30000);
        
	}

}  

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

void DeviceInit( void )
{
	// Configure UART1.
	U1BRG 		= 51;			// BAUD = 9600 bps when FPB = 8 MHz
	U1MODE		= ( 1 << 15 );	// enable UART1
	U1STASET 	= ( 1 << 10);	// enable transmitter

	// Configure SPI2.
	SPI2BRG = 7;	// Fsck = 500 KHz assuming Fpb = 8 MHz
	SPI2CON = ( 1 << SPI2_ON ) | ( 1 << SPI2_SMP ) | ( 1 << SPI2_CKE ) | ( 1 << SPI2_MSTEN );
	Spi2MaConfigSS();
	Spi2MaDisableSlave();
    
	
} 
void PutStrUart1(const char* str)
{
int i=0;
while(str[i]!='\0')
{
PutChUart1(str[i]);
i++;
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


void lights_buttons()
{
	int i;
    //Set the PMOD lights_buttons BTN0,BTN1,BTN2,BTN3 as inputs
    mPORTASetPinsDigitalIn(BIT_6|BIT_7);
    //Set the onboard LEDS LD0,LD1,LD2,LD3 as outputs
    TRISBCLR = (1<<10)|(1<<11)|(1<<12)|(1<<13);
	
		
            //Read BTN0
	        n1 = PORTA & (1<<6);
            //Read BTN1
            n2 = PORTA & (1<<7);
   
			//If BTN0 is pressed
 			if(n1)
            {
                //Set LEDs LD1
			    PORTBSET = (1<<10)|(1<<11);
            }
            else  //if not pressed
 			{
                //Reset LEDs LD1
				PORTBCLR = (1<<10)|(1<<11);
    		}        
            //If BTN1 is pressed
			if(n2)
 			{
                //Set LEDs LD2
			    PORTBSET = (1<<12)|(1<<13);
    		}        
			else
			{
				//Reset LEDs LD2
				PORTBCLR = (1<<12)|(1<<13);
   			}         	


}