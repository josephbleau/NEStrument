#include <vector>

#include "Pitches.h"
#include "NESController.h"

// Arduino Pins
#define debugLED    13
#define clockLine    2
#define dataLine     4
#define latchLine    7
#define speakerPin   8

int recordingState;

int btn_down;
int btn_up;
int btn_pressed;

int* button_state;
int* last_state;

int last_millis;
int this_millis;

int state_buffer_timer;
int state_counter;
int last_state_count;

std::vector<int*> state_buffer;

void playToneFromState(int* button_state, int speaker_line)
{
  if(button_state[BTN_A] == 1) tone(speaker_line, NOTE_B5);
  else if(button_state[BTN_LEFT] == 1)tone(speaker_line, NOTE_C4);
  else if(button_state[BTN_UP] == 1) tone(speaker_line, NOTE_D4);
  else if(button_state[BTN_RIGHT] == 1) tone(speaker_line, NOTE_E4);
  else if(button_state[BTN_DOWN] == 1) tone(speaker_line, NOTE_F4);
  else if(button_state[BTN_SELECT] == 1) tone(speaker_line, NOTE_G4);
  else if(button_state[BTN_B] == 1) tone(speaker_line, NOTE_A5);
  else noTone(speaker_line);
}

void setup()
{
  recordingState = 0;
  button_state = 0;
  last_state = 0;
  btn_down = 0;
  btn_up = 0;
  btn_pressed = 0;
  last_millis = 0;
  this_millis = 0;
  state_buffer_timer = 0;
  state_counter = 0;
  last_state_count = 0;
  
  pinMode(debugLED, OUTPUT);
  pinMode(clockLine, OUTPUT);
  pinMode(dataLine, INPUT);
  pinMode(latchLine, OUTPUT);
}

void loop() 
{
  int *delete_state = last_state;
  last_state = button_state;
  button_state = getNESButtonState(latchLine, dataLine, clockLine);

  this_millis = millis();
 
  if(last_state == 0)
    return;
  
  playToneFromState(button_state, speakerPin);
  
  if(last_state[BTN_START] == 0 && button_state[BTN_START] == 1){
    btn_down = 1;
  }
  if(last_state[BTN_START] == 1 && button_state[BTN_START] == 0){
    btn_up = 1;
  }
  
  if(btn_down == 1 && btn_up == 1) {
    btn_down = 0;
    btn_up = 0;
    btn_pressed = 1;
  }
  
  if(recordingState == 0 && btn_pressed == 1){
    recordingState = 1;
    btn_pressed = 0;
  }
  if(recordingState == 1 && btn_pressed == 1){
    recordingState = 0;
    btn_pressed = 0;
  }
  
  if(recordingState == 1) {
    if(this_millis - last_millis > 1000) {
      last_millis = this_millis;
      if(digitalRead(debugLED) == HIGH){
        digitalWrite(debugLED, LOW);
      }
      else if(digitalRead(debugLED) == LOW){
        digitalWrite(debugLED, HIGH);
      }
    }
    
    if(this_millis - state_buffer_timer > 62) {
      // Store state
     state_buffer.push_back(button_state);
      state_counter = state_counter + 1;
      state_buffer_timer = this_millis;
      tone(8, NOTE_C4);
      if(state_counter == 999) {
        recordingState = 0;
        noTone(8);
      }
    }
  }
  
  if(recordingState == 0) {
    digitalWrite(debugLED, LOW);  
    last_state_count = state_counter;
    state_counter = 0;
  }
  
  
  if(delete_state != 0 && recordingState == 0)
    free(delete_state);
}
