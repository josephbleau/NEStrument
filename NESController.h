#ifndef NES_H
#define NES_H

// NES Buttons
#define BTN_A       0 
#define BTN_B       1
#define BTN_SELECT  2
#define BTN_START   3
#define BTN_UP      4
#define BTN_DOWN    5
#define BTN_LEFT    6
#define BTN_RIGHT   7

class NESController {
  int data_line;
  int clock_line;
  int latch_line;
  
public:
  NESController(int data, int latch, int clock);
  ~NESController();

  /* Returns a vector of integers, 8 integers wide, each containing
     a 1 or 0, a 1 if the button is currently down, a 0 if not. */
  int* pollState();  
};

#endif
