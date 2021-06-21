#ifndef TableMounter_h
#define TableMounter_h

#include "../common/common.h"
#include "UnitMounter.h"



#include <vector>

//#define altura 10
#define altura 20
class TableMounter2D{

  bool table[altura][9];
  public:
  bool insert(UnitMounter2D *unit);
  TableMounter2D();
  bool collide(UnitMounter2D *unit);
  void consoleDraw();
  void glDraw();
  //retorna todas as posições e linhas que foram quebradas
  int boomCount(std::vector<vec3> *posicoes,std::vector<vec3> *linhas);
};

#endif
