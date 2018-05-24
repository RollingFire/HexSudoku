/****************************************************************************
Program: HexBoard
Version: 0.07
Auther: Austin Dorsey
Created: 3/2/18
Last Modified: 3/22/18
Summery:
	Sudoku game that is 0 - F. Holds the code for reading and writing files.
Status:
	      Window  - Working
	      ~Window - Working
    getCharacters - Working
	   	   render - Working
        drawLines - Working
         drawText - Working
 	   renderText - Working
 	 drwaSelected - Working
	  basicOption - working
renderDifficaulty - Working
	   renderLoad - unwriten
	   renderSave - unwriten

  activeMouseCallback - Working
  mouseButtonCallback - Working
           boardMouse - Working
     difficaultyMouse - Working
            saveMouse - Working
            loadMouse - Working

Todo:
	New Text Shader
	Mouse enter/exit?
	Ask if save if that isn't the last action.
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "IO.h"
#include "GUI.h"
#include "HexBoard.h"
#include "Shader.h"
#include "Interact.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void activeMouseCallback(GLFWwindow* window, double x, double y);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void boardMouse(GLFWwindow* window, int button, int action, int mods);
void difficaultyMouse(GLFWwindow* window, int button, int action, int mods);
void saveMouse(GLFWwindow* window, int button, int action, int mods);
void loadMouse(GLFWwindow* window, int button, int action, int mods);

static int NUM_OPTIONS;
static Position* selected;
static Position cell;
static std::vector<std::string>* OPTIONS;
static bool sendInfo = false;
static InteractInfo infoTemp;
static bool highlightPossible = false;
static int renderState = BOARD_STATE;
static int difficaulty = 50;
static bool dragging = false;
static int lastAction = SAVE_GAME;
static std::string fileName;

static const glm::vec3 LINE_COLOR = {0.2f, 0.7f, 0.7f};
static const glm::vec3 WINDOW_COLOR = { 0.1f, 0.5f, 0.6f };
static const glm::vec3 TEXT_COLOR = {0.1f, 0.7f, 0.0f};
static const glm::vec3 HIGHLIGHT_COLOR = {0.6f, 0.6f, 0.0f};


/******************************************************************************
Initializes and opens the window.
******************************************************************************/
Window::Window(std::vector<std::string>* optionsIn) {
	//Window Setup
	if (!glfwInit())
		exit(EXIT_FAILURE);
	
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WIDTH_DEFAULT, HEIGHT_DEFAULT, "HexSudoku", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	//Text Setup
	// Set OpenGL options
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH_DEFAULT), 0.0f, static_cast<GLfloat>(HEIGHT_DEFAULT));
	textShader = new Shader("shaders/text.vs", "shaders/text.frag");
	defaultShader = new Shader("shaders/default.vs", "shaders/default.frag");

	defaultShader->use();
	glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), 0.1, 0.6, 0.6, 0.6);

	getCharacters();
	OPTIONS = optionsIn;
	NUM_OPTIONS = OPTIONS->size();
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, activeMouseCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	renderState = BOARD_STATE;
}

/******************************************************************************
Closes and deleates the window.
******************************************************************************/
Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

