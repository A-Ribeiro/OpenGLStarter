#include "Tetris.h"

#include <stdio.h>
#include <stdlib.h>


void Tetris::drawMovableCubes(const unsigned int time_interval) {
    if (timeComformant <= 0)
        timeComformant = 0;
    timeComformant += (int)time_interval;
    const int interval = 25;
    if ((float)timeComformant / (float)interval >= 50) timeComformant = 0;
    //fazer todos os passos de simul
    while (timeComformant >= interval) {
        //printf("\n\nTICK SIMULATION!!!!\n\n");
        int size = movableCubes.size();
        for (int i = 0; i < size; i++) {
            movableCube cube = movableCubes.front();
            movableCubes.pop();
            cube.position = cube.position + cube.force;
            cube.lifetime--;
            if (cube.lifetime > 0) {
                movableCubes.push(cube);
            }
        }

        size = flashs.size();
        for (int i = 0; i < size; i++) {
            HorizontalFlashLight flash = flashs.front();
            flashs.pop();
            flash.lifetime--;
            if (flash.lifetime > 0) {
                flashs.push(flash);
            }
        }

        timeComformant -= interval;
    }


    //desenhar tudo com lerp
    float lerpThisTime = (float)timeComformant / (float)interval;
    int size;


    AABB aabb;
    RenderSystem *render = RenderSystem::getSingleton();


    //draw flashs
    //glPushAttrib(GL_ENABLE_BIT);
    GLboolean depthTest;
    glGetBooleanv (GL_DEPTH_TEST, &depthTest);

    if (depthTest)
    glDisable(GL_DEPTH_TEST);

    // additive blend
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    size = flashs.size();
    for (int i = 0; i < size; i++) {
        HorizontalFlashLight flash = flashs.front();
        flashs.pop();
        //printf("(%.2f,%.2f,%.2f)",cube.position.x,cube.position.y,cube.position.z);

        const float z = 1;
        const float zinc = 0;
        const float alpha = 0.3;
        float scale = 0.5 + 0.5*((float)flash.totalLifetime - ((float)flash.lifetime + lerpThisTime)) / (float)flash.totalLifetime;
        scale *= scale * scale;


        /*
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(alpha, alpha, 0, alpha);
        glVertex3f(0, flash.height, z);
        glColor4f(0, 0, 0, alpha);
        glVertex3f(0 + 20 * scale, flash.height - 2 * scale, z + zinc);
        glVertex3f(0 + 20 * scale, flash.height + 2 * scale, z + zinc);
        glVertex3f(0 - 20 * scale, flash.height + 2 * scale, z + zinc);
        glVertex3f(0 - 20 * scale, flash.height - 2 * scale, z + zinc);
        glVertex3f(0 + 20 * scale, flash.height - 2 * scale, z + zinc);
        glEnd();
        */

        vec3 vpos[] = {
            vec3(0, flash.height, z),
            vec3(0 + 20 * scale, flash.height - 2 * scale, z + zinc),
            vec3(0 + 20 * scale, flash.height + 2 * scale, z + zinc),
            vec3(0 - 20 * scale, flash.height + 2 * scale, z + zinc),
            vec3(0 - 20 * scale, flash.height - 2 * scale, z + zinc),
            vec3(0 + 20 * scale, flash.height - 2 * scale, z + zinc)
        };

        vec4 vcolor[] = {
            vec4(alpha, alpha, 0, alpha),
            vec4(0, 0, 0, alpha),
            vec4(0, 0, 0, alpha),
            vec4(0, 0, 0, alpha),
            vec4(0, 0, 0, alpha),
            vec4(0, 0, 0, alpha)
        };

        render->drawColor(GL_TRIANGLE_FAN, vpos, vcolor, 6);

        flashs.push(flash);
    }

    //back normal transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (depthTest)
        glEnable(GL_DEPTH_TEST);
    //glPopAttrib();



    //glColor4f(0.5, 0.5, 0.5, 0.5);


    size = movableCubes.size();
    for (int i = 0; i < size; i++) {
        movableCube cube = movableCubes.front();
        movableCubes.pop();
        //printf("(%.2f,%.2f,%.2f)",cube.position.x,cube.position.y,cube.position.z);
        aabb = AABB(cube.position + lerpThisTime * cube.force + vec3(-0.5),
            cube.position + lerpThisTime * cube.force + vec3(0.5));// .draw();

        render->drawAABB_Lines(aabb, vec4(0.5, 0.5, 0.5, 0.5));

        movableCubes.push(cube);
    }



    //if(size>0) printf("\n");


}

