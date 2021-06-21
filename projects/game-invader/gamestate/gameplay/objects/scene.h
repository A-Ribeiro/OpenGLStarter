
#ifndef scene_h
#define scene_h

//#include "../../../../../common/common.h"

//#include "../../../OpenALCalls.h"

//#include "../../../Font3D.h"

//#include "../colision/AABB.h"//AABB
//#include "../../../math/all.h" //vec2

#include "../objects/object.h" //vec2

//#include "../../../OglStateHelper.h"

//#include "../../../menuResource/allImgs.hpp"
//#include "../../../textureHelper.h"

#include "../../../SpaceInvader.h"

extern SpaceInvader *invaderref;
extern GLTexture *textureref;

extern bool canGenerateShoot;
//vec4 colorref(1.0f);

//#include "../../../OpenAL/OpenALCalls.h"

/*
extern GLuint xplodeTex[17],
              tiro,
              spaceship,
              enemy;



  static void texturedDraw(const AABB&aabb){
    glBegin(GL_QUADS);
      glTexCoord2f(0,1);  glVertex2f(aabb.IE.x,aabb.IE.y);
      glTexCoord2f(1,1);  glVertex2f(aabb.SD.x,aabb.IE.y);
      glTexCoord2f(1,0);  glVertex2f(aabb.SD.x,aabb.SD.y);
      glTexCoord2f(0,0);  glVertex2f(aabb.IE.x,aabb.SD.y);
    glEnd();
  }

static void loadXplodeTex(){
  static bool loaded = false;
  if (loaded) return;
  loaded = true;
  for (int i=0;i<17;i++){
    xplodeTex[i] = createTexture(256,256, xplod[i] );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  }
  tiro = createTexture(256,256, tiroImg );
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  spaceship = createTexture(256,256, spaceshipImg );
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  enemy = createTexture(256,256, enemyImg );
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
}

static void unloadXplodeTex(){
  for (int i=0;i<17;i++)
    freeTexture(xplodeTex[i]);
  freeTexture(tiro);
  freeTexture(spaceship);
  freeTexture(enemy);
}
*/


class Scene {
private:

    float globalLerp;
    bool globalLerpOn;
    bool invert_Velocity;
    float velocityFixWhenInverted;
    ObjectList *listOfObject, *listToVerify;
    Object defaultSpaceship;//usado pra inserir mais vidas para o player

