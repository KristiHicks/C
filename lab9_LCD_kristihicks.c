/* 
 * File:   newmain.c
 * Author: VenkataRamesh Bontupalli
 *
 * Created on March 12, 2015, 2:23 PM
 */

#include <plib.h>
//#pragma config FPBDIV = DIV_1, FWDTEN = OFF, FNOSC = FRC, POSCMOD = OFF

// Configuration Bit settings
// SYSCLK = 72 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 36 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
//
#pragma config FPLLMUL = MUL_18, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2

#define SYSCLK          (72000000)
#define PBCLK           (SYSCLK/2)

#define DESIRED_BAUDRATE        (9600)      //The desired BaudRate

// calculate actual BAUD generate value.
#define BAUD_VALUE  ((PBCLK/16/DESIRED_BAUDRATE)-1)

// n1 for Button 1 and n2 for Button 2
int n1=0, n2=0;
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
static	char	szBacklightOn[] = { 0x1B,'[','3', 'e'};

void    DelayMs( int delay );
void 	lights_buttons();

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

    // Open UART1 with Receive and Transmitter enable.
    OpenUART1(UART_EN, UART_RX_ENABLE | UART_TX_ENABLE, BAUD_VALUE);
    putsUART1(szBacklightOn);
    putsUART1(szClearScreen);
    putsUART1(szGotoLine1);
    putsUART1("Count: ");
    putsUART1(szGotoLine2);
    putsUART1(" ");
	DelayMs(300000);
	DelayMs(300000);
int d1=0,d2=0,d3=0;
	while ( 1 ) 
   {
// Write your code here
        d3++;
        if(d3==10){
            d3=0;
            d2++;
            if(d2==10){
                d2=0;
                d1++;
                if(d1==10){
                    d1=0;
                }
            }
        }
        DelayMs(300000);
        putsUART1(szClearScreen);
        putsUART1(szGotoLine1);
        putsUART1("Count:");
        putcUART1(d1+48);
        putcUART1(d2+48);
        putcUART1(d3+48);
        putsUART1(szGotoLine2);
        putsUART1("Button Pressed:");
lights_buttons();
}



}

void DelayMs( int delay )
{	
	while ( 0 < delay ) {
		delay--;
	}
}

// Read the Buttons and display the output on LED's and LCD screen
void lights_buttons()
{
	// write your code here
    int i;
    //Set the onboard buttons BTN0,BTN1 as inputs
    mPORTASetPinsDigitalIn(BIT_6|BIT_7);
    //Set the onboard LEDS LD0,LD1,LD2,LD3 as outputs
    TRISBCLR = (1<<10)|(1<<11)|(1<<12)|(1<<13);
	
			n1 = PORTA & (1<<6);
			n2 = PORTA & (1<<7);
if (n1)
	// Show on the screen that BTN1 is pressed 
    putsUART1("1");

if (n2)
	// Show on the screen that BTN2 is pressed 
    putsUART1("2");


// When BTN1 is pressed, LED1 and LED3 have to gleam
if (n1)
{
      PORTBSET = (1<<10);
	  PORTBSET = (1<<12);
}
else 
{

      PORTBCLR = (1<<10);
      PORTBCLR = (1<<12);
}   

// When BTN2 is pressed, LED1 and LED3 have to gleam
if (n2)    
{
                
     PORTBSET = (1<<11);
     PORTBSET = (1<<13);
}
else  //if not pressed
{
   
   	  PORTBCLR = (1<<11);
	  PORTBCLR = (1<<13);

 }
}