/******************************************************************************
Creats a map of the ASCII charactures.
******************************************************************************/
void Window::getCharacters() {
	textShader->use();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projection));

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}
	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte i = 0; i < 128; i++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(i, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/******************************************************************************
Draws the window with board, options, selection, and messages. 
******************************************************************************/
void Window::render(Board* puzzle, std::vector<std::string>* MESSAGES) {
	glfwMakeContextCurrent(window);
	// Clear the colorbuffer
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	drawSelected(puzzle);
	drawLines();
	drawText(puzzle, MESSAGES);
	
	switch (renderState) {
	case DIFFICAULTY_STATE:
		renderDifficaulty();
		break;
	case LOAD_STATE:
		renderLoad();
		break;
	case SAVE_STATE:
		renderSave();
		break;
	default:
		renderState = BOARD_STATE;
	}
	
	glfwSwapBuffers(window);
	glfwPollEvents();

	//Moves the InteractInfo from file scope to class veriable scope.
	if (sendInfo == true) {
		info = infoTemp;
		infoP = &info;
		lastAction = infoP->action;
		sendInfo = false;
	}
}

/******************************************************************************
Draws the lines for the game.
******************************************************************************/
void Window::drawLines() {
	defaultShader->use();
	glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), LINE_COLOR.x, LINE_COLOR.y, LINE_COLOR.z, 1);
	GLfloat thickness;
	//Board
	thickness = 2.2;
	glLineWidth(thickness);
	glBegin(GL_LINE_LOOP);
		glVertex2f(BOARD_LEFT_F                 , BOARD_TOP_F);
		glVertex2f(BOARD_LEFT_F + BOARD_X_SIZE_F, BOARD_TOP_F);
		glVertex2f(BOARD_LEFT_F + BOARD_X_SIZE_F, BOARD_TOP_F - BOARD_Y_SIZE_F);
		glVertex2f(BOARD_LEFT_F                 , BOARD_TOP_F - BOARD_Y_SIZE_F);
	glEnd();

	glBegin(GL_LINES);
	for (int i = 1; i < 4; i++) {
		glVertex2f(BOARD_LEFT_F + (BOARD_X_SIZE_F / 4) * i, BOARD_TOP_F);
		glVertex2f(BOARD_LEFT_F + (BOARD_X_SIZE_F / 4) * i, BOARD_TOP_F - BOARD_Y_SIZE_F);
	}
	for (int i = 1; i < 4; i++) {
		glVertex2f(BOARD_LEFT_F                 , BOARD_TOP_F - (BOARD_Y_SIZE_F / 4) * i);
		glVertex2f(BOARD_LEFT_F + BOARD_X_SIZE_F, BOARD_TOP_F - (BOARD_Y_SIZE_F / 4) * i);
	}
	glEnd();

	thickness = 1.0;
	glLineWidth(thickness);
	glBegin(GL_LINES);
	for (int i = 1; i < 16; i++) {
		glVertex2f(BOARD_LEFT_F + (BOARD_X_SIZE_F / 16) * i, BOARD_TOP_F);
		glVertex2f(BOARD_LEFT_F + (BOARD_X_SIZE_F / 16) * i, BOARD_TOP_F - BOARD_Y_SIZE_F);
	}
	for (int i = 1; i < 16; i++) {
		glVertex2f(BOARD_LEFT_F                 , BOARD_TOP_F - (BOARD_Y_SIZE_F / 16) * i);
		glVertex2f(BOARD_LEFT_F + BOARD_X_SIZE_F, BOARD_TOP_F - (BOARD_Y_SIZE_F / 16) * i);
	}
	glEnd();


	//Number selection.
	thickness = 2.2;
	glLineWidth(thickness);
	glBegin(GL_LINE_LOOP);
		glVertex2f(SELECT_LEFT_F                  , SELECT_TOP_F);
		glVertex2f(SELECT_LEFT_F + SELECT_X_SIZE_F, SELECT_TOP_F);
		glVertex2f(SELECT_LEFT_F + SELECT_X_SIZE_F, SELECT_TOP_F - SELECT_HEIGHT_F);
		glVertex2f(SELECT_LEFT_F                  , SELECT_TOP_F - SELECT_HEIGHT_F);
	glEnd();

	glBegin(GL_LINES);
	for (int i = 1; i < 16; i++) {
		glVertex2f(SELECT_LEFT_F + (SELECT_X_SIZE_F / 16) * i, SELECT_TOP_F);
		glVertex2f(SELECT_LEFT_F + (SELECT_X_SIZE_F / 16) * i, SELECT_TOP_F - SELECT_HEIGHT_F);
	}
	glEnd();

	//Options
	thickness = 2.2;
	glLineWidth(thickness);
	glBegin(GL_LINE_LOOP);
		glVertex2f(OPTION_LEFT_F                 , BOARD_TOP_F);
		glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, BOARD_TOP_F);
		glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, BOARD_TOP_F - OPTION_HEIGHT_F * NUM_OPTIONS);
		glVertex2f(OPTION_LEFT_F                 , BOARD_TOP_F - OPTION_HEIGHT_F * NUM_OPTIONS);
	glEnd();

	glBegin(GL_LINES);
	for (int i = 1; i < NUM_OPTIONS; i++) {
		glVertex2f(OPTION_LEFT_F                 , BOARD_TOP_F - OPTION_HEIGHT_F * i);
		glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, BOARD_TOP_F - OPTION_HEIGHT_F * i);
	}
	glEnd();
	
	//Show possible
	thickness = 2.2;
	glLineWidth(thickness);
	glBegin(GL_LINE_LOOP);
		glVertex2f(OPTION_LEFT_F                 , BOARD_TOP_F - MESSAGE_GAP_F - (OPTION_HEIGHT_F * NUM_OPTIONS));
		glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, BOARD_TOP_F - MESSAGE_GAP_F - (OPTION_HEIGHT_F * NUM_OPTIONS));
		glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, BOARD_TOP_F - MESSAGE_GAP_F - (OPTION_HEIGHT_F * (NUM_OPTIONS + 1)));
		glVertex2f(OPTION_LEFT_F                 , BOARD_TOP_F - MESSAGE_GAP_F - (OPTION_HEIGHT_F * (NUM_OPTIONS + 1)));
	glEnd();

	//Messages
	thickness = 2.2;
	glLineWidth(thickness);
	glBegin(GL_LINE_LOOP);
		glVertex2f(OPTION_LEFT_F                 , BOARD_TOP_F - 2 * MESSAGE_GAP_F - (OPTION_HEIGHT_F * (NUM_OPTIONS + 1)));
		glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, BOARD_TOP_F - 2 * MESSAGE_GAP_F - (OPTION_HEIGHT_F * (NUM_OPTIONS + 1)));
		glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, SELECT_TOP_F - SELECT_HEIGHT_F);
		glVertex2f(OPTION_LEFT_F                 , SELECT_TOP_F - SELECT_HEIGHT_F);
	glEnd();
}

