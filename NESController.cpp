#include "Arduino.h"
#include "NESController.h"

NESController::NESController(int data, int latch, int clock) 
{
  data_line = data;
  latch_line = latch;
  clock_line = clock;
}

int* NESController::pollState()
{
  int i = 0, *button_state = (int*) malloc(sizeof(int)*8);
  memset(button_state, 0, sizeof(int)*8);
   
  digitalWrite(latch_line, 1);
  delay(2);
  digitalWrite(latch_line, 0);
  delay(2);
    
  for(i = 0; i < 8; i++) 
  {    
    if(digitalRead(data_line) == 0)
    {
      button_state[i] = 1;
    }
    else
    {
      button_state[i] = 0;
    }
    
    digitalWrite(clock_line, 1);
    delay(2);
    digitalWrite(clock_line, 0);
    delay(2);
  }
  
  return button_state;
}
