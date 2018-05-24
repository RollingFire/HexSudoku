#ifndef POSITION_H
#define POSITION_H

#include <string>

class Position {
private:
   int x;
   int y;
public:
   Position();
   Position(int, int);
   int getX();
   void setX(int) throw (std::string);
   void setX(char) throw (std::string);
   int getY();
   void setY(int) throw (std::string);
   void setY(char) throw (std::string);
};

Position getBlock(Position);

#endif
