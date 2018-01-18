/*
      TODO
      Add beeper
      fr24
      i2c display128x64
      fade led constant from 1>64 onkeypress 255 short
*/

// POWER CONSUMPTION start: 10:40

#include <Keypad.h>
#include <SPI.h>
#include <NRFLite.h>

const static uint8_t RADIO_ID = 1;
const static uint8_t DESTINATION_RADIO_ID = 0;
const static uint8_t PIN_RADIO_CE = A11;
const static uint8_t PIN_RADIO_CSN = 53;
const static uint8_t PIN_RADIO_IRQ = 2;

NRFLite radio;


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

char keysPetscii[ROWS][COLS] = {  // should implement PETSCII 
  { 0, 0, 0, 0, 0, 0, 0, 0 },     // 1 BS CTRL RS SPC C= Q 2 
  { 0, 0, 0, 0, 0, 0, 0, 0 },     // 3 W A LSHFT Z S E 4 
  { 0, 0, 0, 0, 0, 0, 0, 0 },     // 5 R D X C F T 6 
  { 0, 0, 0, 0, 0, 0, 0, 0 },     // 7 Y G V B H U 8 
  { 0, 0, 0, 0, 0, 0, 0, 0 },     // 9 I J N M K O 0 
  { 166, 0, 0, 0, 0, 0, 0, 0 },     // + P L , . : @ - 
  { 0, 0, 0, 0, 0, 0, 0, 0 },     // £ * ; / RSHFT = Pi Home 
  { 0, 0, 0, 0, 0, 0, 0, 0 },     // Del Return LR UD F1 F3 F5 F7 
};

byte rowPins[ROWS] = {A15, A14, A13, A12, 49, 48, 47, 46 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = {45, 44, 43, 42, 41, 40, 39, 38 }; //connect to the column pinouts of the keypad
// RESTORE key = 37&36

byte dsub_0[8] = {35, 34, 33, 32, A0, 31, 30, A1}; // UP / DOWN / LEFT / RIGHT / PADDLE-Y / FIRE / +5V / ?GROUND? / PADDLE-X
byte dsub_1[8] = {29, 28, 27, 26, A2, 25, 24, A3};

Keypad kpd = Keypad( makeKeymap(keyShim), rowPins, colPins, ROWS, COLS );

byte rfStack[255] = {};
byte rfReadPointer = 0;
byte rfWritePointer = 0;
byte rfAck = 1; // last task ack // 0 = did not send or failed(resend readPointerStack) // 1 = last byte was acknowledged  

void rfSend(byte data){
  if(rfAck==1 && rfReadPointer==rfWritePointer){  // send next/this byte
    rfStack[rfWritePointer] = data; 
  }
  else {// send last byte and save this byte in stack
    
  }
  byte nxtByteToSend = rfStack[rfReadPointer];
  radio.startSend(DESTINATION_RADIO_ID, &nxtByteToSend, sizeof(nxtByteToSend));
}

void radioInterrupt(){
    // txOk = the radio successfully // txFail = the radio failed to transmit // rxReady = the radio has received data.
    uint8_t txOk, txFail, rxReady;
    radio.whatHappened(txOk, txFail, rxReady);
    
    if (txOk) { // Check to see if an Ack data packet was provided.
        if (radio.hasAckData()) {
            uint8_t ackData;
            radio.readData(&ackData);
            Serial.print("...Received Ack ");
            Serial.print(ackData);
        }
    }

    if (txFail) {
        Serial.println("...Failed");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    //if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
    if (true) {
        Serial.println("Cannot communicate with radio");
        while (true){
          digitalWrite(LED_BUILTIN, HIGH); delay(50);
          digitalWrite(LED_BUILTIN, LOW); delay(250);
        }
    }
    attachInterrupt(digitalPinToInterrupt(PIN_RADIO_IRQ), radioInterrupt, FALLING);
    
    //Keyboard.begin();
    for(byte i=0; i<8; i++){ pinMode(dsub_0[i], INPUT_PULLUP); }
    for(byte i=0; i<8; i++){ pinMode(dsub_1[i], INPUT_PULLUP); }

    while(false){ // debug joystick
      Serial.print("0:");   Serial.print(digitalRead(dsub_0[0]));
      Serial.print("\t1:"); Serial.print(digitalRead(dsub_0[1]));
      Serial.print("\t2:"); Serial.print(digitalRead(dsub_0[2]));
      Serial.print("\t3:"); Serial.print(digitalRead(dsub_0[3]));
      Serial.print("\t4:"); Serial.print(digitalRead(dsub_0[4])); // paddle Y
      Serial.print("\t5:"); Serial.print(digitalRead(dsub_0[5])); // fire
      Serial.print("\t6:"); Serial.print(digitalRead(dsub_0[6])); // paddle X
      Serial.print("\t7:"); Serial.print(digitalRead(dsub_0[7])); // paddle X

      Serial.print("\t0:"); Serial.print(digitalRead(dsub_1[0]));
      Serial.print("\t1:"); Serial.print(digitalRead(dsub_1[1]));
      Serial.print("\t2:"); Serial.print(digitalRead(dsub_1[2]));
      Serial.print("\t3:"); Serial.print(digitalRead(dsub_1[3]));
      Serial.print("\t4:"); Serial.print(digitalRead(dsub_1[4])); // paddle Y
      Serial.print("\t5:"); Serial.print(digitalRead(dsub_1[5])); // fire
      Serial.print("\t6:"); Serial.print(digitalRead(dsub_1[6])); // paddle X
      Serial.print("\t7:"); Serial.println(digitalRead(dsub_1[7])); // paddle X
      delay(100);
    }
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
                    //Keyboard.press(keys[c][r]);
                       
                    Serial.print(kk);
                    Serial.print(" - ");
                    Serial.println((char) keyy);
                    
                    break;
                  case HOLD:
                    break;
                  case RELEASED:
                    if(kk==11 || kk==52) {
                      holdShift = false;
                    } else {
                      if(holdShift){ keyy = keysShifted[c][r]; }
                      else { keyy = keys[c][r]; }
                    }
                    //Keyboard.release(keys[c][r]);

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