/******************************************************************************
Draws the text for the game.
******************************************************************************/
void Window::drawText(Board* puzzle, std::vector<std::string>* MESSAGES) {
	//Board
	for (int j = 0; j < 16; j++) {
		for (int i = 0; i < 16; i++) {
			int num = puzzle -> getCell(j, i) -> num;
			if (num == 16) {
				continue;
			}
			if (puzzle->getCell(j, i)->original) {
				renderText(std::string(1, intToChar(num)), BOARD_LEFT + BOARD_X_SIZE / 16 * i + 10, BOARD_TOP - BOARD_Y_SIZE / 16 * j - 24, 0.45f, TEXT_COLOR);
			}
			else {
				renderText(std::string(1, intToChar(num)), BOARD_LEFT + BOARD_X_SIZE / 16 * i + 10, BOARD_TOP - BOARD_Y_SIZE / 16 * j - 24, 0.45f, glm::vec3(0, 0, 0));
			}
		}
	}
	//Options
	int i = 1;
	for (std::vector<std::string>::iterator it = OPTIONS->begin(); it != OPTIONS->end(); it++) {
		renderText(*it, OPTION_LEFT + 2, (BOARD_TOP - (OPTION_HEIGHT * i) + 3), 0.8f, TEXT_COLOR);
		i++;
	}

	//Show possible
	renderText("Show Possible", OPTION_LEFT + 2, (BOARD_TOP - (OPTION_HEIGHT * (NUM_OPTIONS + 1)) + 3 - MESSAGE_GAP), 0.7f, TEXT_COLOR);

	//Selection
	for (i = 0; i < 16; i++) {
		renderText(std::string(1, intToChar(i)), SELECT_LEFT + (SELECT_X_SIZE/16) * i + 10, SELECT_TOP - 28, 0.6f, TEXT_COLOR);
	}

	//Messages
	GLfloat returnVal = 15;
	i = 1;
	for (std::vector<std::string>::reverse_iterator  it = MESSAGES->rbegin(); it != MESSAGES->rend(); it++) {
		GLfloat printLn = (BOARD_TOP - (OPTION_HEIGHT * (NUM_OPTIONS + 1)) - (2 * MESSAGE_GAP) - (returnVal * i));
		if (printLn < (SELECT_TOP - SELECT_HEIGHT + 2)) {
			break;
		}
		renderText(*it, OPTION_LEFT + 2, (BOARD_TOP - (OPTION_HEIGHT * (NUM_OPTIONS + 1)) - (2 * MESSAGE_GAP) - (returnVal * i)), 0.32f, TEXT_COLOR);
		i++;
	}

	renderText("Version 2.02  By: Austin Dorsey", WIDTH_DEFAULT - 140, 5, 0.2f, glm::vec3(0, 0, 0));
}

