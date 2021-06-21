#include "Tetris.h"

#include <stdio.h>
#include <stdlib.h>


bool TableMounter2D::insert(UnitMounter2D *unit) {
    for (int y = 0; y < altura; y++) {
        for (int x = -4; x <= 4; x++) {
            if (unit->colide(AABB(vec2(x, y) - vec2(0.4), vec2(x, y) + vec2(0.4)))) {
                table[y][x + 4] = true;
            }
        }
    }
    bool ret = false;
    if (unit->someBlockDontColide(AABB(vec2(-4.4, -0.4), vec2(4.4, (float)altura + 0.4 - 1.0)))) {
        ret = true;
    }
    unit->turnOffDontCollided(AABB(vec2(-4.4, -0.4), vec2(4.4, (float)altura + 0.4 - 1.0)));
    return ret;
}
TableMounter2D::TableMounter2D() {
    for (int i = 0; i < altura; i++)
        for (int j = 0; j < 9; j++)
            table[i][j] = false;
}
bool TableMounter2D::collide(UnitMounter2D *unit) {
    for (int y = 0; y < altura; y++) {
        for (int x = -4; x <= 4; x++) {
            if (table[y][x + 4]) {
                if (unit->colide(AABB(vec2(x, y) - vec2(0.4), vec2(x, y) + vec2(0.4)))) {
                    return true;
                }
            }
        }
    }
    //base test colision
    if (unit->colide(AABB(vec2(-5, -1) - vec2(0.4), vec2(5, -1) + vec2(0.4))) ||
        unit->colide(AABB(vec2(-5, -1) - vec2(0.4), vec2(-5, altura + 2) + vec2(0.4))) ||
        unit->colide(AABB(vec2(5, -1) - vec2(0.4), vec2(5, altura + 2) + vec2(0.4)))) {
        return true;
    }
    return false;
}
void TableMounter2D::consoleDraw() {
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < 9; j++)
            if (table[altura - 1 - i][j])
                printf("*");
            else
                printf(".");
        printf("\n");
    }
}
void TableMounter2D::glDraw() {
    AABB aabb;
    RenderSystem *render = RenderSystem::getSingleton();

    render->model.push();

    render->model.top *= translate(-4, 0, 0);

    //glTranslatef(-4, 0, 0);
    //glColor3f(1, 1, 1);

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < 9; x++) {
            if (table[y][x]) {
                aabb = AABB(vec2(x, y) - vec2(0.5), vec2(x, y) + vec2(0.5));// .cubeDraw();
                render->drawAABB_Cube(aabb, vec4(1));
            }
        }
    }

    //glPushAttrib(GL_LINE_BIT);
    float linew;
    glGetFloatv(GL_LINE_WIDTH, &linew);
    glLineWidth(1);

    //glColor3f(0.4, 1, 1);
    aabb = AABB(vec2(-0.5, -0.5), vec2(9.5 - 1.0, (float)altura + 0.5 - 1.0));// .draw();
    render->drawAABB_Lines(aabb, vec4(0.4, 1, 1, 1));

    glLineWidth(3);
    //glColor4f(0, 0, 0, 0.5);
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < 9; x++) {
            if (table[y][x]) {
                aabb = AABB(vec2(x, y) - vec2(0.5), vec2(x, y) + vec2(0.5));// .draw();
                render->drawAABB_Lines(aabb, vec4(0, 0, 0, 0.5));
            }
        }
    }

    //glColor3f(1, 1, 1);
    //glPopAttrib();
    glLineWidth(linew);
    /*
        for(int y=0;y<altura;y++){
          for(int x=0;x<9;x++){
            if(!table[y][x]){
              glPushAttrib(GL_LINE_BIT);
              glLineWidth(1);
              glColor3f(0.4,1,1);
              AABB(vec2(x,y)-vec2(0.5), vec2(x,y)+vec2(0.5)).draw();
              glColor3f(1,1,1);
              glPopAttrib();
            }
          }
        }//*/
    //glTranslatef(4, 0, 0);

    render->model.pop();
}
int TableMounter2D::boomCount(std::vector<vec3> *posicoes, std::vector<vec3> *linhas) {
    for (int y = 0; y < altura; y++) {
        bool filled = true;
        for (int x = 0; x < 9; x++) {
            filled = filled && table[y][x];
        }
        if (filled) {
            linhas->push_back(vec3(0, y, 0));
            for (int x = 0; x < 9; x++) {
                posicoes->push_back(vec3(x - 4, y, 0));
            }
        }
    }

    int retorno = 0;
    for (int artura = 0; artura < altura; artura++) {
        bool filled = true;
        for (int x = 0; x < 9; x++) {
            filled = filled && table[artura][x];
        }
        while (filled) {
            retorno++;
            for (int y = artura; y < altura - 1; y++) {
                for (int x = 0; x < 9; x++) {
                    table[y][x] = table[y + 1][x];
                }
            }
            for (int x = 0; x < 9; x++) {
                table[altura - 1][x] = false;
                filled = filled && table[artura][x];
            }
        }
    }
    return retorno;
}
//----------------------------------------------------------------------------------------------------