void Tetris::addExplosionEffect(const std::vector<vec3> &positions, const std::vector<vec3> &linhas) {

    vec3 zeroPos = (extractTranslation(oglM) + toVec3(oglM*toPtn4(vec3(0, (float)altura*0.5f, 0))))*-1;
    //zeroPos = normalize(trans*-1);
    //DEBUG_PRINT("[Tetris]: addExplosionEffect");
    //struct movableCube
    // vec3 force;
    // vec3 position;
    //   unsigned int lifetime;
    movableCube cube;
    //cube.force = vec3(0,0,1);
    cube.lifetime = 100;
    for (unsigned int i = 0; i < positions.size(); i++) {
        cube.position = positions[i];
        //vec3 vecToView = normalize(zeroPos - toVec3(oglM*toPtn4(cube.position)));
        cube.force = normalize(zeroPos) * 2 + normalize(toVec3(oglM*toPtn4((cube.position*vec3(10, 8.0f / (float)(altura - 1), 0) + vec3(0, -4.0f, 0)))));
        //cube.force = 0.2*normalize(zeroPos*5 + toVec3(oglM*toPtn4(normalize(cube.position*vec3(1,8.0f/(float)(altura-1),0) +  vec3(0,-4.0f,0)))));
        //cube.force = normalize(cube.position*vec3(1,8.0f/(float)(altura-1),0) +
  //                             vec3(0,-4.0f,10))*vec3(1,1,0.5);
        movableCubes.push(cube);
    }

    HorizontalFlashLight flash;
    flash.lifetime = 10;
    flash.totalLifetime = 10;
    for (unsigned int i = 0; i < linhas.size(); i++) {
        flash.height = linhas[i].y;//(oglM*toPtn4(linhas[i])).y;
        flashs.push(flash);
    }
}

int Tetris::selectRandomUnit() {
    return Random::getRange(0,5);
}

Tetris::Tetris(AlMultiSource *whoosh,
    AlMultiSource *plink,
    AlMultiSource *boom) :GenericSimulation(1000, false) {

    x2 = GLTexture::loadFromFile("resources/images/x2.png");
    x3 = GLTexture::loadFromFile("resources/images/x3.png");
    x4 = GLTexture::loadFromFile("resources/images/x4.png");

    timeComformant = 0;
    this->whoosh = whoosh;
    this->plink = plink;
    //    this->speed = speed;
    this->boom = boom;

    lvlInterval = getSimulationInterval_ms();
    units[0] = &u1; units[1] = &u2; units[2] = &u3; units[3] = &u4; units[4] = &u5; units[5] = &u6;
    unitsToShow[0] = &su1;
    unitsToShow[1] = &su2;
    unitsToShow[2] = &su3;
    unitsToShow[3] = &su4;
    unitsToShow[4] = &su5;
    unitsToShow[5] = &su6;
    currentUnit = NULL;
    endGame = false;
    for (int i = 0; i < AllBtnDefs; i++) {
        keyState[i] = false;
    }
    nextUnit = selectRandomUnit();
    score = 0;
    countdownToShowX2 = -1;
    countdownToShowX3 = -1;
    countdownToShowX4 = -1;

    /*
      configure the main matrix
    */
    vec3 eye = vec3(-0.3, 0.3 - 0.2, 5),
        object = vec3(0, 0, 0);
    oglM =
        lookAt(object - eye,//lookTo
            vec3(0, 1, 0),
            eye);
    const float porcentagem = 1.5;
    //glScalef(2.0f/9.0f,2.0f/(float)altura,1);

    oglM = oglM * scale(2.0f / (float)altura, 2.0f / (float)altura, 2.0f / (float)altura);
    oglM = oglM * translate(0, -(float)altura*0.5f + 0.5f, 0);

    oglM = oglM * translate(0, (float)altura*0.5f, 0);
    oglM = oglM * scale(porcentagem, porcentagem, porcentagem);
    oglM = oglM * translate(0, -(float)altura*0.5f, 0);
    //*/
}

