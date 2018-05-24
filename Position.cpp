/****************************************************************************
Program: Position
Version: 0.12
Auther: Austin Dorsey
Created: 2/5/18
Last Modified: 2/5/18
Summery:
    Sudoku game that is 0 - F. Holds the code for the board.

    Board Runs off of 0-15 with 16 being unset.
****************************************************************************/
#include "Position.h"

/******************************************************************************
Initializes a blank position.
******************************************************************************/
Position::Position() {
   x = 0;
   y = 0;
}

/******************************************************************************
Initializes a filled position.
******************************************************************************/
Position::Position(int i, int j) {
   try {
      setX(i);
      setY(j);
   }
   catch (std::string err) {
      throw err;
   }
}

/******************************************************************************
Returns x.
******************************************************************************/
int Position::getX() {
   return x;
}

/******************************************************************************
To set the x position and chacks to make sure that it is in the range of
valid space.
******************************************************************************/
void Position::setX(int set) throw (std::string) {
   if (set < 16 && set >= 0)
      x = set;
   else
      throw std::string("Invaled Position");
}

void Position::setX(char set) throw (std::string) {
   if (isdigit(set)) {
      x = (int)set - 48;  //0 == 0, 1 == 1, ...
   }
   else if (isalpha(set)) {
      int num = ((int)(toupper(set)) - 55); //A == 10, B == 11, ...
      if (num >= 10 && num <= 15) {
         x = num;
      }
      else {
         throw std::string("Invaled Position");
      }
   }
   else {
      throw std::string("Invaled Position");
   }
}

/******************************************************************************
Returns y.
******************************************************************************/
int Position::getY() {
   return y;
}

/******************************************************************************
To set the y position and chacks to make sure that it is in the range of
valid space.
******************************************************************************/
void Position::setY(int set) throw (std::string) {
   if (set < 16 && set >= 0)
      y = set;
   else
      throw std::string("Invaled Position");
}

void Position::setY(char set) throw (std::string) {
   if (isdigit(set)) {
      y = (int)set - 48;  //0 == 0, 1 == 1, ...
   }
   else if (isalpha(set)) {
      int num = ((int)(toupper(set)) - 55); //A == 10, B == 11, ...
      if (num >= 10 && num <= 15) {
         y = num;
      }
      else {
         throw std::string("Invaled Position");
      }
   }
   else {
      throw std::string("Invaled Position");
   }
}
