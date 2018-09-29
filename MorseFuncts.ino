// Magic Morse algorithm is (c) 2011, 2012, 2013 by M. Ray Burnette 
// M. Ray Burnette AKA: Ray Burne ALL COMMERCIAL RIGHTS RESERVED WORLDWIDE
// Magic Morse other: 
//    http://www.picaxeforum.co.uk/entry.php?30-Notes-behind-Magic-Morse
//    http://www.instructables.com/id/Morse-Code-Magic-An-Algorithm/
//    http://www.instructables.com/id/Orb-Flashes-Temperature-in-Morse-Code/
//    http://www.instructables.com/id/Zero-to-Morse-in-6-hours/

#include "Defines.h"

boolean SignalFlag       = true;
boolean SingleSpace      = true;
//boolean CurrentKeyState  = LOW;
boolean MorseKeyState    = LOW;
boolean PreviousKeyState = LOW;
const byte MaxElement    = 6;    // Prosigns are 6 elements
byte Elements;   // Working var reset to MaxElement during decode
byte MMpoint;    // Magic Morse pointer
byte MMcount;    // Magic Morse pointer
int DitCount;    // diagnostics dits in word counter for average
int DahCount;    // diagnostics dahs in word counter for average
long DitSum;     // diagnostics mS of dits in word
long DahSum;     // diagnostics mS of dahs in word
long DITmS;      // morse dot time length in mS
long DAHmS;      // morse dah time length in mS
long quarterDIT; // DITmS/4
long halfDIT;    // DITmS/2
long halfDAH;    // DAHmS/2
long DITDAH;     // DAHmS + DITmS
long DiDiDi;     // 3* DITmS
// long wordSpace;  // defined as 7 * dot time in mS
long wordBreak;  // in Magic Morse, same as Character break = 3*DITmS
long keyDOWN;    // misc. timer
long keyUP;      // misc. timer
long DeBounce;   // misc. timer - mS debounce delay
long TimeStamp;  // misc. timer - millis() real-time reading

// ITU (International Morse Code) decoding: The MM[] matrix is decoded in 6-elements to provide for prosigns
// http://upload.wikimedia.org/wikipedia/en/thumb/5/5a/Morse_comparison.svg/350px-Morse_comparison.svg.png
#if 1
char MM[] PROGMEM = "_EISH5ee0TNDB60-"    // 00-0F   0 - 15      e == ERROR
                    "00ARLw0000MGZ700"    // 10-1F  16 - 31      w == WAIT
                    "000UF0000i0KC000"    // 20-2F  32 - 47      i == INVITE
                    "000WP000000O08:0"    // 30-3F  48 - 63
                    "0000Vu0]0000X/00"    // 40-4F  64 - 79      u == UNDERSTOOD  ] == End Of Work
                    "00000+0.0000Q000"    // 50-5F  80 - 95
                    "00000!?_0000Y(0)"    // 60-6F  96 - 111     () == Left/Right hand bracket
                    "0000J0000000e900"    // 70-7F 112 - 127
                    "000004uc) M.R=BU"    // 80-8F 128 - 143     u == UNDERSTOOD
                    "RNETTE\"00000000,"    // 90-9F 144 - 159     ' @ [150] should be "
                    "00>0000000000[;!"    // A0-AF 160 - 175     [ == Starting Signal
                    "000000@000000000"    // B0-BF 176 - 191
                    "0000030000000000"    // C0-CF 192 - 207
                    "0000000000000000"    // D0-DF 208 - 223
                    "0000020000000000"    // E0-EF 224 - 239
                    "000001'000000000";   // F0-FF 240 - 255
#else
char MM[] PROGMEM = "_EISH5ee0TNDB6-0"    // 00-0F   0 - 15      e == ERROR
                    "00ARLw0000MGZ700"    // 10-1F  16 - 31      w == WAIT
                    "000UF0000i0KC000"    // 20-2F  32 - 47      i == INVITE
                    "000WP000000O0800"    // 30-3F  48 - 63
                    "0000Vu]00000X/00"    // 40-4F  64 - 79      u == UNDERSTOOD  ] == End Of Work
                    "00000+.00000Q000"    // 50-5F  80 - 95
                    "00000!?000_0Y()0"    // 60-6F  96 - 111     () == Left/Right hand bracket
                    "00)0J0000000e900"    // 70-7F 112 - 127
                    "000004(c) M.R=BU"    // 80-8F 128 - 143
                    "RNETTE'0000000,0"    // 90-9F 144 - 159     ' @ [150] should be "
                    "00,0000000000[;0"    // A0-AF 160 - 175     [ == Starting Signal
                    "000000@000000000"    // B0-BF 176 - 191
                    "0000030000000000"    // C0-CF 192 - 207
                    "0000000000000000"    // D0-DF 208 - 223
                    "0000020000000000"    // E0-EF 224 - 239
                    "000001'000000000";   // F0-FF 240 - 255
#endif