    //----------------------------------------------------------------------------
    static bool countXplosion(void* param, Object *object) {
        Scene *scene = (Scene*)param;
        object->counter++;
        if (object->counter > 16) {
            scene->listOfObject->remove(object);
        }
        return true;
    }
    //----------------------------------------------------------------------------
    static bool verifyLimitAndRemoveIfPasses(void* param, Object *object) {
        Scene *scene = (Scene*)param;

        if (!AABB::aabbOverlapsAABB(object->aabb, scene->aabb)) {
            scene->listOfObject->remove(object);
        }
        return true;
    }
    //----------------------------------------------------------------------------
    static bool activeObject(void* param, Object *object) {
        Scene *scene = (Scene*)param;
        object->action = OS_Nothing;
        object->active = true;
        object->velocity = scene->enemy_velocity;
        return true;
    }
    //----------------------------------------------------------------------------
    static bool drawObject(void* param, Object *object) {
        if (!object->active && object->action != OS_GotoFormationOrigin) return true;
        switch (object->type) {
        case OT_Xplosion:
            textureref = invaderref->resources->xplod[object->counter];

            //activeTexture(xplodeTex[object->counter]);
        case OT_EnemyShipShoot:
        case OT_SpaceShipShoot:
        case OT_Enemy:
        case OT_SpaceShip:

            invaderref->render->drawTexture(textureref, object->aabb);

            //texturedDraw(object->aabb);
            break;
        default:

            //object->aabb.draw();
            invaderref->render->drawAABB(object->aabb, vec4(1.0f));

        }

        return true;
    }
    //----------------------------------------------------------------------------
    static bool printObjectContent(void* param, Object *object) {
        printObjectState(*object);
        return true;
    }
    //----------------------------------------------------------------------------    
    static bool calculatePosition(void* param, Object *object) {
        if (!object->active && object->action != OS_GotoFormationOrigin) return true;
        Scene *scene = (Scene*)param;

        if (object->type == OT_SpaceShip) {
            vec2 newPos = object->position + object->velocity * invaderref->time.deltaTime * 30.0f;
            if (AABB::pointInsideAABB(newPos, scene->aabb)) {
                object->position = newPos;
            }
            else {
                object->velocity = vec2(0);
                if (absv(newPos.x - scene->aabb.min_box.x) <
                    absv(newPos.x - scene->aabb.max_box.x)) {
                    newPos.x = scene->aabb.min_box.x;
                }
                else {
                    newPos.x = scene->aabb.max_box.x;
                }
                object->position = newPos;
            }
        }
        else {
            if (scene->globalLerpOn || object->action == OS_GotoFormationOrigin) {
                object->position = lerp(object->formation.origin,
                    object->formation.toMove,
                    scene->globalLerp);
            }
            else {
                object->position = object->position + object->velocity * invaderref->time.deltaTime * 30.0f;
                if (object->type == OT_Enemy) {
                    if (object->aabb.min_box.y < scene->aabb.min_box.y) {
                        scene->killAllSpaceShip();
                    }

                    if (canGenerateShoot) {
                        int rnd = 1 + (int)(1000.0*rand() / (RAND_MAX + 1.0));
                        if (rnd > 10 && rnd < scene->fatorNumeroTiro/*15*/) {
                            scene->enemyShoot(object->position);
                        }
                    }
                }
            }
        }
        return true;
    }
    //----------------------------------------------------------------------------
    static bool calculateAABB(void* param, Object *object) {
        if (!object->active && object->action != OS_GotoFormationOrigin) return true;
        object->aabb = AABB(object->position - object->dimension / 2,
            object->position + object->dimension / 2);
        return true;
    }
    //----------------------------------------------------------------------------
    static bool verifyLimitToSetVelocity(void* param, Object *object) {
        if (!object->active) return true;
        Scene *scene = (Scene*)param;
        if (!AABB::aabbOverlapsAABB(scene->aabb, object->aabb)) {

            float xToAlignA = scene->aabb.max_box.x - object->aabb.min_box.x;
            float xToAlignB = scene->aabb.min_box.x - object->aabb.max_box.x;
            
            if (absv(xToAlignA) < absv(xToAlignB))
                scene->velocityFixWhenInverted = xToAlignA;
            else
                scene->velocityFixWhenInverted = xToAlignB;

            scene->invert_Velocity = true;
            return false;
        }
        return true;
    }
    //----------------------------------------------------------------------------
    static bool invertVelocity(void* param, Object *object) {
        if (!object->active) return true;
        Scene *scene = (Scene*)param;
        object->velocity = object->velocity*(-1);
        object->position = object->position + scene->down_velocity + vec2(scene->velocityFixWhenInverted, 0.0f);
        if (object->aabb.min_box.y < scene->aabb.min_box.y) {
            scene->killAllSpaceShip();
        }
        return true;
    }
    //----------------------------------------------------------------------------
    static bool calculateColision(void* param, Object *object) {
        if (!object->active) return true;
        Scene *scene = (Scene*)param;
        vec2 colisionPoint, colisionPoint2;
        if (scene->listToVerify->hasColision(object, &colisionPoint)) {
            colisionPoint2 = object->position;
            scene->listOfObject->remove(object);
            scene->xplosion(colisionPoint);
            scene->xplosion(colisionPoint2);
        }
        return true;
    }
    //----------------------------------------------------------------------------
    void CreateEnemy(int Nlinhas, int NColunas, double Vspace, double Hspace) {
        int OoS = 0;
        vec2 initHoriz = vec2(0),
            endHoriz = vec2(absv(Hspace*Nlinhas), 0),
            initVert = vec2(0),
            endVert = vec2(0, -absv(Vspace*NColunas)),
            pos;
        float aux = 0;
        for (int i = 0; i < Nlinhas; i++) {
            aux = (i % 2) ? 0.75 : 0.25;
            for (int j = 0; j < NColunas; j++) {
                pos = vec2(aabb.min_box.x, aabb.max_box.y) +
                    lerp(initHoriz, endHoriz, ((float)(j)+aux) / (float)(Nlinhas - 1)) +
                    lerp(initVert, endVert, (float)(i) / (float)(NColunas));
                vec2 Position = OutOfScreenArray[OoS],
                    dimension = vec2(0.2, 0.2);

                Object obj (
                  OT_Enemy,//type
                  OS_GotoFormationOrigin,//action;
                    Position,//formation.origin;//posi��o de entrada
                    pos,//formation.toMove;//posi��o final
                  Position,//position;
                  AABB(Position - dimension / 2,Position + dimension / 2),//aabb;//Axis-Aligned Bound Box
                  false,//active;
                  dimension,//dimension;
                  enemy_velocity,//velocity
                  0//int counter
                );

                OoS = (OoS + 1) % (OS_GotoOutOfScreen_MAX + 1);
                enemyList.insert(obj);
            }
        }
    }
    //----------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------------
    vec2 OutOfScreenArray[OS_GotoOutOfScreen_MAX + 1];
    AABB aabb;//Axis-Aligned Bound Box  