/******************************************************************************
Draws the text.
******************************************************************************/
void Window::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
	
	textShader->use();
	glUniform3f(glGetUniformLocation(textShader->ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos,     ypos,       0.0, 1.0 },
		{ xpos + w, ypos,       1.0, 1.0 },

		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos + w, ypos,       1.0, 1.0 },
		{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/******************************************************************************
Highlights things.
******************************************************************************/
void Window::drawSelected(Board* puzzle) {
	defaultShader->use();
	glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), HIGHLIGHT_COLOR.x, HIGHLIGHT_COLOR.y, HIGHLIGHT_COLOR.z, 0.2);
	GLfloat thickness;

	//Highlights the selected cell.
	if (selected != NULL) {
		thickness = 2.2;
		glLineWidth(thickness);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(BOARD_LEFT_F + (BOARD_X_SIZE_F / 16) * (selected->getY() + 1), BOARD_TOP_F);
			glVertex2f(BOARD_LEFT_F + (BOARD_X_SIZE_F / 16) * selected->getY(), BOARD_TOP_F);
			glVertex2f(BOARD_LEFT_F + (BOARD_X_SIZE_F / 16) * selected->getY(), BOARD_TOP_F - BOARD_Y_SIZE_F);
			glVertex2f(BOARD_LEFT_F + (BOARD_X_SIZE_F / 16) * (selected->getY() + 1), BOARD_TOP_F - BOARD_Y_SIZE_F);
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(BOARD_LEFT_F, BOARD_TOP_F - (BOARD_Y_SIZE_F / 16) * selected->getX());
			glVertex2f(BOARD_LEFT_F, BOARD_TOP_F - ((BOARD_Y_SIZE_F / 16) * (selected->getX() + 1)));
			glVertex2f(BOARD_LEFT_F + BOARD_X_SIZE_F, BOARD_TOP_F - ((BOARD_Y_SIZE_F / 16) * (selected->getX() + 1)));
			glVertex2f(BOARD_LEFT_F + BOARD_X_SIZE_F, BOARD_TOP_F - (BOARD_Y_SIZE_F / 16) * selected->getX());
		glEnd();
	
		//For number line.
		for (int i = 0; i < 16; i++) {
			if (puzzle->getCell(*selected)->original) {
				//Gray
				glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), 0.2, 0.2, 0.2, 0.4);
			}
			else if (puzzle->getCell(*selected)->num == i) {
				//Yellow
				glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), HIGHLIGHT_COLOR.x, HIGHLIGHT_COLOR.y, HIGHLIGHT_COLOR.z, 0.2);
			}
			else if (highlightPossible && !puzzle->isValid(i, *selected)) {
				//Gray
				glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), 0.2, 0.2, 0.2, 0.4);
			}
			else {
				continue;
			}

			glBegin(GL_TRIANGLE_FAN);
				glVertex2f(SELECT_LEFT_F + ((SELECT_X_SIZE_F / 16) * (i + 1)), SELECT_TOP_F);
				glVertex2f(SELECT_LEFT_F + ((SELECT_X_SIZE_F / 16) * i)      , SELECT_TOP_F);
				glVertex2f(SELECT_LEFT_F + ((SELECT_X_SIZE_F / 16) * i), SELECT_TOP_F - SELECT_HEIGHT_F);
				glVertex2f(SELECT_LEFT_F + ((SELECT_X_SIZE_F / 16) * (i + 1)), SELECT_TOP_F - SELECT_HEIGHT_F);
			glEnd();
		}
	}


	//For show possible option.
	if (highlightPossible) {
		glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), HIGHLIGHT_COLOR.x, HIGHLIGHT_COLOR.y, HIGHLIGHT_COLOR.z, 0.2);
		thickness = 2.2;
		glLineWidth(thickness);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, BOARD_TOP_F - MESSAGE_GAP_F - (OPTION_HEIGHT_F * (NUM_OPTIONS + 1)));
			glVertex2f(OPTION_LEFT_F + OPTION_WIDTH_F, BOARD_TOP_F - MESSAGE_GAP_F - (OPTION_HEIGHT_F * NUM_OPTIONS));
			glVertex2f(OPTION_LEFT_F, BOARD_TOP_F - MESSAGE_GAP_F - (OPTION_HEIGHT_F * NUM_OPTIONS));
			glVertex2f(OPTION_LEFT_F, BOARD_TOP_F - MESSAGE_GAP_F - (OPTION_HEIGHT_F * (NUM_OPTIONS + 1)));
		glEnd();
	}
}

