#ifndef IO_H
#define IO_H

#include "HexBoard.h"
#include <string>

void display(Board*);
void displayError(std::string);
void showMessage(std::string);
void showMessage(char*);
void startOptions();
void options();
int promptInput();
int promptDifficaulty() throw (std::string, int);
Position promptPosition() throw (std::string, int);
int promptValue() throw (std::string, int);
void showPossible(Board*);
std::string promptFile();
bool askSave();
int charToInt(char) throw (std::string);
char intToChar(int) throw (std::string);

#endif
