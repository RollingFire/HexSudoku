#ifndef File_H
#define File_H

#include "HexBoard.h"
#include <string>

void loadNew(Board*);
void loadSave(Board*);
void saveFile(Board*);
void readFile(Board*, std::string) throw (std::string);
void writeFile(Board*, std::string) throw (std::string);

#endif
