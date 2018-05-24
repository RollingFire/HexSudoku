#ifndef INTERACT_H
#define INTERACT_H

#include "Position.h"
#include "HexBoard.h"
#include "GUI.h"
#include <string>

#define QUIT_GAME 0
#define NEW_GAME 1
#define LOAD_GAME 2
#define SAVE_GAME 3
#define EDIT_SQU 4

std::string interact(Board* puzzle, InteractInfo* infoP) throw (bool);

#endif
