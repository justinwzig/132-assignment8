/*********************************

  name: justin ziegelmueller
  wustl key: justin.z

  name:
  wustl key:

*********************************/

#include "Arduino.h"

// CLASSES //
#include "prey.h"
#include "predator.h"

Prey prey(0, 4);
Predator predator(0, 0);

byte preyPos[2];
byte predatorPos[2];


// ACCELEREMETOR //
#include <Wire.h>
#include <SparkFun_MMA8452Q.h>

MMA8452Q accel;

// TIMING & CONTROL //
unsigned long preyDelay = 100; //Update this value to change character speed
unsigned long predatorDelay = preyDelay * 2;
unsigned long predatorBlinkDelay = 125;
unsigned long gameLength = 15000;
unsigned long breakLength = 4000;

unsigned long now = 0;
unsigned long preyTimer = 0;
unsigned long predatorTimer = 0;
unsigned long predatorBlinkTimer = 0;
unsigned long gameTimer = 15000;
unsigned long breakTimer = 99999999;
boolean breakSkip = false;

boolean preyCanMove = true;
boolean predatorCanMove = true;
boolean predatorCanDisplay = true;

int preyScore = 0;
int predScore = 0;
boolean predWon = false;

// DISPLAY //
const unsigned long frameLength = 5;
boolean displayData[5][7] = {{0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0}
};

void setup() {
  Serial.begin(9600);
  Serial.println("------initializing------");
  Wire.begin();

  //Set all PinModes
  for (int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }

  //Initialize Acceleremetor
  if (accel.begin() == false) {
    Serial.println("Acceleremetor Error");
    while (true);
  }

  while (true) {
    randomSeed(analogRead(A0));
    int preyRandX = random(0, 4);
    int preyRandY = random(0, 6);
    int predRandX = random(0, 4);
    int predRandY = random(0, 6);
    if (!((preyRandX == predRandX || preyRandX == predRandX + 1) && (preyRandY == predRandY || preyRandY == predRandY + 1))) {
      prey.setStartPos(preyRandX, preyRandY);
      predator.setStartPos(predRandX, predRandY);
      break;
    }
  }

  prey.respawn(preyPos);
  predator.respawn(predatorPos);

  delay(100);
  Serial.println("------ready------");
}

