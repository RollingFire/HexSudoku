/****************************************************************************
Program: HexSudoku
Version: 0.2
Auther: Austin Dorsey
Created: 2/1/18
Last Modified: 3/21/18
Summery:
   Sudoku game that is 0 - F
Todo:
	Creat board directory
	Balance MIN MAX in difficaulty?
	IOS/Android App?
	Ask if want to save before...
	Loading curser.
	Instructions option?
	Victory message
	Pencaling in
	Highlight the selected number across the board.
****************************************************************************/
#include "HexBoard.h"
#include "GenBoard.h"
#include "IO.h"
#include "File.h"
#include "GUI.h"
#include "Interact.h"
#include <string>
#include <vector>
using namespace std;

static vector<string>* OPTIONS = new vector<string>{ "New Game", "Load Game", "Save Game" , "Quit Game" };
static vector<string>* MESSAGES = new vector<string>{};

int main() {
   Board* puzzle = new Board;
   Window* window = new Window(OPTIONS);

   bool running = true;
   while (!glfwWindowShouldClose(window->window) && running)
   {
	   window->render(puzzle, MESSAGES);

	   if (window->infoP != NULL) {
		   try {
			   string message = interact(puzzle, window->infoP);
			   if (message != "") {
				   MESSAGES->push_back(message);
			   }
			   window->infoP = NULL;
		   }
		   catch (bool state) {
			   running = state;
		   }
		   catch (string err) {
			   MESSAGES->push_back(err);
		   }
	   }
   }
   window -> ~Window();
   puzzle -> ~Board();
   delete puzzle;
   delete window;
   return 0;
}
