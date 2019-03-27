#include "Arduino.h"
#include "Prey.h"

Prey::Prey(int startX, int startY){
  _startX=startX;
  _startY=startY;
  _currentX=_startX;
  _currentY=_startY;
}

//dum dum method for dum dum programmer
String Prey::debug(){
  return "good";
}

void Prey::setStartPos(int newX, int newY){
  _startX=newX;
  _startY=newY;
}

void Prey::respawn(byte preyPosData[]){
  _currentX=_startX;
  _currentY=_startY;

  preyPosData[0]=_currentX;
  preyPosData[1]=_currentY;
}

void Prey::move(int dir, byte preyPosData[]){
  //this is a naieve implementation but wanted to keep it neat
  //moves the current coordinate in the array according to direction passed in
  //deals with edge detection (prevents movement if current coordinate is an edge)
  Serial.print("Prey Dir = ");
  Serial.print(dir);
  
         if (dir == 0 && _currentY>0){ //NORTH
    _currentY--;
  } else if (dir == 1 && _currentX<4){ //EAST
    _currentX++;
  } else if (dir == 2 && _currentY<6){ //SOUTH
    _currentY++;
  } else if (dir == 3 && _currentX>0){ //WEST
    _currentX--;
  }

  Serial.print(", Prey Moved to x: ");
  Serial.print(_currentX);
  Serial.print(" y: ");
  Serial.println(_currentY);
  
  preyPosData[0]=_currentX;
  preyPosData[1]=_currentY;  
}