Tetris::~Tetris() {

    setNullAndDelete(x2);
    setNullAndDelete(x3);
    setNullAndDelete(x4);

}
//--------------------------------------------------------------------------------------------------------
void Tetris::processSimulation(const unsigned int counter, const unsigned int time_ms) {
    if (endGame) return;
    //   printf("Process(%d)!!!!!!!!!\n",counter);
    if (currentUnit == NULL) {
        int selected = nextUnit;
        currentUnit = units[selected];
        currentUnit->posx = 0;
        currentUnit->posy = altura + 2;
        nextUnit = selectRandomUnit();
    }
    //plink->play();
    //printf("tick!!!!count = %u, time = %u\n",counter,time_ms);
    //verify colision By Left or Right movement
    //verify colision to block
    currentUnit->posy--;
    if (table.collide(currentUnit)) {
        currentUnit->posy++;
        if (table.insert(currentUnit)) {
            //verify end Game- after movement, one of part of Uni is out of scene
            endGame = true;
            /*verifica se tem alguma coisa pra quebrar ainda*/
            std::vector<vec3> pos, linhas;
            int count = table.boomCount(&pos, &linhas);
            addExplosionEffect(pos, linhas);
            if ((currentUnit->getBlocks() > 0) &&
                count > 0) {
                //         DEBUG_PRINT("[TETRIS]DOWN!!!!");
                do {
                    currentUnit->posy--;
                    //           printf("yPos %d\n",currentUnit->posy);
                } while (!table.collide(currentUnit));
                currentUnit->posy++;
                //         printf("VerifyInsert\n");
                if (!table.insert(currentUnit)) {
                    //           printf("Nao fim de jogo\n");
                               //não é fim de jogo!!!!!
                    score += 300 * count*count;
                    endGame = false;
                    currentUnit->backToNormal();
                    currentUnit = NULL;
                    plink->play();
                    //play da voz WAZAAAAAAAAAAAa
                }
            }
            else
                currentUnit->backToNormal();
        }
        else {
            score += 15;
            plink->play();
            currentUnit = NULL;
            /*
              Verificar se alguma coisa vai quebrar na table
            */
            std::vector<vec3> pos, linhas;
            int count = table.boomCount(&pos, &linhas);
            addExplosionEffect(pos, linhas);
            if (count > 0) {
                switch (count) {
                case 2:
                    countdownToShowX2 = 1000;//1 segundos
                    break;
                case 3:
                    countdownToShowX3 = 1000;//1 segundos
                    break;
                case 4:
                    countdownToShowX4 = 1000;//1 segundos
                    break;
                }
                score += 150 * count*count;
                boom->play();
            }
        }
        //     table.consoleDraw();
             //if(keyState[Down]){
        setSimulationInterval_ms(lvlInterval);
        keyState[Down] = false;
        //       speed->stop();
             //}
    }
    if (score < 10000) {//lvl1
        if (lvlInterval != 1000) {
            lvlInterval = 1000;
            setSimulationInterval_ms(lvlInterval);
        }
    }
    else if (score < 20000) {//lvl2
        if (lvlInterval != 500) {
            lvlInterval = 500;
            setSimulationInterval_ms(lvlInterval);
        }
    }
    else if (score < 30000) {//lvl3
        if (lvlInterval != 300) {
            lvlInterval = 300;
            setSimulationInterval_ms(lvlInterval);
        }
    }
    else if (score < 40000) {//lvl4
        if (lvlInterval != 100) {
            lvlInterval = 100;
            setSimulationInterval_ms(lvlInterval);
        }
    }
}
//---------------------------------------------------------------------------------------------------------
void Tetris::player1PushBtn(const BtnDefs btn) {
    if (currentUnit == NULL || endGame) return;
    switch (btn) {
    case Left:
        currentUnit->posx++;
        if (table.collide(currentUnit))
            currentUnit->posx--;
        break;
    case Right:
        currentUnit->posx--;
        if (table.collide(currentUnit))
            currentUnit->posx++;
        break;
    case Rotate:
        currentUnit->rotate();
        if (table.collide(currentUnit)) {
            currentUnit->rotate(); currentUnit->rotate(); currentUnit->rotate();
        }
        else
            whoosh->play();
        break;
    case Down:
        //        if(keyState[Down] == false){
                  //setSimulationInterval_ms(lvlInterval/5);
                  //DEBUG_PRINT("[TETRIS] MAKE DOWN!!!!");
                  //setSimulationInterval_ms(50);
        setSimulationInterval_ms(25);
        keyState[Down] = true;
        //          speed->play();
        //        }
    case AllBtnDefs:
        break;
    }
}
void Tetris::player1ReleaseBtn(const BtnDefs btn) {
    if (btn == Down) {
        //if(keyState[Down]){
          //DEBUG_PRINT("[TETRIS] MAKE UP!!!!");
        setSimulationInterval_ms(lvlInterval);
        keyState[Down] = false;
        //        speed->stop();
              //}
    }
}
void Tetris::render(const unsigned int time_ms, const vec2 &screenCenter) {

    RenderSystem *render = RenderSystem::getSingleton();


    //make simulation before render
    makeSimulation(time_ms);
    //mat4 tmp = getModelviewFromGL();
    //glLoadMatrixf(oglM.array);

    render->model.top = oglM;

    /*
        const float porcentagem = 1.5;
        //glScalef(2.0f/9.0f,2.0f/(float)altura,1);
        glScalef(2.0f/(float)altura,2.0f/(float)altura,2.0f/(float)altura);
        glTranslatef(0,-(float)altura*0.5f+0.5f,0);

        glTranslatef(0,(float)altura*0.5f,0);
        glScalef(porcentagem,porcentagem,porcentagem);
        glTranslatef(0,-(float)altura*0.5f,0);

        //*/

    table.glDraw();


    render->model.push();

    render->model.top =
        translate(5, -3, -15)*
        scale(0.4, 0.4, 0.2)*
        rotate( DEG2RAD(30.0f), 1, 0, 0)*
        rotate( DEG2RAD(30.0f), 0, 1, 0)*
        scale(1, 1, 0.8);

    /*
    glColor3f(1, 1, 1);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(5, -3, -15);
    glScalef(0.4, 0.4, 0.2);
    glRotatef(30.0f, 1, 0, 0);
    glRotatef(30.0f, 0, 1, 0);
    glScalef(1, 1, 0.8);
    */

    unitsToShow[nextUnit]->glDraw();

    render->model.pop();

    //glPopMatrix();


    if (currentUnit != NULL) {
        if (!endGame) {
            float lerp = -lerpCalc(time_ms) + 1;
            //glTranslatef(0, lerp, 0);

            render->model.push();
            render->model.top *= translate(0, lerp, 0);

            currentUnit->glDraw();

            render->model.pop();

            //glTranslatef(0, -lerp, 0);
            //world = transpose(tmp)*getModelviewFromGL();
        }
        else {
            currentUnit->glDraw();
        }
    }
    //draw X point in middle of screen
    unsigned int diffRender = diffSystime(time_ms, lastRender);
    drawMovableCubes(diffRender);//uhhuhhu
    lastRender = time_ms;
    bool x2b = countdownToShowX2 > 0,
        x3b = countdownToShowX3 > 0,
        x4b = countdownToShowX4 > 0;
    bool any = x2b || x3b || x4b;
    if (any) {
        //glMatrixMode(GL_PROJECTION);   glPushMatrix();   glLoadIdentity();
        //glMatrixMode(GL_MODELVIEW);

        render->projection.push();
        render->projection.top = mat4::IdentityMatrix;

        render->model.top =
            translate(-1, -1, 0) *
            scale(1.0f / screenCenter.x, 1.0f / screenCenter.y, 1);

        //glPushAttrib(GL_ENABLE_BIT);
        GLboolean depthTest;
        glGetBooleanv (GL_DEPTH_TEST, &depthTest);

        if (depthTest)
        glDisable(GL_DEPTH_TEST);

        if (countdownToShowX2 > 0) {

            //x2.centerDraw(screenCenter);
            render->drawTexture_center(x2, screenCenter);
            countdownToShowX2 -= (int)diffRender;
        }
        if (countdownToShowX3 > 0) {
            //x3.centerDraw(screenCenter);
            render->drawTexture_center(x3, screenCenter);
            countdownToShowX3 -= (int)diffRender;
        }
        if (countdownToShowX4 > 0) {
            //x4.centerDraw(screenCenter);
            render->drawTexture_center(x4, screenCenter);
            countdownToShowX4 -= (int)diffRender;
        }

        if (depthTest)
            glEnable(GL_DEPTH_TEST);

        //glPopAttrib();

        render->projection.pop();
        //glMatrixMode(GL_PROJECTION);   glPopMatrix();
        //glMatrixMode(GL_MODELVIEW);
    }

}

bool Tetris::isGameEnd() {
    return endGame;
}
bool Tetris::isPause() {
    return getPausedSimulation();
}
void Tetris::pause() {
    setPausedSimulation(!getPausedSimulation());
}

int Tetris::getScore() {
    return score;
}
vec3 Tetris::getViewPos() {
    if (currentUnit) {
        vec3 p = currentUnit->getWorldPos();
        p = toVec3(world*vec4(p.x, p.y, p.z, 1));
        return p;
    }
    else
        return vec3(0);
}
/*
int main(){
  Tetris te;
  te.setLastTimeSimulation_ms(getSystemTime_ms());
  while(true){
    te.makeSimulation(getSystemTime_ms());
  }
  return 0;
}
*/