#if 1
boolean ReadMorseKeyState()
{
    static boolean CurrentKeyState = LOW;
    static long lastTimeStamp = 0;
    
    TimeStamp = millis();

    // Since debounceDelay is in millis - don't bother checking more often that 1ms
    if (TimeStamp < (lastTimeStamp + 1))
        return CurrentKeyState;
        
    lastTimeStamp = TimeStamp;
    
    // Read state of Morse Code digital input
    MorseKeyState = !digitalRead(morseInPin);  // key down, MorseKeyState = true
    
    // Noise suspression
    if (MorseKeyState != PreviousKeyState) {
        DeBounce = TimeStamp; // reset timer
        return CurrentKeyState;
    }
            
    // Debounce
    if ((TimeStamp - DeBounce) > debounceDelay) {
        CurrentKeyState = MorseKeyState;  // capture current key state
        if (CurrentKeyState) { // key is STABLE DOWN
            tone(toneOutPin, toneHz);
            digitalWrite(PC13, LOW);
            keyDOWN = DeBounce; 
            SignalFlag   = false;
            SingleSpace  = false;
        } else { // key is STABLE UP
            keyUP = DeBounce;
            noTone(toneOutPin);
            digitalWrite(PC13, HIGH);
        }
    }
    return CurrentKeyState;
}
#else
boolean ReadMorseKeyState()
{
    static boolean CurrentKeyState  = LOW;
    static long lastTimeStamp = 0;
    
    TimeStamp = millis();
    if (TimeStamp < (lastTimeStamp + 1))
        return CurrentKeyState;
    lastTimeStamp = TimeStamp;
    
    // Read state of Morse Code digital input
    MorseKeyState = !digitalRead(morseInPin);  // key down, MorseKeyState = true
    
    // Noise suspression
    if (MorseKeyState != PreviousKeyState) {
        DeBounce = TimeStamp; // reset timer
        
    // Debounce
    } else {
        if ((TimeStamp - DeBounce) > debounceDelay) {
            CurrentKeyState = MorseKeyState;  // capture current key state
            if (CurrentKeyState) { // key is STABLE DOWN
                tone(toneOutPin, toneHz);
                keyDOWN = DeBounce; 
                SignalFlag   = false;
                SingleSpace  = false;
            } else { // key is STABLE UP
                keyUP = DeBounce;
                noTone(toneOutPin);
            }
        }
    }
    return CurrentKeyState;
}
#endif


// Decode morse code using Magic Morse algorithm, (c) 2011, 2012, 2013 by M.R. Burnette
// char() value is returned by this function for the decoded Morse elements
int MagicMorse(void)
{
    char temp;
    boolean CurrentKeyState;
    
    CurrentKeyState = ReadMorseKeyState();

    if (!CurrentKeyState) {    // DO NOT decode when Morse Key is DOWN
        long keyUpTime = TimeStamp - keyUP;
        long keyDnTime = keyUP - keyDOWN;
        
        // If the key is UP AND if there are elements AND IF sufficient time has passed since last element...
        // A valid time period for decoding is the Key is UP > 50% of a DIT or the Key was DOWN > 25% DOT time
        if (!SignalFlag && ((Elements > 0) &&  ((keyUpTime > halfDIT) || (keyDnTime > quarterDIT))))   {

            // if signal for less than half a dash, take it as a dot
            if (keyDnTime < halfDAH) {  // DIT? That is, less that 150% of a DIT?
                SignalFlag = true;
                // Magic Morse: increment count for Dot
                ++MMcount;
                --Elements;
                DitSum += (keyUP-keyDOWN);
                ++DitCount;
            } else if (keyDnTime < DITDAH) {  // DAH? That is, less that a DIT + DAH?
                SignalFlag = true;
                // Magic Morse: increment count for Dash
                ++MMcount;
                --Elements;
                DahSum += (keyUP-keyDOWN);
                ++DahCount;
                // Magic Morse Dash weighting based on element position
                switch (MMcount) {
                case 1:  MMpoint |= B00001000;  break;
                case 2:  MMpoint |= B00010000;  break;
                case 3:  MMpoint |= B00100000;  break;
                case 4:  MMpoint |= B01000000;  break;
                case 5:  MMpoint |= B10000000;  break;
                case 6:  MMpoint |= B00000001;  break;
                }
            }
            
        } // end element validity check DOT or DASH, that is a state change has occurred
        
        // RETURN character if 2 x dot time has expired (2/3 of a dash)
        if ((keyUpTime >= (DiDiDi)) && (Elements < MaxElement)) {
            // Magic Morse
            MMpoint += MMcount;
            temp = pgm_read_word(&MM[MMpoint]);
            // Prosign adjustments done here to ease decoding
#if 0
            switch (MMpoint) {
            case 0x3E:  temp = ':';  break;  // colon char(0x3A)
            case 0x66:  if (DahCount > 2) temp = '_';  break;  // underscore char(0x5F)
            case 0x96:  temp = '"';  break;  // double quotes char(0x22)
            case 0xF6:  temp = '\'';  break;  // apostrophe char(0x27)
            }
#endif
            MMpoint = 0;
            MMcount = 0;
            Elements = MaxElement;
            if (temp >= ' ' && temp <= '~') { // Legal Morse character set
                return temp;
            }
        }
        
        // RETURN a word space once and default back to singlespace
        if (SingleSpace == false && (keyUpTime >= wordBreak)) {
            SingleSpace = true; // space written-flag, don't do it again until after next valid element
            MMpoint = 0;
            MMcount= 0;
            Elements = MaxElement;
///            if (Verbose)
///                showtime(3);
            return ' ';
        }
        
    } //end if (!CurrentKeyState) that is, Morse Key is 'down' and mS are being accumulated
    
    // update last input state to current value
    PreviousKeyState = MorseKeyState;
   return 0;
}

