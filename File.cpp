/****************************************************************************
Program: HexBoard
Version: 0.11
Auther: Austin Dorsey
Created: 1/25/18
Last Modified: 2/7/18
Summery:
    Sudoku game that is 0 - F. Holds the code for reading and writing files.
Status:
     loadNew - working - Not being used.
    loadSave - working
    readFile - working
    saveFile - working
   writeFile - working
****************************************************************************/
#include "HexBoard.h"
#include "File.h"
#include "IO.h"
#include "GenBoard.h"
#include "Position.h"
#include <string>
#include <fstream>
using namespace std;

/******************************************************************************
Loads boards from the master boards.
******************************************************************************/
void loadNew(Board* puzzle) {
   string filename = "Boards/Masters/" + promptFile();
   readFile(puzzle, filename);
   int difficaulty = promptDifficaulty();
   setDifficaulty(puzzle, difficaulty);
}

/******************************************************************************
Calls promptFile and readFile. Adding the file directory to the filename.
******************************************************************************/
void loadSave(Board* puzzle) {
   string filename = "Boards/" + promptFile();
   readFile(puzzle, filename);
}

/******************************************************************************
Calls promptFile and writeFile. Adding the file directory to the filename.
******************************************************************************/
void saveFile(Board* puzzle) {
   string filename = "Boards/" + promptFile();
   writeFile(puzzle, filename);
   showMessage(std::string("Game saved.\n"));
}

/******************************************************************************
Reads the file at the filename, and inports it to the board.
******************************************************************************/
void readFile(Board* puzzle, std::string filename) throw (std::string) {
   std::ifstream fin(filename);
   if (fin.fail()) {
      fin.close();
      throw string("Failed to read file.");
   }

   string read;
   for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
         Position cell(i, j);
         fin >> read;
         if (fin.fail()) {
            fin.close();
            throw string("Invaled file.");
         }

         try {
            int num = charToInt(read[0]);

            //Checks the original condition.
            bool original;
            if (toupper(read[1]) == 'T') {
               if (num == 16) {
                  original = false;
               }
               else {
                  original = true;
               }
            }
            else if (toupper(read[1]) == 'F') {
               original = false;
            }
            else {
               fin.close();
               throw string("Invaled file.");
            }

            //Calls edit square.
            if (num <= 16 && num >= 0) {
               puzzle -> editSqu(num, cell, original);
            }
            else {
               fin.close();
               throw string("Invaled file.");
            }
         }
         catch (std::string) {
            fin.close();
            throw string("Invaled file.");
         }
      }
   }
   fin.close();
}

/******************************************************************************
Writes the board to the given filename.
******************************************************************************/
void writeFile(Board* puzzle, std::string filename) throw (std::string) {
   ofstream fout(filename);
   if (fout.fail()) {
      fout.close();
      throw string("Failed to open file.");
   }

   for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
         const Tile* cell = puzzle -> getCell(i, j);
         fout << (intToChar(cell -> num) == ' ' ? 'G' : intToChar(cell -> num))
              << (cell -> original ? 'T' : 'F') << ' ';
      }
      fout << '\n';
   }
   fout.close();
}
