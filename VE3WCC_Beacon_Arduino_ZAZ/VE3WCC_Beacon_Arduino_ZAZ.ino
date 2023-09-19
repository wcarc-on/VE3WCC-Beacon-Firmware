/* 
 __      ________ ______          _______ _____ 
 \ \    / /  ____|___ \ \        / / ____/ ____|
  \ \  / /| |__    __) \ \  /\  / / |   | |     
   \ \/ / |  __|  |__ < \ \/  \/ /| |   | |     
    \  /  | |____ ___) | \  /\  / | |___| |____ 
     \/   |______|____/   \/  \/   \_____\_____|

 CW Beacon ID generator. Designed to run on an Arduino Uno.
 By Bertrand Zauhar, VE2ZAZ, November 2021

*/

// Constants
#define TX_LED 13                 // The LED that indicates keydown. Flashes with the CW keying
#define TTL_CW_OUT 10             // The output pin keying the transmitter(s), active high
#define SPKR 8                    // The pin assigned to a tone sent to a speaker 

// Global Variables 
char          TxStr_pos;          // Used to keep track of the current character position in the string
unsigned int  DitDuration = 80;   // This sets the entire CW sending speed

// Character Morse Code Table
char* MorseCode[] = 
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
char* Numbers[] = 
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

String Tx_String = "VVV DE VE3WCC/B FN15XI";                  // Diefenbunker's location. CW ID must only use letters, numbers, "/" and spaces,
                                                              // otherwise the required "special" characters must be added in the code below
// This function sends the entire CW ID message
void Send_CW_Mess(int ditdelay)
{
  char TxCharacter;
  TxStr_pos = 0;
  while (TxStr_pos < Tx_String.length())                       // This loop will go through the entire Tx CW ID string
  {
    TxCharacter = Tx_String[TxStr_pos];
    Serial.print(TxCharacter);
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
      delay(ditdelay*3);
    }  
    TxStr_pos++;    
    Serial.print(" ");
  }
  Serial.print("\n");
}

// This function sends a single character
void SendCharacter (char* Character,int ditdelay) 
{
  char i = 0;
  while (Character[i] != '\0')                    // Arrays have \0 at their end 
  {
    SendDitDah(Character[i],ditdelay);            // Send the Dit or Dah
    i++;
  } 
  delay(ditdelay*2);                              // Delay between every character
}

// This function sends a Dit or a Dah 
void SendDitDah (char DitDah_data,int ditdelay) 
{
  KeyDown();                                      // Turn LED / CW Output / speaker tone ON
  Serial.print(DitDah_data);
  if (DitDah_data == '.') delay(ditdelay);        // Dah case
  else if (DitDah_data == '-') delay(ditdelay*3); // Dit case
  KeyUp();                                        // Turn LED / CW Output / speaker tone OFF
  delay(ditdelay);                                // Delay before the following dit/dah
}

// This function puts the transmitter(s) in keydown state and turns on the LED and speaker tone
void KeyDown()
{
  digitalWrite(TTL_CW_OUT,HIGH);
  digitalWrite(TX_LED,HIGH);
  tone(SPKR, 800);
}

// This function puts the transmitter(s) in idle state and turns off the LED and speaker tone
void KeyUp()
{
  digitalWrite(TTL_CW_OUT,LOW);
  digitalWrite(TX_LED,LOW);
  noTone(SPKR);
}

// Main Arduino setup function
void setup()
{
  Serial.begin(9600);                     // Open serial port (USB) at 9600 bps
  digitalWrite(TX_LED, LOW);              // Tx LED definition and initial state
  pinMode(TX_LED, OUTPUT);                //             "
  digitalWrite(TTL_CW_OUT,LOW);           // TTL morse signal definition and initial state
  pinMode(TTL_CW_OUT, OUTPUT);            //             "  
  pinMode(SPKR, OUTPUT) ;                 // Definition of speaker pin
}

// Main Arduino loop 
void loop()
{
   // Send the CW ID message
  Send_CW_Mess(DitDuration);  
   // Send keydown for 5 seconds
  delay(400);                             // Pause to separate keydown from CW 
  KeyDown();                              // keydown for 5 seconds 
  Serial.println("Keydown");              //          " 
  delay(5000);                            //          "                             
  KeyUp();                                // keyup
  delay(400);                             // Pause to separate CW from keydown 
}
