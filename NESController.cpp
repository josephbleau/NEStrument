#include "Arduino.h"
#include "NESController.h"

NESController::NESController(int data, int latch, int clock) 
{
  for(int i = 0; i < 500; i++)
    recording_buffer[i] = 0;
  for(int i = 0; i < 8; i++)
    down_timers[i] = 0;
    
  data_line = data;
  latch_line = latch;
  clock_line = clock;
  last_state = 0;
  current_state = 0;
  recording = 0;
  buffer_position = 0;
  last_note_recorded = millis();
  
  keyPressed = NESControllerDummies::dummyKeyPressed;
  keyReleased = NESControllerDummies::dummyKeyReleased;
}

NESController::~NESController()
{
  for(int i = 0; i < NESController::RECORD_BUFFER_MAX; i++)
  {
    if(recording_buffer[i] != 0)
    {
      delete recording_buffer[i];
      recording_buffer[i] = 0;
    }
  }
  
  if(current_state != 0)
  {
    delete current_state;
    current_state = 0;
  }
  if(last_state != 0)
  {
    delete last_state;
    last_state = 0;
  }
}

void NESController::pulse(int line)
{
  digitalWrite(line, HIGH);
  digitalWrite(line, LOW);
}

int* NESController::pollState()
{
  int i = 0, *button_state = (int*) malloc(sizeof(int)*8);
  memset(button_state, 0, sizeof(int)*8);
   
  pulse(latch_line);
    
  for(i = 0; i < 8; i++) 
  {    
    if(digitalRead(data_line) == LOW)
    {
      button_state[i] = HIGH;
    }
    else
    {
      button_state[i] = LOW;
    }
    
    pulse(clock_line);
  }
  
  return button_state;
}

/* The main logic of the controller, meant to be called once per loop() 
   in the main arduino application */ 
void NESController::run()
{
  int* delete_state = last_state;
  last_state = current_state;
  current_state = pollState();
  
  /* If our recording buffer is full, play a tone
     and end the recording. */
  if(buffer_position == RECORD_BUFFER_MAX)
  {
    tone(8, 55);
    buffer_position = 0;
    recording = false;
  }
  
  /* Iterate over the eight possible button states */
  for(int i = 0; i < 8; i++) 
  {
    if(current_state[i] != last_state[i]) 
    {
      if(current_state[i] == 1)
      {
        keyPressed(i);
        
        if(isRecording())
        {
          down_timers[i] = millis();
          last_note_recorded = millis();
        }
      }
      else
      {
        keyReleased(i);
        
        /* During recording three things are stored for each time we release a button.
           The button released, the time it was down, and the time before playing it. */
        if(isRecording())
        {
          recording_buffer[buffer_position] = (int*) malloc(sizeof(int)*3);
          recording_buffer[buffer_position][0] = i;
          recording_buffer[buffer_position][1] = millis() - down_timers[i];
          recording_buffer[buffer_position][2] = millis() - last_note_recorded - recording_buffer[buffer_position][1];
          
          buffer_position = buffer_position + 1;
          last_note_recorded = millis();
        }
      }
    }
  }
 
  free(delete_state);
}

void NESController::toggleRecording()
{
  recording = !recording;
  
  /* Ensure a clean recording state. */
  if (recording)
  {
    last_note_recorded = millis(); 
    buffer_position = 0;
    
    for(int i = 0; i < 8; i++)
    {  
      down_timers[i] = millis();
    }
    
    for(int i = 0; i < RECORD_BUFFER_MAX; i++)
    {
      if(recording_buffer[i] != 0)
      {
        delete recording_buffer[i];
        recording_buffer[i] = 0;
      }
    }
  }
}

int NESController::isRecording()
{
  return recording;
}

int** NESController::getReplayStates()
{
  return recording_buffer;
}

void NESController::registerListeners(void (*keypressed_listener)(int), 
                                      void (*keyreleased_listener)(int))
{
  keyReleased = keyreleased_listener;
  keyPressed = keypressed_listener;
}

void NESController::unregisterListeners()
{
  keyPressed = NESControllerDummies::dummyKeyPressed;
  keyReleased = NESControllerDummies::dummyKeyReleased;
}

void NESControllerDummies::dummyKeyPressed(int){}
void NESControllerDummies::dummyKeyReleased(int){}