    vec2 enemy_velocity, down_velocity;

    ObjectList enemyList;
    ObjectList enemyShootList;
    ObjectList spaceShipShootList;

    ObjectList xplosionList;

    ObjectList spaceShipList;
    int score;
    float aspectX;
    int fatorNumeroTiro;
    //----------------------------------------------------------------------------
    Scene(SpaceInvader *si) {

        invaderref = si;

        fatorNumeroTiro = 0;
        aspectX = (float)si->render->dimension.x / (float)si->render->dimension.y;
        score = 0;
        globalLerpOn = true;
        globalLerp = 0;
        enemy_velocity = vec2(0);
        down_velocity = vec2(0);
        aabb = AABB(vec2(-aspectX, -1), vec2(aspectX, 1));
        OutOfScreenArray[0] = vec2(-1.2*aspectX, 1.2);
        OutOfScreenArray[1] = vec2(1.2*aspectX, 1.2);
        OutOfScreenArray[2] = vec2(-1.2*aspectX, 1.5);
        OutOfScreenArray[3] = vec2(1.2*aspectX, 1.5);
        OutOfScreenArray[4] = vec2(-1.2*aspectX, 1);
        OutOfScreenArray[5] = vec2(1.2*aspectX, 1);
        //loadXplodeTex();
    }

    void resizeWindow(int w, int h) {
        
        aspectX = (float)w / (float)h;

        aabb = AABB(vec2(-aspectX, -1), vec2(aspectX, 1));
        OutOfScreenArray[0] = vec2(-1.2*aspectX, 1.2);
        OutOfScreenArray[1] = vec2(1.2*aspectX, 1.2);
        OutOfScreenArray[2] = vec2(-1.2*aspectX, 1.5);
        OutOfScreenArray[3] = vec2(1.2*aspectX, 1.5);
        OutOfScreenArray[4] = vec2(-1.2*aspectX, 1);
        OutOfScreenArray[5] = vec2(1.2*aspectX, 1);

    }