/******************************************************************************
Draws the basic pop up prompt window with the buttons okay and cancel.
******************************************************************************/
void Window::renderBasicOptionsWindow() {
	defaultShader->use();
	GLfloat thickness;

	//Blured background.
	glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), 0, 0, 0, 0.7);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
		glVertex2f(1, 1);
		glVertex2f(-1, 1);
	glEnd();

	//Fills
	{
		glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), WINDOW_COLOR.x, WINDOW_COLOR.y, WINDOW_COLOR.z, 1);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(DIFFICAULTY_WINDOW_X_SIZE_F / -2, DIFFICAULTY_WINDOW_Y_SIZE_F / -2);
			glVertex2f(DIFFICAULTY_WINDOW_X_SIZE_F / 2, DIFFICAULTY_WINDOW_Y_SIZE_F / -2);
			glVertex2f(DIFFICAULTY_WINDOW_X_SIZE_F / 2, DIFFICAULTY_WINDOW_Y_SIZE_F / 2);
			glVertex2f(DIFFICAULTY_WINDOW_X_SIZE_F / -2, DIFFICAULTY_WINDOW_Y_SIZE_F / 2);
		glEnd();

		glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), WINDOW_COLOR.x - 0.2, WINDOW_COLOR.y - 0.2, WINDOW_COLOR.z - 0.2, 1);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(DIFFICAULTY_X_SIZE_F / -2, DIFFICAULTY_Y_SIZE_F / -2 + DIFFICAULTY_OFFSET_F);
			glVertex2f(DIFFICAULTY_X_SIZE_F / 2, DIFFICAULTY_Y_SIZE_F / -2 + DIFFICAULTY_OFFSET_F);
			glVertex2f(DIFFICAULTY_X_SIZE_F / 2, DIFFICAULTY_Y_SIZE_F / 2 + DIFFICAULTY_OFFSET_F);
			glVertex2f(DIFFICAULTY_X_SIZE_F / -2, DIFFICAULTY_Y_SIZE_F / 2 + DIFFICAULTY_OFFSET_F);
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F, DIFFICAULTY_OPTIONS_TOP_F - DIFFICAULTY_OPTIONS_Y_SIZE_F);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F + DIFFICAULTY_OPTIONS_X_SIZE_F, DIFFICAULTY_OPTIONS_TOP_F - DIFFICAULTY_OPTIONS_Y_SIZE_F);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F + DIFFICAULTY_OPTIONS_X_SIZE_F, DIFFICAULTY_OPTIONS_TOP_F);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F, DIFFICAULTY_OPTIONS_TOP_F);
		glEnd();
	}

	//Lines
	{
		thickness = 2.2;
		glLineWidth(thickness);
		glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), WINDOW_COLOR.x - 0.4, WINDOW_COLOR.y - 0.4, WINDOW_COLOR.z - 0.4, 0.8);
		glBegin(GL_LINE_LOOP);
			glVertex2f(DIFFICAULTY_WINDOW_X_SIZE_F / -2, DIFFICAULTY_WINDOW_Y_SIZE_F / -2);
			glVertex2f(DIFFICAULTY_WINDOW_X_SIZE_F / 2, DIFFICAULTY_WINDOW_Y_SIZE_F / -2);
			glVertex2f(DIFFICAULTY_WINDOW_X_SIZE_F / 2, DIFFICAULTY_WINDOW_Y_SIZE_F / 2);
			glVertex2f(DIFFICAULTY_WINDOW_X_SIZE_F / -2, DIFFICAULTY_WINDOW_Y_SIZE_F / 2);
		glEnd();

		glBegin(GL_LINE_LOOP);
			glVertex2f(DIFFICAULTY_X_SIZE_F / -2, DIFFICAULTY_Y_SIZE_F / -2 + DIFFICAULTY_OFFSET_F);
			glVertex2f(DIFFICAULTY_X_SIZE_F / 2, DIFFICAULTY_Y_SIZE_F / -2 + DIFFICAULTY_OFFSET_F);
			glVertex2f(DIFFICAULTY_X_SIZE_F / 2, DIFFICAULTY_Y_SIZE_F / 2 + DIFFICAULTY_OFFSET_F);
			glVertex2f(DIFFICAULTY_X_SIZE_F / -2, DIFFICAULTY_Y_SIZE_F / 2 + DIFFICAULTY_OFFSET_F);
		glEnd();

		glBegin(GL_LINE_LOOP);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F, DIFFICAULTY_OPTIONS_TOP_F - DIFFICAULTY_OPTIONS_Y_SIZE_F);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F + DIFFICAULTY_OPTIONS_X_SIZE_F, DIFFICAULTY_OPTIONS_TOP_F - DIFFICAULTY_OPTIONS_Y_SIZE_F);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F + DIFFICAULTY_OPTIONS_X_SIZE_F, DIFFICAULTY_OPTIONS_TOP_F);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F, DIFFICAULTY_OPTIONS_TOP_F);
		glEnd();
		glBegin(GL_LINES);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F + DIFFICAULTY_OPTIONS_X_SIZE_F / 2, DIFFICAULTY_OPTIONS_TOP_F - DIFFICAULTY_OPTIONS_Y_SIZE_F);
			glVertex2f(DIFFICAULTY_OPTIONS_LEFT_F + DIFFICAULTY_OPTIONS_X_SIZE_F / 2, DIFFICAULTY_OPTIONS_TOP_F);
		glEnd();
	}

	renderText("OK", DIFFICAULTY_OPTIONS_LEFT + 2, DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE + 5, 0.5f, TEXT_COLOR);
	renderText("Cancel", DIFFICAULTY_OPTIONS_LEFT + (DIFFICAULTY_OPTIONS_X_SIZE / 2) + 2, DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE + 5, 0.5f, TEXT_COLOR);
}

