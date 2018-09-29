// defines associated specifically with Nokia 5110 LCD ScrnFuncts
#define PIN_SCE   PB4
#define PIN_RESET PB3
#define PIN_DC    PB5
#define PIN_SDIN  PA2
#define PIN_SCLK  PA1

#define LCD_C     LOW
#define LCD_D     HIGH
#define LCD_X     84
#define LCD_Y     48

#define EN_DITDAH_LEDS    0

/*  ************************************************ Notes & Changes***********************************************
CHANGES:
20130515: Minor change moving functions between modules, created UtilFuncts.ino
          Implemented Verbose Pin#9
          Tightened setspeed() values and added console echo during verbose
20130512: Combined separate IF logic into more complex single statement in MagicMorse decode
20130510: Corrected maximum Elements to scan to correct Prosigns.  Added MaxElement constant
20130508: Implemented LCDCurrentLine() to identify the current LCD active line
          Implemented PARIS()
20130507: Implemented sub-screen to show WPM and allow changes before program starts main loop
          Implemented 10K potentiometer CT on pin A0 to control WPM from 5 to 40
          Moved Morse Key to Analog Pin A3 in prep for reading Analog signal
20130505: Clean-up & module restructioning & migrating character arrays to PROGMEM
          decode() changed to char MagicMorse to return char and checked in main loop
20130501: Major change: Speed Pin D9 is now inverted to create Normally Low behavior

NOTES:
MiniPRO Pins:
#__      Function_________________________________
A0       Center Tap of 10K pot to control WPM at bootup
A1       n/a
A2       n/a
A3       Morse Key (other side of Key to Gnd)
A4       n/a
A5       n/a
A6       n/a
A7       n/a
RESET    Reset
Tx  0    n/a (dedicated Serial Input)
Rx  1    Diag Output RS232-TTL 9600 BAUD Async used for diagnostics: VT100
PIN 2    n/a
PIN 3-7  Nokia Display  (specifics below)
PIN 8    n/a
PIN 9    Activate Verbose Diagnostics by pulling LOW
PIN 10   Force PARIS replay: tone & LCD, not diagnostic
PIN 11   Tone Out --> approx 750Hz @5V to Piezo
PIN 12   Green LED indicate DASH
PIN 13   Red   LED indicate DIT

Nokia 5110 Graphic LCD Pinout:
_______ Mini Pro____   _______ Nokia GLCD___      _____ test board cabling ___
#define PIN_SCE   7    LCD CE ....  Pin 2          Yellow
#define PIN_RESET 6    LCD RST .... Pin 1          Blue
#define PIN_DC    5    LCD Dat/Com. Pin 3  (DC)    Orange
#define PIN_SDIN  4    LCD SPIDat . Pin 4  (DIN)   White
#define PIN_SCLK  3    LCD SPIClk . Pin 5          Brown

//                     LCD Gnd .... Pin 2          Black
//                     LCD Vcc .... Pin 8          Red 3.3V
//                     LCD Vled ... Pin 7          Green (100 Ohms to Gnd)

*/


