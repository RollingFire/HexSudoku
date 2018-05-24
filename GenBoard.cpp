/****************************************************************************
Program: HexSudoku
Version: 0.3
Auther: Austin Dorsey
Created: 2/2/18
Last Modified: 2/7/18
Summery:
   Generates Sudoku game that is 0 - F
Status:
        genBoard - working
          check0 - working
       checkMore - working
  setDifficaulty - working
Todo:
   Add a time out function for a board.
   Check others to see if unique.
****************************************************************************/
#include "GenBoard.h"
#include "HexBoard.h"
#include "Position.h"
#include "File.h"
#include "IO.h"
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
using namespace std;

//Prototypes for functions that I don't want in header.
int check0(Board*) throw (int);
int checkMore(Board*);

const int MIN = 100;
const int MAX = 180;

/******************************************************************************
Generates the given number of master boards.
******************************************************************************/
void genBoard(Board* puzzle) {
   srand(time(NULL));
   puzzle -> reset();
   while (true) {
      int totelSet = 0;
      try {
         while (totelSet < 256) {
            while (totelSet <= 16) {
               int x = rand() % 16;
               int y = rand() % 16;
               Position cell(x, y);
               if (puzzle -> getCell(cell) -> num != 16) {
                  continue;
               }
               while (true) {
                  int val = rand() % 16;
                  if (puzzle -> getCell(cell) -> possible[val]) {
                     puzzle -> editSqu(val, cell);
                     totelSet++;
                     break;
                  }
               }
            }

            int change = check0(puzzle);
            if (change) {
               totelSet += change;
               continue;
            }

            change = checkMore(puzzle);
            if (change) {
               totelSet += change;
               continue;
            }
         }
      }
      catch (int) {
         puzzle -> reset();
         continue;
      }
      catch (std::string err) {
         displayError(err);
      }
      break;
   }
}

/******************************************************************************
Checks to see if there are any cells that do not have possible values. If
there is, it removes an affecting value. Will continue if the removed value
only has 1 possibility. Repeates upto 5 times.
******************************************************************************/
int check0(Board* puzzle) throw (int) {
   for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
         Position cell(i, j);
         if (puzzle -> getCell(cell) -> num != 16) {
            continue;
         }
         if (!puzzle -> getNumPossible(cell)) {
            int numChanged = 0;
            int runs = 0;
            while (true) {
               runs++;
               //Get random cell in row, column, or block.
               int direction = rand() % 3; //1 = row
               Position newCell;
               if (direction != 3)
               {
                  //Debug(cout << "Row/Col\n";)
                  int position = rand() % 16;
                  while (position == (direction ? cell.getX() : cell.getY())) {
                     position = rand() % 16;
                  }

                  if (direction) {
                     newCell.setX(position);
                     newCell.setY(cell.getY());
                  }
                  else {
                     newCell.setY(position);
                     newCell.setX(cell.getX());
                  }
               }
               else {
                  //Debug(cout << "Block\n";)
                  int row = rand() % 4;
                  int col = rand() % 4;
                  while (row == cell.getX() || col == cell.getY()) {
                     row = rand() % 4;
                     col = rand() % 4;
                  }
                  Position block = getBlock(cell);
                  newCell.setY(col + block.getY());
                  newCell.setX(row + block.getX());
               }
               if (puzzle -> getCell(newCell) -> num == 16) {
                  continue;
               }
               cell = newCell;

               //Blanks out cell.
               puzzle -> editSqu(16, cell);
               numChanged--;
               if (puzzle -> getNumPossible(cell) > 1) {
                  return numChanged;
               }

               if (runs >= 5) {
                  throw -1;
               }
            }
         }
      }
   }
   return 0; //Should never get called.
}

/******************************************************************************
Chooces a random value for a random cell eith the lowest number of
possible numbers.
******************************************************************************/
int checkMore(Board* puzzle) {
   for (int pos = 1; pos < 16; pos++) {
      std::vector<Position> list;
      for (int i = 0; i < 16; i++) {
         for (int j = 0; j < 16; j++) {
            Position cell(i, j);
            if (puzzle -> getCell(cell) -> num == 16 &&
                puzzle -> getNumPossible(cell) == pos) {
               list.push_back(cell);
            }
         }
      }
      if (!list.size()) {
         continue;
      }

      Position cell = list.at(rand() % list.size());
      int option = rand() % (puzzle -> getNumPossible(cell));
      int x = 0;
      for (int val = 0; val < 16; val++){
         if (puzzle -> getCell(cell) -> possible[val]) {
            if (x == option) {
               puzzle -> editSqu(val, cell);
               return 1;
            }
            x++;
         }
      }
   }
   return 0;
}

/******************************************************************************
Sets random cells to blank. Number of cells removed is determined by percent
difficaulty. Sets remaining cells as original.
******************************************************************************/
void setDifficaulty(Board* puzzle, int difficaulty) {
   srand(time(NULL));
   int toRemove = ((MAX - MIN) * difficaulty / 100) + MIN;
   if (toRemove < MIN) {
      toRemove = MIN;
   }
   else if (toRemove > MAX) {
      toRemove = MAX;
   }

   while (toRemove > 0) {
      int x = rand() % 16;
      int y = rand() % 16;
      Position cell(x, y);
      if (puzzle -> getCell(cell) -> num == 16) {
         continue;
      }
      puzzle -> editSqu(16, cell);
      toRemove--;
   }

   for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
         Position cell(i, j);
         if (puzzle -> getCell(cell) -> num != 16) {
            puzzle -> editSqu(cell, true);
         }
      }
   }
}
