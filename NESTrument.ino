#include "Pitches.h"
#include "NESController.h"

// Arduino Pins
#define debugLED    13
#define clockLine    2
#define dataLine     4
#define latchLine    7
#define speakerLine  8

void controllerButtonPressed(int);
void controllerButtonReleased(int);
void playToneFromState(int*, int);
void playTonesFromReplay();

NESController controller(dataLine, latchLine, clockLine);

void setup()
{
  pinMode(debugLED, OUTPUT);
  pinMode(clockLine, OUTPUT);
  pinMode(dataLine, INPUT);
  pinMode(latchLine, OUTPUT);

  controller.registerListeners(controllerButtonPressed, 
                               controllerButtonReleased);
}

void loop() 
{
  controller.run();
  
  if(controller.isRecording())
  {
    digitalWrite(debugLED, HIGH);
  }
  else
  {
    digitalWrite(debugLED, LOW);
  }
}

void playToneFromState(int button)
{
  if(button == BTN_A) tone(speakerLine, NOTE_B5);
  else if(button == BTN_LEFT) tone(speakerLine, NOTE_C4);
  else if(button == BTN_UP) tone(speakerLine, NOTE_D4);
  else if(button == BTN_RIGHT) tone(speakerLine, NOTE_E4);
  else if(button == BTN_DOWN) tone(speakerLine, NOTE_F4);
  else if(button == BTN_B) tone(speakerLine, NOTE_A5);
  else noTone(speakerLine);
}

void playTonesFromReplay()
{
  int** replay_states = controller.getReplayStates();
  for(int i = 0; i < NESController::RECORD_BUFFER_MAX; i++)
  {
    if(replay_states[i] == 0)
      return;
      
    if(i!=0) delay(replay_states[i][1]);
    playToneFromState(replay_states[i][0]);
  }
}

void controllerButtonReleased(int button)
{
  playToneFromState(-1);
}

void controllerButtonPressed(int button)
{
  if(button == BTN_START)
  {
    controller.toggleRecording();
  }
  
  if(button == BTN_SELECT)
  {
    if(!controller.isRecording())
      playTonesFromReplay();
  }
  
  playToneFromState(button);
}
