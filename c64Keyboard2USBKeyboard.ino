/* @file MultiKey.ino
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | The latest version, 3.0, of the keypad library supports up to 10
|| | active keys all being pressed at the same time. This sketch is an
|| | example of how you can get multiple key presses from a keypad or
|| | keyboard.
|| #
*/

#include <Keypad.h>
#include "Keyboard.h"

const byte ROWS = 8; //four rows
const byte COLS = 8; //three columns

char keyShim[ROWS][COLS] = {
  { 0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 }, //   1 BS CTRL RS SPC C= Q 2 
  { 8, 9 ,10,11,12,13,14,15 },//  3 W A LSHFT Z S E 4 
  { 16,17,18,19,20,21,22,23 },  // 5 R D X C F T 6 
  { 24,25,26,27,28,29,30,31 },  // 7 Y G V B H U 8 
  { 32,33,34,35,36,37,38,39 },  // 9 I J N M K O 0 
  { 40,41,42,43,44,45,46,47 },  // + P L , . : @ - 
  { 48,49,50,51,52,53,54,55 },//£ * ; / RSHFT = Pi Home 
  { 56,57,58,59,60,61,62,63 }, // Del Return LR UD F1 F3 F5 F7 
};

char keys[ROWS][COLS] = {
  { 49,178,128,177,32,135,113,50 }, //   1 BS CTRL RS SPC C= Q 2 
  { 51,119,97,129,122,115,101,52 },//  3 W A LSHFT Z S E 4 
  { 53,114,100,120,99,102,116,54 },  // 5 R D X C F T 6 
  { 55,121,103,118,98,104,117,56 },  // 7 Y G V B H U 8 
  { 57,105,106,110,109,107,111,48 },  // 9 I J N M K O 0 
  { 43,112,108,44,46,58,34,45 },  // + P L , . : @ - 
  { 35,42,59,47,133,61,211,210 },//£ * ; / RSHFT = Pi Home 
  { 212,176,215,217,194,196,198,200 }, // Del Return LR UD F1 F3 F5 F7 
};

char keysShifted[ROWS][COLS] = {
  { 33,178,128,177,32,135,81,64 }, //   ! BS CTRL RS SPC C= Q " 
  { 92,87,65,129,90,83,69,36 },//  # W A LSHFT Z S E $ 
  { 37,82,68,88,67,70,84,38 },  // % R D X C F T & 
  { 39,89,71,86,66,72,85,40 },  // ' Y G V B H U ( 
  { 41,73,74,78,77,75,79,48 },  // ) I J N M K O 0 
  { 43,80,76,60,62,91,96,95 },  // + P L < > : @ - 
  { 35,42,93,63,133,61,214,210 }, //£ * ; ? RSHFT = Pi Home 
  { 209,176,216,218,195,197,199,201 }, // Del Return LR UD F2 F4 F6 F8 
};

char keys2[ROWS][COLS] = {
  { 49,223,9,177,32,128,113,50 }, //   1 BS CTRL RS SPC C= Q 2 
  { 51,119,97,129,122,115,101,52 },//  3 W A LSHFT Z S E 4 
  { 53,114,100,120,99,102,116,54 },  // 5 R D X C F T 6 
  { 55,121,103,118,98,104,117,56 },  // 7 Y G V B H U 8 
  { 57,105,106,110,109,107,111,48 },  // 9 I J N M K O 0 
  { 45,112,108,44,46,59,91,61 },  // + P L , . : @ - 
  { 209,93,39,47,133,92,212,210 },//£ * ; / RSHFT = Pi Home 
  { 178,176,215,217,194,196,198,200 }, // Del Return LR UD F1 F3 F5 F7 
};

byte colPins[ROWS] = {10,11,12,13,A3,A2,A1,A0 }; //connect to the row pinouts of the keypad
byte rowPins[COLS] = {2, 3, 4, 5, 6, 7, 8, 9 }; //connect to the column pinouts of the keypad

Keypad kpd = Keypad( makeKeymap(keyShim), rowPins, colPins, ROWS, COLS );


void setup() {
    Serial.begin(9600);
    Keyboard.begin();
}

boolean holdShift = false;

void loop() {

    // Fills kpd.key[ ] array with up-to 10 active keys.
    // Returns true if there are ANY active keys.
    if (kpd.getKeys()) {

      
        for (int i=0; i<LIST_MAX; i++) {  // Scan the whole key list.
            if ( kpd.key[i].stateChanged ) {  // Only find keys that have changed state.
                char keyy;
                byte kk = (byte)kpd.key[i].kchar;
                byte c = kk / 8;
                byte r = kk-(c*8);
              
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                  case PRESSED:
                    if(kk==11 || kk==52) {
                      holdShift = true;
                    } else {
                      if(holdShift){ keyy = keysShifted[c][r]; }
                      else { keyy = keys[c][r]; }
                      
                    }
                    Keyboard.press(keys[c][r]);
                       
                    Serial.print(kk);
                    Serial.print(" - ");
                    Serial.println((byte) keyy);
                    
                    break;
                  case HOLD:
                    break;
                  case RELEASED:
                    Keyboard.release(keys[c][r]);
                    break;
                  case IDLE:
                    break;
                }
                
                if(holdShift){ keyy = keysShifted[c][r]; }
                else { keyy = keys[c][r]; }
                
            }
        }
    }
}  // End loop
