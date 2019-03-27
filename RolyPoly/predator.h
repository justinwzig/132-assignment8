#ifndef Predator_h
#define Predator_h

#include "Arduino.h"

class Predator {
  public:
    Predator(int startX, int startY);

    int _currentX;
    int _currentY;
    int _startX;
    int _startY;

    String debug();
    void setStartPos(int newX, int newY); //resets the start pos
    void respawn(byte predatorPosData[]); //returns the prey's position to the default (upper-right) and writes position after move to array passed in.
    void move(int dir, byte predatorPosData[]); //writes position after move to array passed into second arguement
    
  private:
    
    
};

#endif