/******************************************************************************
Draws all of the things nessisary for the difficaulty prompt.
******************************************************************************/
void Window::renderDifficaulty() {
	renderBasicOptionsWindow();
	
	defaultShader->use();
	glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), 0.4, 0.4, 0.4, 1);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f((DIFFICAULTY_X_SIZE_F / -2) + ((DIFFICAULTY_X_SIZE_F - DIFFICAULTY_SLIDER_SIZE_F) / 100) * difficaulty - DIFFICAULTY_SLIDER_SIZE_F / 2 + DIFFICAULTY_SLIDER_OFFSET_F, DIFFICAULTY_Y_SIZE_F / -2 + DIFFICAULTY_OFFSET_F);
		glVertex2f((DIFFICAULTY_X_SIZE_F / -2) + ((DIFFICAULTY_X_SIZE_F - DIFFICAULTY_SLIDER_SIZE_F) / 100) * difficaulty + DIFFICAULTY_SLIDER_SIZE_F / 2 + DIFFICAULTY_SLIDER_OFFSET_F, DIFFICAULTY_Y_SIZE_F / -2 + DIFFICAULTY_OFFSET_F);
		glVertex2f((DIFFICAULTY_X_SIZE_F / -2) + ((DIFFICAULTY_X_SIZE_F - DIFFICAULTY_SLIDER_SIZE_F) / 100) * difficaulty + DIFFICAULTY_SLIDER_SIZE_F / 2 + DIFFICAULTY_SLIDER_OFFSET_F, DIFFICAULTY_Y_SIZE_F / 2 + DIFFICAULTY_OFFSET_F);
		glVertex2f((DIFFICAULTY_X_SIZE_F / -2) + ((DIFFICAULTY_X_SIZE_F - DIFFICAULTY_SLIDER_SIZE_F) / 100) * difficaulty - DIFFICAULTY_SLIDER_SIZE_F / 2 + DIFFICAULTY_SLIDER_OFFSET_F, DIFFICAULTY_Y_SIZE_F / 2 + DIFFICAULTY_OFFSET_F);
	glEnd();

	glUniform4f(glGetUniformLocation(defaultShader->ID, "color"), WINDOW_COLOR.x - 0.4, WINDOW_COLOR.y - 0.4, WINDOW_COLOR.z - 0.4, 0.8);
	glBegin(GL_LINE_LOOP);
		glVertex2f(DIFFICAULTY_X_SIZE_F / -2, DIFFICAULTY_Y_SIZE_F / -2 + DIFFICAULTY_OFFSET_F);
		glVertex2f(DIFFICAULTY_X_SIZE_F / 2, DIFFICAULTY_Y_SIZE_F / -2 + DIFFICAULTY_OFFSET_F);
		glVertex2f(DIFFICAULTY_X_SIZE_F / 2, DIFFICAULTY_Y_SIZE_F / 2 + DIFFICAULTY_OFFSET_F);
		glVertex2f(DIFFICAULTY_X_SIZE_F / -2, DIFFICAULTY_Y_SIZE_F / 2 + DIFFICAULTY_OFFSET_F);
	glEnd();

	renderText(("Difficaulty: " + std::to_string(difficaulty)), WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2, DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE + 5, 0.5f, TEXT_COLOR);
}

/******************************************************************************
Draws all of the things nessisary for the save prompt.
******************************************************************************/
void Window::renderSave() {
	renderBasicOptionsWindow();

	renderText(fileName, WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2 + 2, HEIGHT_DEFAULT / 2 - DIFFICAULTY_OPTIONS_Y_SIZE / 2 + DIFFICAULTY_OFFSET + 5, 0.7, TEXT_COLOR);
	renderText("Type filename.", WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2, DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE + 5, 0.5f, TEXT_COLOR);
}

/******************************************************************************
Draws all of the things nessisary for the load prompt.
******************************************************************************/
void Window::renderLoad() {
	renderBasicOptionsWindow();

	renderText(fileName, WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2 + 2, HEIGHT_DEFAULT / 2 - DIFFICAULTY_OPTIONS_Y_SIZE / 2 + DIFFICAULTY_OFFSET + 5, 0.7, TEXT_COLOR);
	renderText("Type filename.", WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2, DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE + 5, 0.5f, TEXT_COLOR);

}

/******************************************************************************
Handles the active mouse requirments on the game.
******************************************************************************/
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (renderState) {
	case LOAD_STATE:
	case SAVE_STATE:
		if (action == GLFW_PRESS) {
			if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
				if (mods == GLFW_MOD_SHIFT) {
					fileName += char(key);
				}
				else {
					fileName += char(key + 32);
				}
			}
			else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
				if (mods != GLFW_MOD_SHIFT) {
					fileName += char(key);
				}
				else {
					switch (key) {
					case GLFW_KEY_1:
						fileName += '!';
						break;
					case GLFW_KEY_2:
						fileName += '@';
						break;
					case GLFW_KEY_3:
						fileName += '#';
						break;
					case GLFW_KEY_4:
						fileName += '$';
						break;
					case GLFW_KEY_5:
						fileName += '%';
						break;
					case GLFW_KEY_6:
						fileName += '^';
						break;
					case GLFW_KEY_7:
						fileName += '&';
						break;
					case GLFW_KEY_8:
						fileName += '*';
						break;
					case GLFW_KEY_9:
						fileName += '(';
						break;
					case GLFW_KEY_0:
						fileName += ')';
						break;
					}
				}
			}
			else if (key >= GLFW_KEY_APOSTROPHE && key <= GLFW_KEY_SLASH) {
				if (mods != GLFW_MOD_SHIFT) {
					fileName += char(key);
				}
				else {
					switch (key) {
					case GLFW_KEY_APOSTROPHE:
						fileName += '"';
						break;
					case GLFW_KEY_COMMA:
						fileName += '<';
						break;
					case GLFW_KEY_MINUS:
						fileName += '_';
						break;
					case GLFW_KEY_PERIOD:
						fileName += '>';
						break;
					case GLFW_KEY_SLASH:
						fileName += '?';
						break;
					}
				}
			}
			else {
				switch (key) {
				case GLFW_KEY_SPACE:
					fileName += ' ';
					break;
				case GLFW_KEY_BACKSPACE:
					fileName.pop_back();
					break;
				case GLFW_KEY_LEFT_SHIFT:
				case GLFW_KEY_RIGHT_SHIFT:
					break;
				case GLFW_KEY_ENTER:
					infoTemp.fileName = "Boards/" + fileName;
					sendInfo = true;
					renderState = BOARD_STATE;
					break;
				case GLFW_KEY_EQUAL:
					if (mods != GLFW_MOD_SHIFT) {
						fileName += char(key);
					}
					else {
						fileName += '+';
					}
					break;
				case GLFW_KEY_BACKSLASH:
					if (mods != GLFW_MOD_SHIFT) {
						fileName += char(key);
					}
					else {
						fileName += '|';
					}
					break;
				case GLFW_KEY_GRAVE_ACCENT:
					if (mods != GLFW_MOD_SHIFT) {
						fileName += char(key);
					}
					else {
						fileName += '~';
					}
					break;
				case GLFW_KEY_SEMICOLON:
					if (mods != GLFW_MOD_SHIFT) {
						fileName += char(key);
					}
					else {
						fileName += ':';
					}
					break;
				}
			}
		}
		break;
	}
}

