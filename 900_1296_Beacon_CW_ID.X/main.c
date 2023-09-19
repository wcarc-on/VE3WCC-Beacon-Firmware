/* 
 __      ________ ______          _______ _____ 
 \ \    / /  ____|___ \ \        / / ____/ ____|
  \ \  / /| |__    __) \ \  /\  / / |   | |     
   \ \/ / |  __|  |__ < \ \/  \/ /| |   | |     
    \  /  | |____ ___) | \  /\  / | |___| |____ 
     \/   |______|____/   \/  \/   \_____\_____|

 CW Beacon ID generator. Designed to run on a PIC12F683, and to be compiled in MPLAB X IDE.
 Used on the W7BAS Micro-Beacon board. Replaces the PIC16F688. 
  - Plugs at the top end of the '688 socket.
  - Needs a bodge wire to re-route the '683 GP4 output to pin 7 on the '688 socket.
 Author: Bertrand Zauhar - VE2ZAZ/VA2IW, September 2023

*/

#include <xc.h>
#include <pic12f683.h>              // Compiled for PIC12F683

// Constants
#define _XTAL_FREQ  250000          // Oscillator frequency considered by the compiler and simulator
#define ON 1                        // Clearer than 0 and 1
#define OFF 0                       //      "
#define TTL_CW_OUT GP4              // I/O pin labelling

// Configuration bytes (Fuses)
#pragma config FOSC = INTOSCIO  	// Internal oscillator selected, I/O function on RA4 pin, I/O function on RA5 pin
#pragma config WDTE = OFF       	
#pragma config PWRTE = OFF      	
#pragma config MCLRE = OFF      	
#pragma config CP = OFF         	
#pragma config CPD = OFF        	
#pragma config BOREN = OFF      	
#pragma config IESO = OFF       	
#pragma config FCMEN = OFF      	

// Global Variables 
char          TxStr_pos;          // Used to keep track of the current character position in the string
unsigned int  DitDuration = 70;   // This sets the entire CW sending speed

// Character Morse Code Table
const char* MorseCode[] = 
{
  ".-",     // A
  "-...",   // B
  "-.-.",   // C
  "-..",    // D
  ".",      // E
  "..-.",   // F
  "--.",    // G
  "....",   // H
  "..",     // I
  ".---",   // J
  "-.-",    // K
  ".-..",   // L
  "--",     // M
  "-.",     // N
  "---",    // O
  ".--.",   // P
  "--.-",   // Q
  ".-.",    // R
  "...",    // S
  "-",      // T
  "..-",    // U
  "...-",   // V
  ".--",    // W
  "-..-",   // X
  "-.--",   // Y
  "--.."    // Z
};

// Number Morse Code Table
const char* Numbers[] = 
{
  "-----",   // 0
  ".----",   // 1
  "..---",   // 2
  "...--",   // 3
  "....-",   // 4
  ".....",   // 5
  "-....",   // 6
  "--...",   // 7
  "---..",   // 8
  "----."    // 9
};

// CW ID must only use letters, numbers, "/" and spaces,
const char Tx_String[] = "VVV DE VE3WCC/B FN15XI";         // FN15XI is Diefenbunker's location.


// Timing function based on looping calculated by the compiler
void delay_ms(unsigned int delay) 
{
    for (unsigned int i = 0; i < delay; i++) __delay_ms(1);
}

// This function puts the transmitter in keydown state
void KeyDown()
{
  TTL_CW_OUT = ON;
}

// This function puts the transmitter in idle state
void KeyUp()
{
  TTL_CW_OUT = OFF;
}

// This function sends a Dit or a Dah 
void SendDitDah(char DitDah_data,unsigned int ditdelay) 
{
  KeyDown();                                            // Turn CW Output ON
  if (DitDah_data == '.') delay_ms(ditdelay);           // Dah case
  else if (DitDah_data == '-') delay_ms(ditdelay*3);    // Dit case
  KeyUp();                                              // Turn CW Output OFF
  delay_ms(ditdelay);                                   // Delay before the following dit/dah
}

// This function sends a single character
void SendCharacter(char* Character,unsigned int ditdelay) 
{
  char i = 0;
  while (Character[i] != '\0')                          // Arrays have \0 at their end 
  {
    SendDitDah(Character[i],ditdelay);                  // Send the Dit or Dah
    i++;
  } 
  delay_ms(ditdelay*2);                                 // Delay between every character
}

// This function sends the entire CW ID message
void Send_CW_Mess(unsigned int ditdelay)
{
  char TxCharacter;
  TxStr_pos = 0;
  while (TxStr_pos < sizeof(Tx_String))                       // This loop will go through the entire Tx CW ID string
  {
    TxCharacter = Tx_String[TxStr_pos];
     // Letters
    if (TxCharacter >= 'A' && TxCharacter <= 'Z')              // Treat the character case
    {
       // Call the Character element then send morse code equivalent
      SendCharacter(MorseCode[TxCharacter - 'A'],ditdelay);    // Uses ascii code to locate character
    }
     else if (TxCharacter >= '0' && TxCharacter <= '9')        // Treat the number case
    {
       // Call Number's Element then send morse code equivalent
      SendCharacter(Numbers[TxCharacter - '0'],ditdelay);      // Uses ascii code to locate character
    }
     // Special character cases
    else if (TxCharacter == '/')                               // Forward slash
   
    {
      SendCharacter("-..-.",ditdelay);
    }  
    else if (TxCharacter == ' ')                               // A space, equivalent to a Dah
    {
      delay_ms(ditdelay*3);
    }  
    TxStr_pos++;    
  }
}

// Setup function, run only once
void setup()
{
    // Microcontroller register initialization
    PIR1 =       0;          // Disable interrupts
    IOC =        0b00000000; // Disable I/O pin driven interrupts
    PIE1 =       0b00000000; // Do not allow interrupts
    OPTION_REG = 0b00000111; // Globally active Pull-ups.
    WPU =        0b00101111; // Individual pull-ups activations
    OSCCON =     0b00100001; // Internal oscillator selection. 250 Khz frequency selection
    T1CON =      0b00000100; // TMR1 disabled
    T2CON =      0b01111011; // TMR2 disabled
    CCP1CON =    0b00000000; // No PWM
    ADCON0 =     0b10000000; // No ADC
    ANSEL =      0b00000000; // No analog functions on pins
    CMCON0 =     0b00010111; // No comparator
    CMCON1 =     0b00000000; // No comparator
    VRCON =      0b00100001; // No comparator
    WDTCON =     0b00010100; // Warchdog turned off
    GPIO   =     0b00000000; // Set proper level on pins before enabling the outputs
    TRISIO =     0b11101111; // Assign I/O pins as shown below:
								//	GP5-not used 
								//	GP4-CW Morse output
								//	GP3-Input only, not used 
								//	GP2-not used 
								//	GP1-not used 
								//	GP0-not used 

	delay_ms(250);			 // 250 millisecond stabilisation pause
}

//Actual CW sending function. Will loop in main function
void Send_ID_Cycle()
{
   // Send the CW ID message
  Send_CW_Mess(DitDuration);  
   // Send keydown for 5 seconds
  delay_ms(400);                            // Pause to separate keydown from CW 
  KeyDown();                                // keydown for 5 seconds 
  delay_ms(5000);                           //          "                             
  KeyUp();                                  // keyup
  delay_ms(400);                            // Pause to separate CW from keydown 
}

// Main function. Everyrthing happens here.
void main(void) 
{
  setup();                                  // Called only once because of loop following
  while (1) Send_ID_Cycle();                // Will loop forever
}
