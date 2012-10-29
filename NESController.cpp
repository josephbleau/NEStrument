#include "Arduino.h"
#include "NESController.h"

NESController::NESController(int data, int latch, int clock) 
{
  for(int i = 0; i < 500; i++)
    recording_buffer[i] = 0;
    
  data_line = data;
  latch_line = latch;
  clock_line = clock;
  last_state = 0;
  current_state = 0;
  recording = 0;
  buffer_position = 0;
  keyPressed = NESControllerDummies::dummyKeyPressed;
  keyReleased = NESControllerDummies::dummyKeyReleased;
}

NESController::~NESController(){}

void NESController::pulse(int line)
{
  digitalWrite(line, HIGH);
  delay(2);
  digitalWrite(line, LOW);
  delay(2);
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

void NESController::run()
{
  int* delete_state = last_state;
  last_state = current_state;
  current_state = pollState();
  
  
  for(int i = 0; i < 8; i++) 
  {
    if(current_state[i] != last_state[i]) 
    {
      if(current_state[i] == HIGH)
      {
        keyPressed(i);
      }
      else
      {
        keyReleased(i);
      }
    }
  }
  
  if(isRecording())
  {
    if(millis() - recording_timer >= 40)
    {
      recording_timer = millis();
      recording_buffer[buffer_position] = current_state;
      buffer_position = buffer_position + 1;
      
      if(buffer_position == 249)
      {
        buffer_position = 0;
        toggleRecording();
      }
    }
  }
  
  if(delete_state != 0 && !isRecording())
  {
    free(delete_state);
  }
}

void NESController::toggleRecording()
{
  recording = !recording;
  
  if(recording)
  {
    recording_timer = millis();
  }
  else
  {
    for(int i = 0; i < 250; i++)
    {
      if(recording_buffer[i] != 0)
      {
        delete recording_buffer[i];
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
