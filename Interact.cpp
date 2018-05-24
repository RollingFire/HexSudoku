/****************************************************************************
Program: HexBoard
Version: 0.02
Auther: Austin Dorsey
Created: 3/9/18
Last Modified: 3/21/18
Summery:
Sudoku game that is 0 - F. Holds the code for reading and writing files.
Status:

Todo:
	Load
	Save
	Asks if save on quit if needed.
****************************************************************************/

#include "Interact.h"
#include "HexBoard.h"
#include "GUI.h"
#include "GenBoard.h"
#include "File.h"

#include "iostream"
#include <string>
using namespace std;

string interact(Board* puzzle, InteractInfo* infoP) throw (bool) {
	string message = "";

	//Edit
	if (infoP->action == EDIT_SQU) {
		if (puzzle->getCell(infoP->cell)->original) {
			message = "Value can not be changed.";
		}
		else if (puzzle->getCell(infoP->cell)->num != infoP->num) {
			puzzle->editSqu(infoP->num, infoP->cell);
		}
		else {
			puzzle->editSqu(16, infoP->cell);
		}
		goto end;
	}

	//New game
	else if (infoP->action == NEW_GAME) {
		try {
			//Asks save.
			//Prompt difficaulty
			genBoard(puzzle);
			setDifficaulty(puzzle, infoP->num);
		}
		catch (string err) {
			message = err;
		}
		catch (...) {}
		goto end;
	}

	//Save game
	else if (infoP->action == SAVE_GAME) {
		try {
			writeFile(puzzle, infoP->fileName);
			message = "Game saved.";
		}
		catch (string err) {
			message = err;
		}
		goto end;
	}

	//Load game
	else if (infoP->action == LOAD_GAME) {
		try {
			readFile(puzzle, infoP->fileName);
			message = "Load successful.";
		}
		catch (string err) {
			message = err;
		}
		goto end;
	}

	//Quit game
	else if (infoP->action == QUIT_GAME) {
		throw false;
	}
	else {
		std::cout << infoP->action;
	}

end:
	infoP = NULL;
	return message;
}
