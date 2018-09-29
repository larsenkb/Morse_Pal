// Utility Functions for Magic Morse

#include "Defines.h"

// This is my ASCII to morse encoding that I worked out for my college senior project.
// Least significant 3 bits is the count of dit/dahs in the morse character. The upper
// 5 bits are the morse character with 1=dit and 0=dah.
// Morse characters with 6 symbols have a bit count of either 6 (110) or 0 (000). Where
// the MSB of the bit count is the final morse symbol
uint8_t A2M[] PROGMEM = {  // ASCII offset is 0x21 (subtract 0x21 from ascii to get to start of table)
        0x00, 0xB6, 0x00, 0x00, 0x00, 0x00,   // !  "  #  $  %  &   // !#$%& not implemented
        0x86, 0x4D, 0x48, 0x64, 0xAD, 0x30,   // '  (  )  *  +  ,
        0x78, 0xA8, 0x6D, 0x05, 0x85, 0xC5,   // -  .  /  0  1  2
        0xE5, 0xF5, 0xFD, 0x7D, 0x3D, 0x1D,   // 3  4  5  6  7  8
        0x0D, 0x1E, 0x56, 0x00, 0x75, 0x00,   // 9  :  ;  <  =  >
        0xCE, 0x96, 0x82, 0x74, 0x54, 0x63,   // ?  @  A  B  C  D   // converts ~ to @
        0x81, 0xD4, 0x23, 0xF4, 0xC2, 0x84,   // E  F  G  H  I  J
        0x43, 0xB4, 0x02, 0x42, 0x03, 0x94,   // K  L  M  N  O  P
        0x24, 0xA3, 0xE3, 0x01, 0xC3, 0xE4,   // Q  R  S  T  U  V
        0x83, 0x64, 0x44, 0x34, 0x4D, 0x6D,   // W  X  Y  Z  (  /   // converts [{ to (  and \| to /
        0x48, 0x00, 0xC8                      // )  ^  _            // converts ]} to )
};

void SendMorseLCD( char temp )
{
    gotoXY(nColumn * 7, nRow); // Nokia LCD function to place character 6 lines of 12 characters in font
    LcdCharacter( temp ); 
    ++nColumn;
    if (nColumn >= 12) {  // lines fill to 12 characters and increase through line 6 then line 1 is cleared
        nColumn = 0;
        nRow = ++nRow % 6;
///        if (nRow < 3)
///            nRow = 3;
        gotoXY(nColumn, nRow);
        LcdString(BlankLine[0]);
        LcdCurrentLine(nRow);
    }
}


void LEDflasher( int PIN )
{
#if EN_DITDAH_LEDS
    int LEDcolor = PIN;
    digitalWrite(LEDcolor, HIGH);
    delay(LED_flashdelay);// a small delay of a few milliseconds is required for eyes to detect blink
    digitalWrite(LED_RED, LOW);  digitalWrite(LED_GREEN, LOW);  // off
#endif
}

#if 1
void showtime(int Which)
{
    char buf[80];
    
    switch (Which) {
    case 1:    // DIT
        Serial << (F(".(")) << (keyUP-keyDOWN) << (F(") "));
        break;
    case 2:    // DAH
        Serial << (F("-(")) << (keyUP-keyDOWN) << (F(") "));
        break;
    case 3:    // Word break
    sprintf(buf, "DItMs: %d  avg: %d   DAHmS: %d  avg: %d   WPM: %d\n\r", DITmS, DitSum / DitCount, DAHmS, DahSum / DahCount, WPM);
    Serial.write(buf);
///        Serial << (F(" \\ ")) << endl;
///        Serial << (F("Dit Avg = ")) << DitSum / DitCount << (F("[")) << DITmS << (F("]"));
///        Serial << (F("  Dah Avg = ")) << DahSum / DahCount << (F("[")) << DAHmS << (F("]")) << endl;
///        DitSum = 0; DahSum = 0; DitCount = 0; DahCount = 0;
        break;
    }
}
#endif

#if 0
void STATUS (void)
{
    if (!digitalRead(VerbosePin)); {
        Serial << (F("DIT: "))            << DITmS      <<(F(" mS  "));
        Serial << (F("DIT range:   >  ")) << quarterDIT << (F(" < ")) << halfDAH   << (F(" mS")) << endl;
        Serial << (F("DAH: "))            << DAHmS      <<(F(" mS  "));
        Serial << (F("DAH range:   >= ")) << halfDAH    << (F(" < ")) << DITDAH    << (F(" mS")) << endl;
        Serial << (F("Char Break:  >= ")) << DiDiDi     << (F(" < ")) << wordBreak << (F(" mS")) << endl;
        Serial << (F("Word Break:  >= ")) << wordBreak  << (F(" mS")) << endl      << endl;
    }
}
#endif

void delayDIT(int nbr)
{
  delay(nbr * DITmS);
}

void DIT(void)
{
    tone(toneOutPin, toneHz); delayDIT(1);  // <dit>
    noTone(toneOutPin); delayDIT(1);        // <quiet>
}

void DAH(void)
{
    tone(toneOutPin,toneHz); delayDIT(3);   // <dah>
    noTone(toneOutPin); delayDIT(1);        // <quiet>
}


