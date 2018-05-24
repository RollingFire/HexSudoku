#include "HexBoard.h"
#include "IO.h"
#include "File.h"
#include "GenBoard.h"
#include "GUI.h"
#include <iostream>
#include <string>
#include <vector>
#include "Interact.h"
#include "DifficaultyGUI.h"
using namespace std;

static vector<string>* OPTIONS = new vector<string>{ "New Game", "Load Game", "Save Game" , "Quit Game"};
static vector<string>* MESSAGES = new vector<string>{ "First message", "Second message" };

/*
int main() {
	Window* window = new Window(OPTIONS);
	cout << getDifficaultyGUI(window);
	return 0;
}
*/

//*
int main() {
   Board* puzzle = new Board;
   //genBoard(puzzle);
   //setDifficaulty(puzzle, 80);
   Window* window = new Window(OPTIONS);
   bool running = true;
   for (int i = 0; i < 5; i++) {
	   MESSAGES->push_back("More messages!");
   }
   while (!glfwWindowShouldClose(window->window) && running)
   {
	   window -> render(puzzle, MESSAGES);

	   if (window->infoP != NULL) {
		   try {
			   string message = interact(puzzle, window);
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
   return 0;
}
//*/