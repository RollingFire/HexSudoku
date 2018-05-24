#ifndef HEX_BOARD_H
#define HEX_BOARD_H

#include <string>
#include "Position.h"

struct Tile {
   int num;
   bool possible[16];
   bool original;
};

class Board {
private:
   Tile puzzle[16][16];
public:
   Board();
   ~Board() { delete[] puzzle; }
   void reset();
   void updatePossible(int, int, Position);
   int getNumPossible(Position);
   bool isValid(int, Position);
   void editSqu(int, Position) throw (std::string);
   void editSqu(int, Position, bool) throw (std::string);
   void editSqu(Position, bool);
   const Tile* getCell(int x, int y);
   const Tile* getCell(Position);
};

#endif
