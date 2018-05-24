/****************************************************************************
Program: HexBoard
Version: 0.12
Auther: Austin Dorsey
Created: 1/24/18
Last Modified: 2/7/18
Summery:
    Sudoku game that is 0 - F. Holds the code for the board.

    Board Runs off of 0-15 with 16 being unset.
Status:
Board class:
  updatePossible(int, int, Position) - working
                      getNumPossible - working
                             isValid - working
              editSqu(int, Position) - working
        editSqu(int, Position, bool) - working
             editSqu(Position, bool) - working
                   getCell(int, int) - working
                   getCell(Position) - working
                            getblock - working
****************************************************************************/
#include "HexBoard.h"
#include "Position.h"
#include <string>
#include <cctype>


/******************************************************************************
Initializes blank boards.
******************************************************************************/
Board::Board() {
   for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
         puzzle[i][j].num = 16;
         for (int x = 0; x < 16; x++) {
            puzzle[i][j].possible[x] = true;
         }
         puzzle[i][j].original = false;
      }
   }
}

void Board::reset() {
   for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
         puzzle[i][j].num = 16;
         for (int x = 0; x < 16; x++) {
            puzzle[i][j].possible[x] = true;
         }
         puzzle[i][j].original = false;
      }
   }
}

/******************************************************************************
Changes the values of the cell, and called update updatePossible.
******************************************************************************/
void Board::editSqu(int val, Position cell) throw (std::string) {
   if (puzzle[cell.getX()][cell.getY()].original) {
      throw std::string("Unchangable value.");
   }
   if (val > 16 || val < 0) {
      throw std::string("Invaled Value");
   }
   int oldVal = puzzle[cell.getX()][cell.getY()].num;
   puzzle[cell.getX()][cell.getY()].num = val;
   updatePossible(oldVal, val, cell);
}

void Board::editSqu(int val, Position cell, bool orig) throw (std::string) {
   if (val > 16 || val < 0) {
      throw std::string("Invaled Value");
   }
   int oldVal = puzzle[cell.getX()][cell.getY()].num;
   puzzle[cell.getX()][cell.getY()].num = val;
   puzzle[cell.getX()][cell.getY()].original = orig;
   updatePossible(oldVal, val, cell);
}

void Board::editSqu(Position cell, bool orig) {
   puzzle[cell.getX()][cell.getY()].original = orig;
}

/******************************************************************************
Removes the new value from the row, block, and column. Adds back the old
value to the row, block, and column if valed.
******************************************************************************/
void Board::updatePossible(int oldVal, int newVal, Position cell) {
   Position test = cell;

   //Tests row.
   for (int i = 0; i < 16; i++) {
      if (newVal != 16) {
         puzzle[i][cell.getY()].possible[newVal] = false;
      }
      if (oldVal != 16) {
         test.setX(i);
         puzzle[i][cell.getY()].possible[oldVal] = isValid(oldVal, test);
      }
   }

   //Tests column
   test = cell;
   for (int j = 0; j < 16; j++) {
      if (newVal != 16) {
         puzzle[cell.getX()][j].possible[newVal] = false;
      }
      if (oldVal != 16) {
         test.setY(j);
         puzzle[cell.getX()][j].possible[oldVal] = isValid(oldVal, test);
      }
   }

   //Tests block
   test = getBlock(cell);
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         if (newVal != 16) {
            puzzle[test.getX() + i][test.getY() + j].possible[newVal] = false;
         }
         if (oldVal != 16) {
            test.setY(j);
            test.setX(i);
            puzzle[test.getX() + i][test.getY() + j].possible[oldVal] =
                                                      isValid(oldVal, test);
         }
      }
   }
}

/******************************************************************************
Returns the number of possible values for the given cell.
******************************************************************************/
int Board::getNumPossible(Position cell) {
   int possible = 0;
   for (int x = 0; x < 16; x++) {
      if (puzzle[cell.getX()][cell.getY()].possible[x]) {
         possible++;
      }
   }
   return possible;
}

/******************************************************************************
Check to see is value is already in the row and column.
******************************************************************************/
bool Board::isValid(int val, Position a) {
   //Test Row
   for (int i = 0; i < 16; i++) {
      if (a.getX() == i)
         continue;
      if (val == puzzle[i][a.getY()].num) {
         return false;
      }
   }

   //Test Col
   for (int j = 0; j < 16; j++) {
      if (a.getY() == j)
         continue;
      if (val == puzzle[a.getX()][j].num) {
         return false;
      }
   }

   //Test block
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         if (a.getX() == i + getBlock(a).getX() &&
             a.getY() == j + getBlock(a).getY()) {
            continue;
         }
         if (val == puzzle[i + getBlock(a).getX()]
                          [j + getBlock(a).getY()].num) {
            return false;
         }
      }
   }

   return true;
}

/******************************************************************************
Returns puzzle. 0 is empty, and 1 is 0.
******************************************************************************/
const Tile* Board::getCell(int x, int y) {
   return &puzzle[x][y];
}

const Tile* Board::getCell(Position a) {
   return &puzzle[a.getX()][a.getY()];
}

/******************************************************************************
Returns the top left cell of the block of the Position that is given.
******************************************************************************/
Position getBlock(Position a) {
   Position b;
   b.setX((a.getX() / 4) * 4);
   b.setY((a.getY() / 4) * 4);
   return b;
}