    ~Scene() {
        //unloadXplodeTex();
    }
    //----------------------------------------------------------------------------
    void createEnemy(double enemy_Velocity, double down_Velocity,
        int Nl, int Nc, double vspc, double hspc) {
        this->enemy_velocity = vec2(absv(enemy_Velocity), 0);
        this->down_velocity = vec2(0, -absv(down_Velocity));

        CreateEnemy(Nl, Nc, vspc, hspc);
    }
    //----------------------------------------------------------------------------
    void createSpaceShip(int numberOfLifes) {
        vec2 Position = vec2(0, -0.8),
            dimension = vec2(0.2, 0.2);

        Object aux(
                OT_SpaceShip,//type
                OS_Nothing,//action;
                 vec2(0),//formation.origin;//posi��o de entrada
                 vec2(0),//formation.squadForm;//posi��o na forma��o de ataque
                Position,//position;
                AABB(Position - dimension / 2,Position + dimension / 2),//aabb;//Axis-Aligned Bound Box
                false,//active;
                dimension,//dimension;
                vec2(0),//velocity
                0//int counter
        );
        defaultSpaceship = aux;
        for (int i = 0; i < numberOfLifes; i++) {
            spaceShipList.insert(defaultSpaceship);
        }
    }
    //----------------------------------------------------------------------------
    void clear() {
        score = 0;
        globalLerpOn = true;
        globalLerp = 0;
        enemy_velocity = vec2(0);
        down_velocity = vec2(0);
        aabb = AABB(vec2(-aspectX, -1), vec2(aspectX, 1));
        enemyList.clear();
        enemyShootList.clear();
        spaceShipShootList.clear();
        spaceShipList.clear();
        xplosionList.clear();
    }
    //----------------------------------------------------------------------------
    void calculateAABB() {
        enemyList.traverseObjectList(calculateAABB, this);
        enemyShootList.traverseObjectList(calculateAABB, this);
        spaceShipList.traverseObjectList(calculateAABB, this);
        spaceShipShootList.traverseObjectList(calculateAABB, this);
    }
    //----------------------------------------------------------------------------
    void calculateColisions() {
        listOfObject = &enemyList;
        listToVerify = &spaceShipShootList;
        listOfObject->traverseObjectList(calculateColision, this);
        score += enemyList.getNumberOfRemoves() * 5;

        listOfObject = &enemyShootList;
        listToVerify = &spaceShipShootList;
        listOfObject->traverseObjectList(calculateColision, this);

        listOfObject = &spaceShipList;
        listToVerify = &enemyShootList;
        listOfObject->traverseObjectList(calculateColision, this);

        listOfObject = &enemyList;
        listToVerify = &spaceShipList;
        listOfObject->traverseObjectList(calculateColision, this);

        enemyList.doRemove();
        enemyShootList.doRemove();
        spaceShipList.doRemove();
        spaceShipShootList.doRemove();
    }
    //----------------------------------------------------------------------------
    void simulationTick() {
        if (spaceShipList.getFirst() == NULL) return;


        invert_Velocity = false;
        enemyList.traverseObjectList(verifyLimitToSetVelocity, this);
        if (invert_Velocity) {
            enemyList.traverseObjectList(invertVelocity, this);
            velocityFixWhenInverted = 0;
        }


        static float elapsed = 0.0f;

        elapsed += invaderref->time.deltaTime;

        if (elapsed > 0.050f) {
            canGenerateShoot = true;
            elapsed = 0.0f;
        }
        else {
            canGenerateShoot = false;
        }

        enemyList.traverseObjectList(calculatePosition, this);
        enemyShootList.traverseObjectList(calculatePosition, this);
        spaceShipList.traverseObjectList(calculatePosition, this);
        spaceShipShootList.traverseObjectList(calculatePosition, this);

        calculateAABB();
        calculateColisions();

        /*
          verificando tiros que sairam fora da tela
        */
        listOfObject = &enemyShootList;
        enemyShootList.traverseObjectList(verifyLimitAndRemoveIfPasses, this);
        listOfObject = &spaceShipShootList;
        spaceShipShootList.traverseObjectList(verifyLimitAndRemoveIfPasses, this);

        if (canGenerateShoot) {
            listOfObject = &xplosionList;
            xplosionList.traverseObjectList(countXplosion, this);
            xplosionList.doRemove();
        }

        globalLerp = (globalLerp + minimum ( invaderref->time.deltaTime * 0.5f, 0.01f ) );
        if (globalLerpOn && globalLerp >= 1) {
            enemyList.traverseObjectList(activeObject, this);
            globalLerpOn = false;
            globalLerp = 1.0;
            if (getSpaceShipObject() != NULL)
                activeFirstSpaceShip();
        }
    }
    //----------------------------------------------------------------------------
    bool isLevelComplete() {
        return (enemyList.size() == 0);
    }
    //----------------------------------------------------------------------------
    void activeFirstSpaceShip() {
        getSpaceShipObject()->active = true;
    }
    //----------------------------------------------------------------------------
    /*
      retorna NULL se n�o existir nenhuma spaceship
    */
    Object* getSpaceShipObject() {
        Object* retorno = spaceShipList.getFirst();
        //    retorno->active = true;
        return retorno;
    }
    //----------------------------------------------------------------------------
    void enemyShoot(const vec2 &position) {
        if (getSpaceShipObject() == NULL || !getSpaceShipObject()->active) return;
        invaderref->resources->sndPlayShoot();
        double shootSpeed = 0.05;
        vec2 dimension = vec2(0.05, 0.1);
        Object aux(
                OT_EnemyShipShoot,//type
                OS_Nothing,//action;
                 vec2(0),//formation.origin;//posi��o de entrada
                 vec2(0),//formation.squadForm;//posi��o na forma��o de ataque
                position,//position;
                AABB(position - dimension / 2,position + dimension / 2),//aabb;//Axis-Aligned Bound Box
                true,//active;
                dimension,//dimension;
                vec2(0,-absv(shootSpeed)),//velocity
                0//int counter
        );
        enemyShootList.insert(aux);
    }
    //----------------------------------------------------------------------------
    void spaceShipShoot(const vec2 &position) {
        invaderref->resources->sndPlayShoot();
        double shootSpeed = 0.05;
        vec2 dimension = vec2(0.05, 0.1);
        Object aux (
                OT_SpaceShipShoot,//type
                OS_Nothing,//action;
                 vec2(0),//formation.origin;//posi��o de entrada
                 vec2(0),//formation.squadForm;//posi��o na forma��o de ataque
                position,//position;
                AABB(position - dimension / 2,position + dimension / 2),//aabb;//Axis-Aligned Bound Box
                true,//active;
                dimension,//dimension;
                vec2(0,absv(shootSpeed)),//velocity
                0//int counter
        );
        spaceShipShootList.insert(aux);
    }
    //----------------------------------------------------------------------------
    void xplosion(const vec2 &position) {
        invaderref->resources->sndPlayXplode();
        vec2 dimension = vec2(0.4, 0.4);
        Object aux(
            OT_Xplosion,//type
            OS_Nothing,//action;
             vec2(0),//formation.origin;//posi��o de entrada
             vec2(0),//formation.squadForm;//posi��o na forma��o de ataque
            position,//position;
            AABB(position - dimension / 2,position + dimension / 2),//aabb;//Axis-Aligned Bound Box
            true,//active;
            dimension,//dimension;
            vec2(0),//velocity
            0//int counter
        );
        xplosionList.insert(aux);
    }
    //----------------------------------------------------------------------------
    void killAllSpaceShip() {
        spaceShipList.clear();
    }
    //----------------------------------------------------------------------------  
    void printState() {
        printf("     SCENE");
        for (int i = 0; i < OS_GotoOutOfScreen_MAX + 1; i++) {
            printf("        OutOfScreenArray[%i] = (%g,%g)\n", i, OutOfScreenArray[i].x, OutOfScreenArray[i].y);
        }
        printf("        globalLerp  = %g\n", globalLerp);
        printf("        aabb = IE(%g, %g) SD(%g, %g)\n", aabb.min_box.x, aabb.min_box.y,
            aabb.max_box.x, aabb.max_box.y);

        printf("        enemy_velocity = (%g, %g)\n", enemy_velocity.x, enemy_velocity.y);
        printf("        down_velocity = (%g, %g)\n", down_velocity.x, down_velocity.y);
        //    printf("      --enemyList\n");
        //    enemyList.traverseObjectList(printObjectContent,this);
        printf("      --enemyShootList\n");
        //    enemyShootList.traverseObjectList(printObjectContent,this);
        printf("      --spaceShipList\n");
        spaceShipList.traverseObjectList(printObjectContent, this);
        printf("      --spaceShipShootList\n");
        spaceShipShootList.traverseObjectList(printObjectContent, this);
    }
    //----------------------------------------------------------------------------
    void render(float deltaTime) {

        glDisable(GL_DEPTH_TEST);




        //PushEnableAttribWithDepthAndLightingDisabled();

        //glScalef(1,1,1);

        //glColor3f(1,1,1);
        //aabb.draw();
    //    spaceShipList.traverseObjectList(drawObject,this);
    //    glColor3f(0,1,0);
    //    enemyList.traverseObjectList(drawObject,this);

        //glEnable(GL_TEXTURE_2D);
        //activeTexture(spaceship);

        //invaderref->render->drawAABB(aabb, vec4(1.0f, 1.0f, 1.0f, 0.3f));

        invaderref->render->shader->enable();

        invaderref->render->shader->setColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));


        textureref = invaderref->resources->spaceshipImg;
        spaceShipList.traverseObjectList(drawObject, this);

        textureref = invaderref->resources->enemyImg;
        //activeTexture(enemy);
        enemyList.traverseObjectList(drawObject, this);

        //activeTexture(tiro);

        textureref = invaderref->resources->tiroImg;
        
        invaderref->render->shader->setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
        //glColor3f(1,0,0);

        enemyShootList.traverseObjectList(drawObject, this);
        spaceShipShootList.traverseObjectList(drawObject, this);

        invaderref->render->shader->setColor(vec4(1.0f));

        //glColor3f(1,1,1);
        xplosionList.traverseObjectList(drawObject, this);



        //PopAttrib();
    }
    //----------------------------------------------------------------------------
    bool getglobalLerpOn() {
        return globalLerpOn;
    }
    //----------------------------------------------------------------------------
    void shootShip() {
        if (!globalLerpOn) {
            //      printf("MOUSE DOWN!!!!!!!!!!\n");
            Object *tmp = getSpaceShipObject();
            if (tmp != NULL) {
                if (tmp->active) {
                    //          printf("SHOOOOOOOOOOOOOOOOOOOOOOOOOOTTTTTTTTTTTT\n");
                    spaceShipShoot(tmp->position);
                }
                else {
                    //          tmp->active = true;
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    bool activateSpaceShip() {
        if (!globalLerpOn&&getSpaceShipObject() != NULL) {
            getSpaceShipObject()->active = true;
            return true;
        }
        if (globalLerpOn) return true;
        return false;
    }
    //----------------------------------------------------------------------------
    float getSpaceShipLerp() {
        Object *tmp = getSpaceShipObject();
        if (tmp != NULL)
            return (tmp->velocity.x + 0.1) / 0.2;
        return 0;
    }
    //----------------------------------------------------------------------------
    void incVelShip(float vel) {
#define mouseSensibility 1.001
        if (!globalLerpOn) {
            Object *tmp = getSpaceShipObject();
            if (tmp != NULL && tmp->active) {

                /*
                tmp->velocity = clamp(
                    tmp->velocity +
                    vec2(1, 0)*(((float)vel)*mouseSensibility),

                    vec2(-0.1, 0), vec2(0.1, 0));
                */


                tmp->velocity = vec2(vel, 0.0);

            }
        }
    }
    //----------------------------------------------------------------------------
    void renderCongratulations(const vec2& screenCenter, float deltatime);
    //----------------------------------------------------------------------------
};



#endif
