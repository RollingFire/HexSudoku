/****************************************************************************
Program: HexBoard
Version: 1.01
Auther: Austin Dorsey
Created: 1/24/18
Last Modified: 2/10/18
Summery:
    Sudoku game that is 0 - F. Holds the code for inputs and outputs.
Status:
           display - working - Unussed
      displayError - working - Unussed
       showMessage - working - Unussed
      startOptions - working - Unussed
           options - working - Unussed
       promptInput - working - Unussed
 promptDifficaulty - working - Unussed
    promptPosition - working - Unussed - Does not tell the user how to quit.
       promptValue - working - Unussed - Does not tell the user how to quit.
      showPossible - working - Unussed
           getFile - working - Unussed
           askSave - working - Unussed
         charToInt - working
         intToChar - working
****************************************************************************/
#include "IO.h"
#include "HexBoard.h"
#include "Position.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

/******************************************************************************
Displays the board.
******************************************************************************/
void display(Board *puzzle) {
   cout << "\n   A B C D   E F G H   I J K L   M N O P\n";
   for (int j = 0; j < 16; j++) {
      if (j == 4 || j == 8 || j == 12) {
         cout << "   --------+---------+---------+---------\n";
      }
      cout << j << ((j < 10) ? "  " : " ");
      for (int i = 0; i < 16; i++) {
         if (i == 4 || i == 8 || i == 12) {
            cout << "| ";
         }
         int val = puzzle -> getCell(j, i) -> num;
         cout << intToChar(val) << " ";
      }
      cout << endl;
   }
}

/******************************************************************************
Displays string errors
******************************************************************************/
void displayError(string err) {
   cout << err << '\n';
}

/******************************************************************************
Displays the message.
******************************************************************************/
void showMessage(std::string message) {
   cout << message;
}

void showMessage(char* message) {
   cout << message;
}

/******************************************************************************
Displays the options that are avalible at the start of the game before loading.
******************************************************************************/
void startOptions() {
   cout << "\n1. New Game\n"
          << "2. Load Game\n"
          << "Q. Quit\n";
}

/******************************************************************************
Displays the options of the game.
******************************************************************************/
void options() {
   cout << "\n1. New Game\n"
          << "2. Load Game\n"
          << "3. Save Game\n"
          << "4. Show Options\n"
          << "5. Edit Square\n"
          << "6. Show Possibilities\n"
          << "Q. Quit\n";
}

/******************************************************************************
Asks the user
******************************************************************************/
int promptInput() {
   while (true) {
      char in;
      cout << "> ";
      cin >> in;
      if (cin.fail()) {
         displayError("Invaled input.");
         cin.clear();
         cin.ignore(256, '\n');
         continue;
      }
      cin.clear();
      cin.ignore(256, '\n');
      return in;
   }
}

/******************************************************************************
Asked the user for the difficaulty between 0 and 100.
******************************************************************************/
int promptDifficaulty() throw (string, int) {
   while (true) {
      int difficaulty;
      std::string input;
      cout << "Enter difficaulty: ";
      cin >> input;

      if (toupper(input[0]) == 'Q') {
         throw 0;
      }
      stringstream buffer(input);
      buffer >> difficaulty;
      if (buffer.fail()) {
         displayError("Invaled input.");
         cin.clear();
         cin.ignore(256, '\n');
         continue;
      }
      if (difficaulty < 0 || difficaulty > 100) {
         displayError("Invaled difficaulty. (0-100)");
         cin.clear();
         cin.ignore(256, '\n');
         continue;
      }
      return difficaulty;
   }
}

/******************************************************************************
Prompts user for a cell.
******************************************************************************/
Position promptPosition() throw (std::string, int) {
   Position cell;
   std::string input;
   cout << "Enter cell: ";
   cin >> input;

   if (toupper(input[0]) == 'Q') {
      throw 0;
   }
   else if (isalpha(input[0])) {
      stringstream buffer(input);
      char letter;
      buffer >> letter;
      cell.setY((int)toupper(letter) - 65);
      if (isdigit(buffer.peek())) {
         int num;
         buffer >> num;
         cell.setX(num);
      }
      else {
         cin.clear();
         cin.ignore(256, '\n');
         throw std::string("Invaled input.");
      }
   }
   else if (isdigit(input[0])) {
      stringstream buffer(input);
      int num;
      buffer >> num;
      cell.setX(num);
      if (isalpha(buffer.peek())) {
         char letter;
         buffer >> letter;
         cell.setY((int)toupper(letter) - 65);
      }
      else {
         cin.clear();
         cin.ignore(256, '\n');
         throw std::string("Invaled input.");
      }
   }
   else {
      cin.clear();
      cin.ignore(256, '\n');
      throw std::string("Invaled input.");
   }
   cin.clear();
   cin.ignore(256, '\n');
   return cell;
}

/******************************************************************************
Prompts user for a value.
******************************************************************************/
int promptValue() throw (std::string, int) {
   string val;
   cout << "What value do you want to change it to? ";
   cin >> val;
   if (toupper(val[0]) == 'Q') {
      throw 0;
   }
   if (cin.fail() || val.length() > 1) {
      cin.clear();
      cin.ignore(256, '\n');
      throw std::string("Invaled input.");
   }
   cin.clear();
   cin.ignore(256, '\n');
   return charToInt(val[0]);
}

/******************************************************************************
Displays the possibilities of a cell. Including what it is already set as.
******************************************************************************/
void showPossible(Board* puzzle) {
   Position cell = promptPosition();
   const Tile* A = puzzle -> getCell(cell);
   cout << "Possible values: ";
   for (int i = 0; i < 16; i++) {
      if (A -> possible[i] || A -> num == i ) {
         cout << intToChar(i) << ' ';
      }

   }
   cout << '\n';
}

/******************************************************************************
Gets the filename from the user.
******************************************************************************/
std::string promptFile() {
   string file;
   cout << "Enter the filename: ";
   getline(cin, file);
   return file;
}

/******************************************************************************
Asks the user if they would like to save the game.
******************************************************************************/
bool askSave() {
   char in;
   while (true) {
      cout << "Would you like to save the game? (Y/N) : ";
      cin >> in;
      if (cin.fail()) {
         cout << "Invaled input.\n";
         cin.clear();
         cin.ignore(256, '\n');
         continue;
      }
      switch (toupper(in)) {
         case 'Y':
            return true;
         case 'N':
            return false;
         default:
            cout << "Invaled choice.\n";
      }
   }
}

/******************************************************************************
Converts a char 0-F to an int. G is 16 for blank.
******************************************************************************/
int charToInt(char a) throw (std::string) {
   if (isdigit(a)) {
      return (int)a - 48;  //0 == 0, 1 == 1, ...
   }
   else if (isalpha(a)) {
      int num = (int)(toupper(a)) - 55; //A == 10, B == 11, ...
      if (num > 16) {
         throw std::string("Letter out of range.");
      }
      else {
         return num;
      }
   }
   else if (a == '.') {
      return 16;
   }
   else {
      throw std::string("Not a number or letter");
   }
}

/******************************************************************************
Converts a char 0-F to an int.
******************************************************************************/
char intToChar(int a) throw (std::string) {
   switch (a) {
      case 10:
         return 'A';
      case 11:
         return 'B';
      case 12:
         return 'C';
      case 13:
         return 'D';
      case 14:
         return 'E';
      case 15:
         return 'F';
      case 16:
         return ' ';
      default:
         return (char)(a + 48);
   }
}