char sendChar(byte letter)
{
    uint8_t temp, bCnt;
    char tmpLetter = letter;
      
    if (letter == ' ') { delayDIT(4); return tmpLetter; }
    
    // map ASCII 0x60-0x7F to 0x40-0x5F
    if (letter > '_') letter -= 0x20;
    
    letter -= 0x21; //0x27;   // remove control characters (up through SPACE)

    if (letter < 0 ) { return tmpLetter; }

    temp = pgm_read_word(&A2M[letter]);
    if (temp == 0) { delayDIT(4); return tmpLetter; }

    bCnt = temp & 0x7;
    if (bCnt == 0) bCnt = 6;

    do {
        if (temp & 0x80)
            DIT();
        else
            DAH();
        temp <<= 1;
    } while (--bCnt);

    delayDIT(2);
    tmpLetter = toupper(tmpLetter);
    return tmpLetter;
}


void sendStr( char *s )
{
///    Serial << (F("WPM: ")) << WPM << endl;
///    if (!digitalRead(VerbosePin))
///        STATUS();
///    Serial << (F("Printing PARIS on LCD display:")) << endl;
    while (*s) {
        SendMorseLCD(*s);
        sendChar(*s++);
    }
    delayDIT(2);
}


void setspeed(byte value) // see:http://kf7ekb.com/morse-code-cw/morse-code-spacing/  
{
    WPM        = value; ///kbl needed?
    DITmS      = 1200 / WPM;
///    DAHmS      = (3 * 1200) / WPM;
    DAHmS      = DITmS * 3;
    // character break is 3 counts of quiet where dah is 3 counts of tone
    // wordSpace  = 7 * 1200 / WPM;
    wordBreak  = (DITmS * 7);    // changed from wordSpace*2/3; Key UP time in mS for WORDBREAK (space)
///    wordBreak  *= 6;
///    wordBreak  /= 8;
    Elements   = MaxElement;    // International Morse is 5 characters but ProSigns are 6 characters
    halfDIT    = DITmS/2;       // Minimum mS that Key must be UP (quiet) before MM assignment to dot/dash
    quarterDIT = DITmS/4;       // Minimum accepted value in mS for a DIT element (sloppy)
    halfDAH    = DAHmS/2;       // Maximum accepted value in mS for a DIT element (sloppy)
    DITDAH     = DITmS + DAHmS; // Maximum accepted value in mS for a DAH element (sloppy)
    DiDiDi     = (DITmS * 3);   // Minimum mS that Key must be up to decode a character via MM
///    DiDiDi     *= 7;
///    DiDiDi     /= 8;
}

void adjSpeed(void)
{
    long temp;

    DAHmS      = DahSum / DahCount;
    DITmS      = DAHmS / 3;
    WPM        = 1200 / DITmS;
///    DAHmS      = 3 * DITmS;
///    DAHmS      = DahSum / DahCount;
    // character break is 3 counts of quiet where dah is 3 counts of tone
    // wordSpace  = 7 * 1200 / WPM;
    wordBreak  = DITmS * 7;    // changed from wordSpace*2/3; Key UP time in mS for WORDBREAK (space)
    Elements   = MaxElement;   // International Morse is 5 characters but ProSigns are 6 characters
    halfDIT    = DITmS/2;      // Minimum mS that Key must be UP (quiet) before MM assignment to dot/dash
    quarterDIT = DITmS/4;      // Minimum accepted value in mS for a DIT element (sloppy)
    halfDAH    = DAHmS/2;      // Maximum accepted value in mS for a DIT element (sloppy)
    DITDAH     = DITmS + DAHmS;// Maximum accepted value in mS for a DAH element (sloppy)
    DiDiDi     = DITmS * 3;    // Minimum mS that Key must be up to decode a character via MM

    DitSum = 0;
    DahSum = 0;
    DitCount = 0;
    DahCount = 0;
}

#if 0
int freeRam () {
    extern int __heap_start, *__brkval; 
    int v; 
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif

void setWPM(byte WPM)
{
    WPM = 5; setspeed(WPM);  // initialize to 10 for consistent behavior
//    tone(toneOutPin, 1500);
    tone(toneOutPin, toneHz);
    LcdClear();
    nRow = 1;
    nColumn = 0;
    gotoXY(nColumn * 7, nRow); // Nokia LCD function to place character 6 lines of 12 characters in font
    LcdString(*msg3);
    nRow = 5;
    gotoXY(nColumn * 7, nRow);
    LcdString(*msg4);
    delay(2000);
    noTone(toneOutPin);
    LcdClear();
    nRow = 1;
    gotoXY(nColumn * 7, nRow);
    LcdString(*msg5);
    ++nRow;
    gotoXY(nColumn * 7, nRow);
    LcdString(*msg6);
    nRow = 4;
    // loop until Morse Key is Pressed
    while(digitalRead(morseInPin)) {
        ++WPM;
        if (WPM > 40)
            WPM = 5;
        gotoXY(nColumn, nRow);
        //LcdString(BlankLine[0]);
        LcdString(*msg7);
        dispcountt(WPM);
        LcdString("  ");
        delay(700);
    }  // end while
    setspeed(WPM);
    EEPROM.write(EEaddr, WPM);
    delay(100);
}

