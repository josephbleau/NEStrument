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

namespace NESControllerDummies
{
  void dummyKeyPressed(int);
  void dummyKeyReleased(int);
}

class NESController {
public:
  static const int RECORD_BUFFER_MAX = 64;
  
private:
  int data_line;
  int clock_line;
  int latch_line;
  
  void (*keyPressed)(int);
  void (*keyReleased)(int);

  int recording;
  int* recording_buffer[NESController::RECORD_BUFFER_MAX];
  int buffer_position;
  int down_timers[8];
  int last_note_recorded;
  
  int* current_state;
  int* last_state;
  
  void pulse(int line);
  int* pollState();  
  
public:
  NESController(int data, int latch, int clock);
  ~NESController();
  
  void run();
  
  void toggleRecording();
  int isRecording();
 
  int** getReplayStates();
  
  void registerListeners(void (*)(int), void (*)(int));
  void unregisterListeners();
};

#endif