/******************************************************************************
Handles the active mouse requirments on the game.
******************************************************************************/
void activeMouseCallback(GLFWwindow* window, double x, double y) {
	switch (renderState) {
	//case BOARD_STATE:
	//	break;
	case DIFFICAULTY_STATE:
		if (dragging) {
			if (x < WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2) {
				difficaulty = 0;
			}
			else if (x > WIDTH_DEFAULT / 2 + DIFFICAULTY_X_SIZE / 2) {
				difficaulty = 100;
			}
			else {
				difficaulty = (int)((x - (WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2)) / (DIFFICAULTY_X_SIZE / 100));
			}
		}
		break;
	//case LOAD_STATE:
	//	break;
	//case SAVE_STATE:
	//	break;
	}
}

/******************************************************************************
Is called when the mouse clicks.
******************************************************************************/
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	switch (renderState) {
	case BOARD_STATE:
		boardMouse(window, button, action, mods);
		break;
	case DIFFICAULTY_STATE:
		difficaultyMouse(window, button, action, mods);
		break;
	case LOAD_STATE:
		loadMouse(window, button, action, mods);
		break;
	case SAVE_STATE:
		saveMouse(window, button, action, mods);
		break;
	}
}

/******************************************************************************
Manages the mouse when the game is in the board state. Action is taken on the 
release and the press is a spot that you would take action.
******************************************************************************/
void boardMouse(GLFWwindow* window, int button, int action, int mods) {
static bool validClick = false;

double x, y;
glfwGetCursorPos(window, &x, &y);
y = HEIGHT_DEFAULT - y; //To make the bottom y = 0

if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
	if ((x > BOARD_LEFT && x < BOARD_LEFT + BOARD_X_SIZE &&
		y < BOARD_TOP && y > BOARD_TOP - BOARD_Y_SIZE)
		||
		(x > SELECT_LEFT && x < SELECT_LEFT + SELECT_X_SIZE &&
			y < SELECT_TOP && y > SELECT_TOP - SELECT_HEIGHT)
		||
		(x > OPTION_LEFT && x < OPTION_LEFT + OPTION_WIDTH &&
			y < BOARD_TOP && y > BOARD_TOP - (OPTION_HEIGHT * NUM_OPTIONS))
		||
		(x > OPTION_LEFT && x < OPTION_LEFT + OPTION_WIDTH &&
			y < BOARD_TOP - (OPTION_HEIGHT * NUM_OPTIONS) - MESSAGE_GAP &&
			y > BOARD_TOP - (OPTION_HEIGHT * (NUM_OPTIONS + 1)) - MESSAGE_GAP)) {
		validClick = true;
	}
	else {
		validClick = false;
	}
}
else if (validClick && action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
	//Cell selection
	if (x > BOARD_LEFT && x < BOARD_LEFT + BOARD_X_SIZE &&
		y < BOARD_TOP && y > BOARD_TOP - BOARD_Y_SIZE) {
		cell.setY((int)((x - BOARD_LEFT) / (BOARD_X_SIZE / 16)));
		cell.setX((int)((BOARD_TOP - y) / (BOARD_Y_SIZE / 16)));

		selected = &cell;
		sendInfo = false;
	}

	//Number selection
	else if (x > SELECT_LEFT && x < SELECT_LEFT + SELECT_X_SIZE &&
		y < SELECT_TOP && y > SELECT_TOP - SELECT_HEIGHT) {
		if (selected == NULL) {
			return;
		}

		//Creates the InteractInfo
		infoTemp.action = EDIT_SQU;
		infoTemp.cell = *selected;
		infoTemp.num = ((int)((x - SELECT_LEFT) / (SELECT_X_SIZE / 16)));
		sendInfo = true;
	}

	//Option selection
	else if (x > OPTION_LEFT && x < OPTION_LEFT + OPTION_WIDTH &&
		y < BOARD_TOP && y > BOARD_TOP - (OPTION_HEIGHT * NUM_OPTIONS)) {
		int option = ((int)((BOARD_TOP - y) / (OPTION_HEIGHT)));
		switch (option) {
		case 0:
			infoTemp.action = NEW_GAME;
			renderState = DIFFICAULTY_STATE;
			break;
		case 1:
			infoTemp.action = LOAD_GAME;
			renderState = LOAD_STATE;
			fileName = "";
			break;
		case 2:
			infoTemp.action = SAVE_GAME;
			renderState = SAVE_STATE;
			fileName = "";
			break;
		case 3:
			infoTemp.action = QUIT_GAME;
			sendInfo = true;
			break;
		}
		selected = NULL;
	}

	//Show possible
	else if (x > OPTION_LEFT && x < OPTION_LEFT + OPTION_WIDTH &&
		y < BOARD_TOP - (OPTION_HEIGHT * NUM_OPTIONS) - MESSAGE_GAP &&
		y > BOARD_TOP - (OPTION_HEIGHT * (NUM_OPTIONS + 1)) - MESSAGE_GAP) {
		highlightPossible = (highlightPossible ? false : true);
	}
	else {
		selected = NULL;
		sendInfo = false;
	}
}
}

