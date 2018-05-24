#ifndef GUI_H
#define GUI_H

// Std. Includes
#include <iostream>
#include <map>
#include <string>
#include <vector>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
// GL includes
#include "Shader.h"

#include "HexBoard.h"
#include "Position.h"


//Window size.
#define WIDTH_DEFAULT 1280
#define HEIGHT_DEFAULT 720

//Convert from pixxel to f types.
#define TO_F_X(x) ((2*x/WIDTH_DEFAULT)-1)
#define TO_F_Y(x) ((2*x/HEIGHT_DEFAULT)-1)
#define TO_F_X_DIS(x) (2*x/WIDTH_DEFAULT)
#define TO_F_Y_DIS(x) (2*x/HEIGHT_DEFAULT)

//Board points pixxel.
#define BOARD_TOP (GLfloat)660 
#define BOARD_LEFT (GLfloat)190 
#define BOARD_X_SIZE (GLfloat)520
#define BOARD_Y_SIZE (GLfloat)520
#define SELECT_GAP (GLfloat)10
#define SELECT_TOP (GLfloat)(BOARD_TOP-BOARD_Y_SIZE-SELECT_GAP)
#define SELECT_X_SIZE (GLfloat)600
#define SELECT_LEFT (GLfloat)(BOARD_LEFT-((SELECT_X_SIZE-BOARD_X_SIZE)/2))
#define SELECT_HEIGHT (GLfloat)(SELECT_X_SIZE/16)
#define OPTION_GAP (GLfloat)100 
#define OPTION_WIDTH (GLfloat)256 
#define OPTION_HEIGHT (GLfloat)35
#define OPTION_LEFT (GLfloat)(BOARD_LEFT + BOARD_X_SIZE + OPTION_GAP)
#define MESSAGE_GAP (GLfloat)20
//  Difficaulty.
#define DIFFICAULTY_WINDOW_X_SIZE (GLfloat)560
#define DIFFICAULTY_WINDOW_Y_SIZE (GLfloat)180
#define DIFFICAULTY_OPTIONS_TOP (GLfloat)340
#define DIFFICAULTY_OPTIONS_LEFT (GLfloat)670
#define DIFFICAULTY_OPTIONS_X_SIZE (GLfloat)200
#define DIFFICAULTY_OPTIONS_Y_SIZE (GLfloat)35
#define DIFFICAULTY_X_SIZE (GLfloat)420
#define DIFFICAULTY_Y_SIZE (GLfloat)40
#define DIFFICAULTY_OFFSET (GLfloat)25
#define DIFFICAULTY_SLIDER_SIZE (GLfloat) 20
#define DIFFICAULTY_SLIDER_OFFSET (GLfloat) 10

//Board points -1 - 1.
#define BOARD_TOP_F TO_F_Y(BOARD_TOP)
#define BOARD_LEFT_F TO_F_X(BOARD_LEFT)
#define BOARD_X_SIZE_F TO_F_X_DIS(BOARD_X_SIZE)
#define BOARD_Y_SIZE_F TO_F_Y_DIS(BOARD_Y_SIZE)
#define SELECT_GAP_F TO_F_Y_DIS(SELECT_GAP)
#define SELECT_TOP_F TO_F_Y(SELECT_TOP)
#define SELECT_X_SIZE_F TO_F_X_DIS(SELECT_X_SIZE)
#define SELECT_LEFT_F TO_F_X(SELECT_LEFT)
#define SELECT_HEIGHT_F TO_F_Y_DIS(SELECT_HEIGHT)
#define OPTION_GAP_F TO_F_X_DIS(OPTION_GAP)
#define OPTION_WIDTH_F TO_F_X_DIS(OPTION_WIDTH)
#define OPTION_HEIGHT_F TO_F_Y_DIS(OPTION_HEIGHT)
#define OPTION_LEFT_F TO_F_X(OPTION_LEFT)
#define MESSAGE_GAP_F TO_F_Y_DIS(MESSAGE_GAP)
//  Difficaulty.
#define DIFFICAULTY_WINDOW_X_SIZE_F TO_F_X_DIS(DIFFICAULTY_WINDOW_X_SIZE)
#define DIFFICAULTY_WINDOW_Y_SIZE_F TO_F_Y_DIS(DIFFICAULTY_WINDOW_Y_SIZE)
#define DIFFICAULTY_OPTIONS_TOP_F TO_F_Y(DIFFICAULTY_OPTIONS_TOP)
#define DIFFICAULTY_OPTIONS_LEFT_F TO_F_X(DIFFICAULTY_OPTIONS_LEFT)
#define DIFFICAULTY_OPTIONS_X_SIZE_F TO_F_X_DIS(DIFFICAULTY_OPTIONS_X_SIZE)
#define DIFFICAULTY_OPTIONS_Y_SIZE_F TO_F_Y_DIS(DIFFICAULTY_OPTIONS_Y_SIZE)
#define DIFFICAULTY_X_SIZE_F TO_F_X_DIS(DIFFICAULTY_X_SIZE)
#define DIFFICAULTY_Y_SIZE_F TO_F_Y_DIS(DIFFICAULTY_Y_SIZE)
#define DIFFICAULTY_OFFSET_F TO_F_Y_DIS(DIFFICAULTY_OFFSET)
#define DIFFICAULTY_SLIDER_SIZE_F TO_F_X_DIS(DIFFICAULTY_SLIDER_SIZE)
#define DIFFICAULTY_SLIDER_OFFSET_F TO_F_X_DIS(DIFFICAULTY_SLIDER_OFFSET)

//Render states
#define BOARD_STATE 1
#define DIFFICAULTY_STATE 2
#define LOAD_STATE 3
#define SAVE_STATE 4


struct Character {
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint Advance;
};

struct InteractInfo {
	int action;
	Position cell;
	int num;
	std::string fileName;
};

class Window {
public:
	GLFWwindow * window;

	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;
	InteractInfo info;
	InteractInfo* infoP;

	glm::mat4 projection;
	Shader* defaultShader;
	GLuint defaultMatrixID;
	Shader* textShader;
	GLuint textMatrixID;

public:
	Window(std::vector<std::string>* OPTIONS);
	~Window();
	void render(Board* puzzle, std::vector<std::string>* MESSAGES);
private:
	void getCharacters();
	void drawLines();
	void drawText(Board* puzzle, std::vector<std::string>* MESSAGES);
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	void drawSelected(Board* puzzle);
	void renderBasicOptionsWindow();
	void renderDifficaulty();
	void renderSave();
	void renderLoad();
};

#endif