void loop() {
  /// TIMING ///
  now = millis();
  if (now > preyTimer) {
    preyCanMove = true;
    preyTimer = now + preyDelay;
  }
  if (now > predatorTimer) {
    predatorCanMove = true;
    predatorTimer = now + predatorDelay;
  }
  if (now > predatorBlinkTimer) {
    predatorCanDisplay = !predatorCanDisplay;
    predatorBlinkTimer = now + predatorBlinkDelay;
  }
  if (now > gameTimer && !breakSkip) {
    Serial.println("------end of game-----");
    if (predWon) {
      predScore++;
    } else {
      preyScore++;
    }
    displayBreak();
    breakSkip = true;
    breakTimer = now + breakLength;
  }
  if (now > breakTimer && breakSkip) {
    Serial.println("------end of break-----");
    reset();
    gameTimer = now + gameLength;
  }

  /// GAMEPLAY LOGIC ///
  if (!breakSkip) { //Skip all if duing break (go to display logic)
    
    /// PREY LOGIC ///
    if (accel.available() && preyCanMove) {
      if (accel.isRight()) { //EAST
        Serial.println("Reading: East");
        prey.move(1, preyPos);
      } else if (accel.isLeft()) { //WEST
        Serial.println("Reading: West");
        prey.move(3, preyPos);
      } else if (accel.isDown()) { //SOUTH
        Serial.println("Reading: South");
        prey.move(2, preyPos);
      } else if (accel.isUp()) { //NORTH
        Serial.println("Reading: North");
        prey.move(0, preyPos);
      }
    }

    /// PREDATOR LOGIC ///
    if (predatorCanMove) {
      if(Serial.available()>0){
        int direction = (int)Serial.read();
        predator.move(direction, predatorPos);
      }
    }

    /// KILL LOGIC ///
    if ((preyPos[0] == predatorPos[0] || preyPos[0] == predatorPos[0] + 1) && (preyPos[1] == predatorPos[1] || preyPos[1] == predatorPos[1] + 1)) {
      predWon = true;
      gameTimer = 0;
    }

    /// CHARACTER LIGHTS ///
    for (int x = 0; x < 5; x++) {
      for (int y = 0; y < 7; y++) {
        if (x == preyPos[0] && y == preyPos[1]) {
          displayData[x][y] = 1;
        }
        if ((x == predatorPos[0] && y == predatorPos[1]) && predatorCanDisplay) {
          displayData[x][y] = 1;
          displayData[x + 1][y] = 1;
          displayData[x][y + 1] = 1;
          displayData[x + 1][y + 1] = 1;
        }
      }
    }

    /// CHARACTER TIMING ///
    predatorCanMove = false;
    preyCanMove = false;

  } //breakskip

  ///////////////////////////// LIGHT LOGIC ////////////////////////
  for (int currentCol = 0; currentCol < 5; currentCol++) {
    //set next time for delta timing
    unsigned long next = millis() + frameLength / 2;

    //set all columns to high
    for (int allCols = 0; allCols < 5; allCols++) {
      digitalWrite(allCols + 9, HIGH);
    }

    //revert current column to low
    digitalWrite(currentCol + 9, LOW);

    //read row data and write to the digital pins
    for (int row = 0; row < 7; row++) {
      digitalWrite(row + 2, displayData[currentCol][row]); //bitRead(font_5x7[currentCharacter][currentCol - 9], row - 1));
    }

    //Delta timing
    while (millis() < next);
  }

  if (!breakSkip) {
    //Wipe display
    for (int x = 0; x < 5; x++) {
      for (int y = 0; y < 7; y++) {
        displayData[x][y] = 0;
      }
    }
  }
}

void displayBreak() {
  now = millis(); 
  
  int winnerScore, loserScore;
  if (predWon) {
    Serial.println("Predator won");
    displayData[2][3] = 1;
    displayData[3][3] = 1;
    displayData[2][4] = 1;
    displayData[3][4] = 1;
    winnerScore = predScore;
    loserScore = preyScore;
  } else {
    Serial.println("Prey won");
    displayData[2][3] = 1;
    winnerScore = preyScore;
    loserScore = predScore;
  }
  Serial.print("Score: Prey - ");
  Serial.print(preyScore);
  Serial.print(" Predator - ");
  Serial.println(predScore);
  if (winnerScore == loserScore) {
    displayData[2][6] = 1;
  } else if (winnerScore + 1 == loserScore || winnerScore + 2 == loserScore) {
    displayData[2][6] = 1;
    displayData[1][6] = 1;
  } else if (winnerScore - 1 == loserScore || winnerScore - 2 == loserScore) {
    displayData[2][6] = 1;
    displayData[3][6] = 1;
  } else if (winnerScore < loserScore) {
    displayData[1][6] = 1;
    displayData[0][6] = 1;
    displayData[2][6] = 1;
  } else if (winnerScore > loserScore) {
    displayData[2][6] = 1;
    displayData[3][6] = 1;
    displayData[4][6] = 1;
  }
}

void reset() {
  predWon=false;
  
  preyTimer = 0;
  predatorTimer = 0;
  predatorBlinkTimer = 0;

  preyCanMove = true;
  predatorCanMove = true;
  predatorCanDisplay = true;

  breakSkip = false;

  while (true) {
    randomSeed(millis());
    int preyRandX = random(0, 4);
    randomSeed(millis());
    int preyRandY = random(0, 6);
    int predRandX = random(0, 4);
    int predRandY = random(0, 6);
    randomSeed(millis());
    if (!((preyRandX == predRandX || preyRandX == predRandX + 1) && (preyRandY == predRandY || preyRandY == predRandY + 1))) {
      prey.setStartPos(preyRandX, preyRandY);
      predator.setStartPos(predRandX, predRandY);
      break;
    }
  }

  prey.respawn(preyPos);
  predator.respawn(predatorPos);
}
