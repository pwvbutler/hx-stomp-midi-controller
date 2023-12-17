#include <MIDI.h>

// number of footswitches
const int NUM_SWITCHES = 5;

// pin numbers connected to each footswitch
const int FOOTSWITCH_1_PIN = 9;
const int FOOTSWITCH_2_PIN = 10;
const int FOOTSWITCH_3_PIN = 11;
const int FOOTSWITCH_4_PIN = 12;
const int FOOTSWITCH_5_PIN = 13;

const int footswitches[] = {
  FOOTSWITCH_1_PIN, 
  FOOTSWITCH_2_PIN,
  FOOTSWITCH_3_PIN,
  FOOTSWITCH_4_PIN,
  FOOTSWITCH_5_PIN,
};

// index of each footswitch
const int FOOTSWITCH_1 = 0;
const int FOOTSWITCH_2 = 1;
const int FOOTSWITCH_3 = 2;
const int FOOTSWITCH_4 = 3;
const int FOOTSWITCH_5 = 4;

// footswitch states
int currentSwitchStates[NUM_SWITCHES];
int prevSwitchStates[NUM_SWITCHES];
static bool footswitchStates[NUM_SWITCHES];

// constants
const int SWITCH_PRESSED = LOW;
const int SWITCH_NOT_PRESSED = HIGH;
const int SWITCH_ON = true;
const int SWITCH_OFF = false;

// long press variables
const long LONG_PRESS_TIME = 1000;
unsigned long pressedTime;
bool overpage = false;
bool playing = false;
bool recording = false;
bool loopStored = false;

MIDI_CREATE_DEFAULT_INSTANCE();


void setup() {

  for (int i = 0; i < NUM_SWITCHES; i++) 
  {
    pinMode(footswitches[i], INPUT_PULLUP);
    currentSwitchStates[i] = SWITCH_NOT_PRESSED; 
    prevSwitchStates[i] = SWITCH_NOT_PRESSED;
    footswitchStates[i] = SWITCH_OFF;
  }

  //Serial.begin(9600);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}


void loop() {

  int pressedSwitch = readSwitchStates();

  if (pressedSwitch != -1) 
  { 
    if (recording) {
      MIDI.sendControlChange(61,127,1); // LOOP play
      playing = true;
      recording = false;
      loopStored = true;
      delay(50);
      return;
    }

    if (!overpage) {
      switch (pressedSwitch) {
        case FOOTSWITCH_1:
          MIDI.sendControlChange(49,0,1); // FS1
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
    
        case FOOTSWITCH_2:
          MIDI.sendControlChange(50,0,1); // FS2
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
    
        case FOOTSWITCH_3:
          MIDI.sendControlChange(51,0,1); // FS3
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
    
       case FOOTSWITCH_4:
          MIDI.sendControlChange(52,0,1); // FS4
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
    
       case FOOTSWITCH_5:
          MIDI.sendControlChange(53,0,1); // FS5
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
      }
    } else {
      switch (pressedSwitch) {
        case FOOTSWITCH_1:
          MIDI.sendControlChange(49,0,1); // FS1
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
    
        case FOOTSWITCH_2:
          MIDI.sendControlChange(50,0,1); // FS2
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
    
        case FOOTSWITCH_3:
          MIDI.sendControlChange(63,127,1); // LOOP undo/redo
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
    
       case FOOTSWITCH_4:
          if (!playing && loopStored) {
            MIDI.sendControlChange(61,127,1); // LOOP play
            playing = true;
          } else {
            MIDI.sendControlChange(61,0,1); // LOOP stop
            playing = false;
          }
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
    
       case FOOTSWITCH_5:
          if (!playing) {
            MIDI.sendControlChange(60,127,1); // LOOP record
            recording = true;
          } else {
            MIDI.sendControlChange(60,0,1); // LOOP overdub
          }
          footswitchStates[pressedSwitch] = !footswitchStates[pressedSwitch];
          break;
      }
    }
  }

  delay(50); 
}


int readSwitchStates() {
  
  for (int i = 0; i < NUM_SWITCHES; i++) 
  {
    currentSwitchStates[i] = digitalRead(footswitches[i]);

    if ( currentSwitchStates[i] != prevSwitchStates[i] && currentSwitchStates[i] == SWITCH_PRESSED) 
    {
      if (i != FOOTSWITCH_5) 
      {
        prevSwitchStates[i] = currentSwitchStates[i];
        return i;
      } 
      else 
      {
        pressedTime = millis();
        
        while (currentSwitchStates[i] == SWITCH_PRESSED) {
          delay(50);
          currentSwitchStates[i] = digitalRead(footswitches[i]);
        }
        
        prevSwitchStates[i] = currentSwitchStates[i];
        
        if (millis() - pressedTime > LONG_PRESS_TIME) {
          changePage();
          return -1;
        } else {
          return i;
        }
      }
    }
  
    prevSwitchStates[i] = currentSwitchStates[i];
  
  }

  return -1;
}


void changePage() {
  overpage = !overpage;
  footswitchStates[FOOTSWITCH_3] = SWITCH_OFF;
  footswitchStates[FOOTSWITCH_4] = SWITCH_OFF;
  footswitchStates[FOOTSWITCH_5] = SWITCH_OFF;
  playing = false;
  recording = false;
  loopStored = false;
}