/******************************************************************************
Manages the mouse when the game is in the difficaulty state. Action is taken
on the release.
******************************************************************************/
void difficaultyMouse(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = HEIGHT_DEFAULT - y; //To make the bottom y = 0
		if (y > HEIGHT_DEFAULT / 2 - DIFFICAULTY_Y_SIZE / 2 + DIFFICAULTY_OFFSET &&
			y < HEIGHT_DEFAULT / 2 + DIFFICAULTY_Y_SIZE / 2 + DIFFICAULTY_OFFSET &&
			x > WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2 + ((DIFFICAULTY_X_SIZE - DIFFICAULTY_SLIDER_SIZE) / 100) * difficaulty - DIFFICAULTY_SLIDER_SIZE / 2 + DIFFICAULTY_SLIDER_OFFSET &&
			x < WIDTH_DEFAULT / 2 - DIFFICAULTY_X_SIZE / 2 + ((DIFFICAULTY_X_SIZE - DIFFICAULTY_SLIDER_SIZE) / 100) * difficaulty + DIFFICAULTY_SLIDER_SIZE / 2 + DIFFICAULTY_SLIDER_OFFSET) {
			dragging = true;
		}
	}
	else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = HEIGHT_DEFAULT - y; //To make the bottom y = 0
		
		if (dragging) {
			dragging = false;
			return;
		}
		//OK
		if (x > DIFFICAULTY_OPTIONS_LEFT && x < DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE / 2 &&
			y < DIFFICAULTY_OPTIONS_TOP && y > DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE) {
			renderState = BOARD_STATE;
			infoTemp.num = difficaulty;
			sendInfo = true;
		}
		//Cancel
		else if (x > DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE / 2 && x < DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE &&
			y < DIFFICAULTY_OPTIONS_TOP && y > DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE) {
			renderState = BOARD_STATE;
			sendInfo = false;
		}
	}

}

/******************************************************************************
Manages the mouse when the game is in the save state. Action is taken
on the release.
******************************************************************************/
void saveMouse(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = HEIGHT_DEFAULT - y; //To make the bottom y = 0

		if (x > DIFFICAULTY_OPTIONS_LEFT && x < DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE / 2 &&
			y < DIFFICAULTY_OPTIONS_TOP && y > DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE) {
			infoTemp.fileName = "Boards/" + fileName;
			sendInfo = true;
			renderState = BOARD_STATE;
		}
		//Cancel
		else if (x > DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE / 2 && x < DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE &&
			y < DIFFICAULTY_OPTIONS_TOP && y > DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE) {
			renderState = BOARD_STATE;
			sendInfo = false;
		}
	}
}

/******************************************************************************
Manages the mouse when the game is in the load state. Action is taken
on the release.
******************************************************************************/
void loadMouse(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = HEIGHT_DEFAULT - y; //To make the bottom y = 0

		if (x > DIFFICAULTY_OPTIONS_LEFT && x < DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE / 2 &&
			y < DIFFICAULTY_OPTIONS_TOP && y > DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE) {
			infoTemp.fileName = "Boards/" + fileName;
			sendInfo = true;
			renderState = BOARD_STATE;
		}
		//Cancel
		else if (x > DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE / 2 && x < DIFFICAULTY_OPTIONS_LEFT + DIFFICAULTY_OPTIONS_X_SIZE &&
			y < DIFFICAULTY_OPTIONS_TOP && y > DIFFICAULTY_OPTIONS_TOP - DIFFICAULTY_OPTIONS_Y_SIZE) {
			renderState = BOARD_STATE;
			sendInfo = false;
		}
	}
}