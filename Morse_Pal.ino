/*
  MAGIC MORSE (c) 2011, 2012, 2013 by Mickey R. Burnette, AKA Ray Burne
  This implementation specific to Arduino Mini-Pro 328 running at 3.3V
  and used with a NOKIA 5110 Craphic LCD.
  Built under Arduino 1.0.5
  Binary sketch size: 11,108 bytes (of a 30,720 byte maximum) MiniPro 5V profile
  Version 8, 20130515 FreeMem() 1086
  Modified: 7Mar2018 to run on BluePill
*/

#include <EEPROM.h>
#include <Streaming.h>
#include "Defines.h"                // Nokia 5110 LCD pin usage as controlled by "ScrnFuncts.ino"


// constants
const int  BAUD            = 9600;  // any standard serial value: 300 - 115200
const int  EEaddr          = 0;     // EEPROM address for storing WPM
const int  toneHz          = 748;   // 748 = 746.3 Hz on freq counter (750 = 757.6Hz)
const int  LED_flashdelay  = 2;     // mS ... small dealy to allow eye to see LED
const int  MaxConsoleCount = 80;    // Serial output characters before linefeed+cr
const long debounceDelay   = 5;    // the debounce time. Keep well below dotTime
// Arduino 328P pins (not physical)
const int  VarResPin       = PA0;    // select the input pin for the potentiometer
const byte morseInPin      = PB15; //PC14;    // Used for Morse Key (other end to Gnd)
const byte VerbosePin      = PB9;    // Echos from within MagicMorse() decode state of dit/dah
const byte forcePARISpin   = PB10;   // When momentary LOW, forces PARIS to be output on LCD/Serial
const int  toneOutPin      = PA10;   // PWM output for simulated 750Hz (F#1/Gb1 = 746. Hz)
const int  LED_RED         = PB7;    // Red   LED indicates Dah
const int  LED_GREEN       = PB8;    // Green LED indicates Dit
// global prog variables
byte WPM;                           // set by POT on pin#A0
byte nRow;                          // line count      (0-5 for NOKIA LCD)
byte nColumn;                       // character count (0-11 for NOKIA LCD)
byte ConsoleCount;                  // Serial character output counter
byte NOKIAcontrast         = 0xB0;  // LCD initialization contrast
boolean Verbose;                    // state of Pin#9
// Character array pointers
//char* PROGMEM msg0[]       = {"Magic Morse (c) 2013 by M. Burnette All Rights  Reserved    Ver 8.130529"};
char* PROGMEM msg1[]       = {"*Long Dash* With Morse  Key To Begin"};
//char* PROGMEM msg2[]       = {"Currently:  WPM = "};
char* PROGMEM msg2[]       = {"WPM = "};
char* PROGMEM msg3[]       = {"Auto Cycling"};
char* PROGMEM msg4[]       = {"Release Key!"};
char* PROGMEM msg5[]       = {"LONG Dash To"};
char* PROGMEM msg6[]       = {"Save  EEPROM"};
char* PROGMEM msg7[]       = {"WPM = "};
char* PROGMEM BlankLine[]  = {"            "};// Nokia 12 x 6 (84 * 48)


void setup(void)
{ 
    pinMode(morseInPin, INPUT_PULLUP);

    pinMode(VerbosePin, INPUT_PULLUP);

//////    pinMode(toneOutPin, OUTPUT);
///    tone(toneOutPin, toneHz);
///    noTone(toneOutPin);

    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH);
    
#if EN_DITDAH_LEDS
    pinMode(LED_RED, OUTPUT);
    digitalWrite(LED_RED, HIGH);
    pinMode(LED_GREEN, OUTPUT);
    digitalWrite(LED_GREEN, HIGH);
    LEDflasher(LED_GREEN);
    LEDflasher(LED_RED);
#endif

    pinMode(forcePARISpin, INPUT_PULLUP);

    WPM = EEPROM.read(EEaddr);

    Verbose = (!digitalRead(VerbosePin)); // IF pin is LOW, Verbose is TRUE, else FALSE

// spool some basic information out diagnostic ASIC serial port
    Serial.begin(BAUD);
///    Serial << (F("(c) 2011 - 2013 by M. R. Burnette")) << endl;
///    Serial << (F("Free RAM available: ")) ;
///    Serial << freeRam() << endl;
///    Serial << (F("Version 8.130529")) << endl;

    // LCD
    LcdInitialise(); /// LcdClear(); ///LcdString(*msg0);  
///    delay(2000);
    // Set WPM default and write to EEPROM IF Morse Key is closed at this point...
    if (!digitalRead(morseInPin) || WPM == 0 || WPM > 40)
        setWPM(WPM);
    LcdClear();
    delay(500);
    nColumn = 0;
    nRow = 0;
    LcdString(*msg2);
    nRow = 2;
    dispcountt(WPM);
    setspeed(WPM);
///    sendStr("Paris");
    ++nRow;
    nColumn = 0;
    gotoXY(nColumn, nRow);
    LcdCurrentLine(nRow);
}

int nSpaces = 0;

void loop(void)
{
    char temp;
    
    
///    Verbose = (!digitalRead(VerbosePin)); // IF pin is LOW, Verbose is TRUE, else FALSE
    if (!digitalRead(forcePARISpin)) {
        sendStr("Paris");
    }
    
///    ReadMorseKeyState();
    temp = MagicMorse();
    if (temp != 0) {
        Serial.print(temp);  // debug channel - physical ASIC on ATmega328P
///        if (++ConsoleCount > 79 ) {
 ///           Serial << endl;
///            ConsoleCount = 1;
///        }
        SendMorseLCD(temp);

#if 0
        if (temp == ' ') {
            if (++nSpaces > 5) {
                nSpaces = 0;
                showtime(3);
                adjSpeed();
            }
        }
#endif
    }
#if 1
    if (Serial.available() > 0) {
        char incomingByte = Serial.read();
        if (isLowerCase(incomingByte))
            incomingByte = toupper(incomingByte);
        Serial.write(incomingByte);
        if (incomingByte == '\r')
            Serial.write('\n');
        SendMorseLCD(incomingByte);
        sendChar(incomingByte);
    }
#endif
}

