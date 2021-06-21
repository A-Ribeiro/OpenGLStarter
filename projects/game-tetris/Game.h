
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include "common/common.h"
#include "tetris/Tetris.h"

extern bool canExitApp;


const char  *FONT_CONFIG_BRANCH = "FontConfig",
*FONTFILE = "file";
//*METRICS = "metrics",
//*IMAGE = "image";

const char  *SOUND_CONFIG_BRANCH = "SoundConfig",
*MUSIC_LEVEL = "MusicLevel",
*FX_LEVEL = "FxLevel";

const char  *SCORE_BRANCH = "Score",
*SCORE_NAME = "name",
*SCORE_POINTS = "points",
*SCORE_LAST_NAME = "lastName";

const char  *VERSION_STRING = "May have some bugs\nBeta (RC 1.0)";

#include <string>
class ScoreMenu : public Interface {
    GLTexture *cursor,
        *backGrnd;
    GLFont *font;

    AABBMenuList *list;
    AlMultiSource *click, *hover;
    vec2 centerScreen, mousepos;
    ConfigFile *score;
    std::string names, scores;

    vec2 dimension_inv;

    void loadScores() {
        names.clear();
        scores.clear();
        char aux[20];
        sprintf(aux, "%s0", SCORE_NAME);
        names = score->getString(SCORE_BRANCH, aux, "Unknown");
        sprintf(aux, "%s0", SCORE_POINTS);
        scores = score->getString(SCORE_BRANCH, aux, "0");
        for (int i = 1; i < 5; i++) {
            sprintf(aux, "%s%u", SCORE_NAME, i);
            names = names + "\n" + score->getString(SCORE_BRANCH, aux, "Unknown");
            sprintf(aux, "%s%u", SCORE_POINTS, i);
            scores = scores + "\n" + score->getString(SCORE_BRANCH, aux, "0");
        }
    }
    int verifyInsertPosition(const int newScore)const {
        char aux[20];
        for (int i = 0; i < 5; i++) {
            sprintf(aux, "%s%u", SCORE_POINTS, i);
            if (newScore >= score->getInt(SCORE_BRANCH, aux, 0)) {
                return i;
            }
        }
        return -1;
    }
    //-----------------------------------------
    int insertPosition;
    bool toInsertNewScore;
    std::string nameToInsert, nameToDisplay;
    int backSpace, lastTime, scorePtn;
    //--------------------------
    void insertNewScoreInPosition() {
        score->setString(SCORE_BRANCH, SCORE_LAST_NAME, nameToInsert.c_str());
        char aux[20], aux2[20];
        for (int i = 5 - 1; i > insertPosition; i--) {
            sprintf(aux, "%s%u", SCORE_POINTS, i);
            sprintf(aux2, "%s%u", SCORE_POINTS, i - 1);
            score->setInt(SCORE_BRANCH, aux, score->getInt(SCORE_BRANCH, aux2, 0));
            sprintf(aux, "%s%u", SCORE_NAME, i);
            sprintf(aux2, "%s%u", SCORE_NAME, i - 1);
            score->setString(SCORE_BRANCH, aux, score->getString(SCORE_BRANCH, aux2, "Unknown"));
        }
        sprintf(aux, "%s%u", SCORE_NAME, insertPosition);
        score->setString(SCORE_BRANCH, aux, nameToInsert.c_str());
        sprintf(aux, "%s%u", SCORE_POINTS, insertPosition);
        score->setInt(SCORE_BRANCH, aux, scorePtn);
        score->save();
        toInsertNewScore = false;
        loadScores();
    }
public:
    ScoreMenu(GLTexture *cursor,
        ConfigFile *score,
        AlMultiSource *click,
        AlMultiSource *hover,
        GLFont *fnt,
        bool enterNewScore,
        int scorePtn) {

        backGrnd = GLTexture::loadFromFile("resources/images/score.png");

        this->score = score;
        font = fnt;
        this->cursor = cursor;
        //create OK button
        const char *files[] = { "resources/images/ok.png" };
        const char *filesSel[] = { "resources/images/oks.png" };
        list = new AABBMenuList(files, filesSel, 1, 0);

        this->click = click;
        this->hover = hover;

        toInsertNewScore = false;
        if (enterNewScore) {
            insertPosition = verifyInsertPosition(scorePtn);
            if (insertPosition >= 0) {
                nameToInsert = score->getString(SCORE_BRANCH, SCORE_LAST_NAME, "Unknown");
                nameToDisplay = nameToInsert + ".";
                toInsertNewScore = true;
                backSpace = -1;
                this->scorePtn = scorePtn;
            }
        }
        if (!toInsertNewScore)
            loadScores();
    }
    ~ScoreMenu() {

        setNullAndDelete(backGrnd);

        names.clear();
        scores.clear();
        if (list != NULL)
            delete list;
        list = NULL;
    }
    void resizeWindow(const int w, const int h) {
        centerScreen = vec2(w, h)*0.5f;

        dimension_inv = vec2(2.0f / (float)w, 2.0f / (float)h);

        list->setPosition(centerScreen + vec2(215, -150), true);
    }
    void render(const unsigned int time_ms, const bool showCursor = true) {

        RenderSystem *render = RenderSystem::getSingleton();

        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);

        render->model.top = translate(-1, -1, 0) * scale(dimension_inv.x, dimension_inv.y, 1);

        if (toInsertNewScore) {
            if (backSpace > 0) {
                backSpace -= diffSystime(time_ms, lastTime);
                if (backSpace <= 0) {
                    if (nameToInsert.length() > 0) {
                        nameToInsert.resize(nameToInsert.length() - 1);
                        nameToDisplay = nameToInsert + ".";
                        backSpace = 50;
                    }
                    else
                        backSpace = -1;
                }
            }
            lastTime = time_ms;

            render->print(font, "Your name: ", centerScreen, vec4(1.0f), 1.0f, 0.5f, 0.75f);

            std::string nameToDisplayAux;

            if (
                ((time_ms / 200) % 2 == 1)
                && nameToDisplay.size() > 0)
                nameToDisplayAux = nameToDisplay.substr(0, nameToDisplay.length() - 1);
            else
                nameToDisplayAux = nameToDisplay.substr(0, nameToDisplay.length());

            render->print(font, nameToDisplayAux.c_str(), centerScreen, vec4(1.0f), 0.0f, 0.5f, 0.75f);

            //font->staticPrint(PM_RIGHT, centerScreen, "Your name: ", 11);
            //font->staticPrint(PM_NONE, centerScreen, nameToDisplay.c_str(), nameToDisplay.length() - (time_ms / 200) % 2);
        }
        else {
            render->drawTexture_center(backGrnd, centerScreen);

            //backGrnd.centerDraw(centerScreen);

            render->print(font, "Names", centerScreen + vec2(70, 200), vec4(1.0f), 1.0f, 1.0f, 0.75f);
            render->print(font, names.c_str(), centerScreen + vec2(70, 105), vec4(1.0f), 1.0f, 1.0f, 0.75f);

            render->print(font, "Score", centerScreen + vec2(130, 200), vec4(1.0f), 0.0f, 1.0f, 0.75f);
            render->print(font, scores.c_str(), centerScreen + vec2(130, 105), vec4(1.0f), 0.0f, 1.0f, 0.75f);

            /*
            font->staticPrint(PM_TOP_BEGIN_FROM_UPPER | PM_RIGHT, centerScreen + vec2(70, 200), "Names", 5);
            font->staticPrint(PM_TOP_BEGIN_FROM_UPPER | PM_RIGHT, centerScreen + vec2(70, 105), names.c_str(), names.length());
            font->staticPrint(PM_TOP_BEGIN_FROM_UPPER, centerScreen + vec2(130, 200), "Score", 5);
            font->staticPrint(PM_TOP_BEGIN_FROM_UPPER, centerScreen + vec2(130, 105), scores.c_str(), scores.length());
            */

            list->render();
        }

        render->drawTexture_center(cursor, mousepos);
        //cursor->centerDraw(mousepos);
    }
    bool keyUp(const int k) {
        //if (k == '\b')
        if (k == sf::Keyboard::BackSpace)
            backSpace = -1;
        return true;
    }
    bool keyDown(const int k) {
        if (toInsertNewScore) {

            if (k == sf::Keyboard::Return)
                //if (k == 13)
            {
                insertNewScoreInPosition();
            }
            else
                if (k == sf::Keyboard::Escape)
                    //if (k == 27)
                {
                    toInsertNewScore = false;
                    loadScores();
                }
                else
                    if (k == sf::Keyboard::BackSpace)
                        //if (k == '\b')
                    {
                        if (nameToInsert.length() > 0) {
                            nameToInsert.resize(nameToInsert.length() - 1);
                            nameToDisplay = nameToInsert + ".";
                            backSpace = 500;
                        }
                    }
                    else
                        if ((k >= sf::Keyboard::A && k <= sf::Keyboard::Z) ||
                            (k == sf::Keyboard::Space)
                            )
                            /*
                        if ((k >= 'a'&&k <= 'z') ||
                            (k >= 'A'&&k <= 'Z') ||
                            (k >= '0'&&k <= '9') ||
                            (k == ' ') || (k == ':') ||
                            (k == ';') || (k == ',') ||
                            (k == '.') || (k == '(') ||
                            (k == ')') || (k == '?') ||
                            (k == '\"') || (k == '!'))
                        */
                        {
                            if (k == sf::Keyboard::Space)
                                nameToInsert += ' ';
                            else {
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
                                    nameToInsert += 'A' + k - sf::Keyboard::A;
                                else
                                    nameToInsert += 'a' + k - sf::Keyboard::A;
                            }

                            nameToDisplay = nameToInsert + ".";
                        }
            return true;
        }
        //if (k == 27 || k == 13)
        if (k == sf::Keyboard::Escape || k == sf::Keyboard::Return)
            setActionReturn(3);
        return true;
    }
    void setMousePos(const vec2 &pos) {
        mousepos = pos;
        if (toInsertNewScore) return;
        int lastS = list->getSelected();
        list->setSelected(list->colision(pos));
        int newS = list->getSelected();
        if (newS >= 0 && lastS != newS) {
            hover->play();
        }
    }
    bool mouseDown(const int btn, const vec2 &pos) {
        if (toInsertNewScore) return true;
        if (btn == sf::Mouse::Left) {
            if (list->getSelected() == 0) {//OK
                click->play();
                setActionReturn(3);
            }
        }
        return true;
    }
};



class MainOptions : public GUIGroup {
    GLTexture *cursor,
        *backGrnd;
    GLFont *font;
    int MusicS, MusicL, FxS, FxL;
    GLTexture *bolinha;
    ConfigFile *config;
    float musicLevel, fxLevel;

    AABBMenuList *list;
    AlMultiSource *click, *hover;
public:
    MainOptions(GLTexture *cursor,
        ConfigFile *config,
        AlMultiSource *click,
        AlMultiSource *hover,
        GLFont *fnt)
        //:
        //backGrnd("./images/optSound.png"),//backGrnd("images/options.png"),
        //bolinha("./images/bolinha.png")
    {

        backGrnd = GLTexture::loadFromFile("resources/images/optSound.png");
        bolinha = GLTexture::loadFromFile("resources/images/bolinha.png");


        font = fnt;
        this->cursor = cursor;
        this->config = config;
        //load options
        musicLevel = config->getFloat(SOUND_CONFIG_BRANCH, MUSIC_LEVEL, 1.0);
        fxLevel = config->getFloat(SOUND_CONFIG_BRANCH, FX_LEVEL, 1.0);
        //create OK button
        const char *files[] = { "resources/images/ok.png" };
        const char *filesSel[] = { "resources/images/oks.png" };
        list = new AABBMenuList(files, filesSel, 1, 0);

        this->click = click;
        this->hover = hover;
    }
    ~MainOptions() {
        config->setFloat(SOUND_CONFIG_BRANCH, MUSIC_LEVEL, musicLevel);
        config->setFloat(SOUND_CONFIG_BRANCH, FX_LEVEL, fxLevel);
        if (list != NULL)
            delete list;
        list = NULL;

        setNullAndDelete(backGrnd);
        setNullAndDelete(bolinha);

    }
    void resizeWindow(const int w, const int h) {
        if (this->w == w && this->h == h) return;
        GUIGroup::resizeWindow(w, h);

        clearAll();
        MusicS = addSlider(centerScreen + vec2(-110, -12),
            centerScreen + vec2(105, -12),
            20, bolinha);
        FxS = addSlider(centerScreen + vec2(-110, -117),
            centerScreen + vec2(105, -117),
            20, bolinha);
        /*
            addSlider(centerScreen + vec2(-152,-32),
                      centerScreen + vec2( 185,-32),
                      20);
            addSlider(centerScreen + vec2(-152,-147),
                      centerScreen + vec2( 185,-147),
                      20);//*/
        MusicL = addLabel(font,
            "000",
            centerScreen + vec2(-42, 8),
            false, false, 0.75f);
        FxL = addLabel(font,
            "000",
            centerScreen + vec2(-82, -91),
            false, false, 0.75f);

        ((Slider*)getInterface(MusicS))->setLerp(musicLevel);
        ((Slider*)getInterface(FxS))->setLerp(fxLevel);

        char tmp[10];
        sprintf(tmp, "%03.0f", musicLevel*100.0f);
        ((Label*)getInterface(MusicL))->setText(tmp);

        sprintf(tmp, "%03.0f", fxLevel*100.0f);
        ((Label*)getInterface(FxL))->setText(tmp);
        /*
            p = p + vec2( 0,font.getHeight());

            label2 = addLabelBtn(&font,
                                 "ColehBtn!!!",
                                 p,vec2(400,0),
                                 true, true);//*/

        list->setPosition(centerScreen + vec2(215, -150), true);
    }
    bool performAction(const int ID, Event &e) {
        //    if(ID == label2) return false;
        switch (e.type) {
        case LERP_CHANGED: {
            char tmp[1024];
            sprintf(tmp, "%03.0f", e.lerpChanged.newLerp*100.0f);
            Label* l = NULL;
            if (ID == MusicS) {
                musicLevel = e.lerpChanged.newLerp;
                l = (Label*)getInterface(MusicL);
            }
            if (ID == FxS) {
                fxLevel = e.lerpChanged.newLerp;
                l = (Label*)getInterface(FxL);
            }
            if (l != NULL)
                l->setText(tmp);
        }
        case ET_NON_SPECIFIED:
        case MOUSE_PRESS:
        case MOUSE_RELEASE:
        case KEY_PRESS:
        case KEY_RELEASE:
        case SET_FOCUS:
            break;
        }
        return true;
    }
    void render(const unsigned int time_ms, const bool showCursor = true) {
        RenderSystem *render = RenderSystem::getSingleton();

        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);

        render->model.top = translate(-1, -1, 0) * scale(GUIGroup::screenRes_inv.x, GUIGroup::screenRes_inv.y, 1);

        render->drawTexture_center(backGrnd, centerScreen);

        //backGrnd.centerDraw(centerScreen);

        GUIGroup::render(time_ms, false);
        list->render();

        render->drawTexture_center(cursor, mousepos);
        //cursor->centerDraw(mousepos);
    }
    bool keyDown(const int k) {
        GUIGroup::keyDown(k);
        if (k == sf::Keyboard::Escape || k == sf::Keyboard::Return)
            setActionReturn(3);
        return true;
    }
    void setMousePos(const vec2 &pos) {
        GUIGroup::setMousePos(pos);
        int lastS = list->getSelected();
        list->setSelected(list->colision(pos));
        int newS = list->getSelected();
        if (newS >= 0 && lastS != newS) {
            hover->play();
        }
    }
    bool mouseDown(const int btn, const vec2 &pos) {
        GUIGroup::mouseDown(btn, pos);
        if (btn == sf::Mouse::Left) {
            if (list->getSelected() == 0) {//OK
                click->play();
                setActionReturn(3);
            }
        }
        return true;
    }

};



class MainMenu : public Interface {

    //ListHelper listCommands;

    GLTexture *img,
        *cursor;
    vec2 mousePos,
        dimension,
        dimension_inv;

    //ListHelper drawBackGround;

    int selected;
    AABBMenuList *list;

    AlMultiSource *click, *hover;
    ConfigFile *config;

    GLFont *font;

public:
    MainMenu(GLTexture *cursor, ConfigFile *config,
        AlMultiSource *click, AlMultiSource *hover,
        GLFont *fnt)
        //:listCommands(1),
        //img("./images/titulo.png")
    {

        img = GLTexture::loadFromFile("resources/images/titulo.png");

        font = fnt;
        this->cursor = cursor;
        const char *files[] = { "resources/images/start.png" ,"resources/images/opt.png" ,"resources/images/scorebtn.png" ,"resources/images/exit.png" };
        const char *filesSel[] = { "resources/images/starts.png","resources/images/opts.png","resources/images/scorebtns.png","resources/images/exits.png" };
        list = new AABBMenuList(files, filesSel, 4, 0);
        selected = -1;
        this->config = config;
        this->click = click;
        this->hover = hover;
    }
    ~MainMenu() {
        if (list) delete list;
        list = NULL;

        setNullAndDelete(img);

    }
    void resizeWindow(const int w, const int h) {
        dimension = vec2(w, h);
        vec2 dimension_2 = dimension * 0.5;
        dimension_inv.x = 2.0f / (float)w;
        dimension_inv.y = 2.0f / (float)h;

        /*
        drawBackGround.begin();
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1, 1, 1);
        glLoadIdentity();
        glTranslatef(-1, -1, 0);
        glScalef(dimension_inv.x, dimension_inv.y, 1);
        img.centerDraw(dimension_2);
        drawBackGround.end();
        */

        dimension_2 = dimension_2 + vec2(0, (float)img->height * 0.5 - 150.0);
        list->setPosition(dimension_2, true);
    }
    void render(const unsigned int time_ms, const bool showCursor = true) {
        RenderSystem *render = RenderSystem::getSingleton();

        //drawBackGround.call();

        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);


        render->model.top = translate(-1, -1, 0) * scale(dimension_inv.x, dimension_inv.y, 1);

        //glColor3f(1, 1, 1);
        //glLoadIdentity();
        //glTranslatef(-1, -1, 0);
        //glScalef(dimension_inv.x, dimension_inv.y, 1);

        vec2 dimension_2 = dimension * 0.5;

        render->drawTexture_center(img, dimension_2);

        //img.centerDraw(dimension_2);



        list->render();

        render->print(font, VERSION_STRING, vec2(dimension.x - 50.0f, 50.0f), vec4(1.0f), 1.0f, 0.0f, 0.5f);

        //font->staticPrint(PM_BOTOM_BEGIN_FROM_DOWN | PM_RIGHT,
            //vec2(dimension.x - 50.0f, 50.0f),
            //VERSION_STRING, strlen(VERSION_STRING));


        render->drawTexture_center(cursor, mousePos);
        //cursor->centerDraw(mousePos);
    }
    void setMousePos(const vec2 &pos) {
        mousePos = pos;
        int lastS = list->getSelected();
        list->setSelected(list->colision(pos));
        int newS = list->getSelected();
        if (newS >= 0 && lastS != newS) {
            hover->play();
        }
    }
    void dispatchSelected() {
        unsigned int item = list->getSelected();
        switch (item) {
        case 0://start
            setActionReturn(1);//ir para interface 1
            click->play();
            break;
        case 1://opt
            setActionReturn(2);//ir para interface 1
            click->play();
            break;
        case 2://scores
            setActionReturn(5);//ir para interface 1
            click->play();
            break;
        case 3://exit
            click->play();
            while (click->isPlaying());
            canExitApp = true;


            //windowExitApplication();
            break;
        }
    }
    bool mouseDown(const int btn, const vec2 &pos) {
        if (btn == sf::Mouse::Left) {
            dispatchSelected();
        }
        return true;
    }
    bool keyDown(const int k) {
        switch (k) {
            //case 103 << 16:
        case sf::Keyboard::Down:
        {
            int s = list->getSelected();
            list->setSelected(s + 1);
            if (s >= list->count() - 1)
                list->setSelected(-1);
            else if (s >= -1)
                hover->play();
            //tetris.player1PushBtn(Down);
        }
        break;
        //case 102 << 16:
        case sf::Keyboard::Right:
            //tetris.player1PushBtn(Left);
            break;
            //case 101 << 16:
        case sf::Keyboard::Up:
        {
            int s = list->getSelected();
            if (s < 0)
                s = list->count();
            list->setSelected(s - 1);
            if (s > 0)
                hover->play();
            //tetris.player1PushBtn(Rotate);
        }
        break;
        //case 100 << 16:
        case sf::Keyboard::Left:
            //tetris.player1PushBtn(Right);
            break;
        case sf::Keyboard::Escape:
            canExitApp = true;
            //windowExitApplication();
            break;
        case sf::Keyboard::Return:
            dispatchSelected();
            break;
        }
        return true;
    }
};




class TetrisGame : public Interface {
    AlMultiSource whoosh,
        plink,
        crack,
        boom;
    GLTexture *paused,
        *cursor;
    Tetris tetris;
    vec2 center, mouse;
    GLFont *font;
public:
    TetrisGame(GLTexture *cursor,
        ConfigFile *config,
        GLFont *fnt) :whoosh("resources/sons/hover.ogg"),
        plink("resources/sons/click.ogg"),
        crack("resources/sons/click.ogg"),
        boom("resources/sons/crash.ogg"),
        //paused("./images/pause.png"),
        tetris(&whoosh, &plink,
            &boom) {

        paused = GLTexture::loadFromFile("resources/images/pause.png");

        font = fnt;
        this->cursor = cursor;
        float Fxlvl = config->getFloat(SOUND_CONFIG_BRANCH, FX_LEVEL, 1.0);
        whoosh.setGain(Fxlvl);
        plink.setGain(Fxlvl);
        crack.setGain(Fxlvl);
        boom.setGain(Fxlvl);
    }
    ~TetrisGame() {
        setNullAndDelete(paused);
    }
    void resizeWindow(const int w, const int h) {
        center = vec2(w, h)*0.5;
    }
    void render(const unsigned int time_ms, const bool showCursor = true) {
        RenderSystem *render = RenderSystem::getSingleton();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_DEPTH_TEST);

        render->projection.push();
        render->projection.top =
            projection_perspective_rh_negative_one(45.0, center.x / center.y, 0.001, 1000) *
            scale(0.001, 0.001, 0.001);

        render->model.top = mat4::IdentityMatrix;

        //glMatrixMode(GL_PROJECTION);
        //glPushMatrix();
        //loadMatrixToGL(projection(45.0, center.x / center.y, 0.001, 1000));
        //scale(0.001, 0.001, 0.001);
        //glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();


        /*
            float v = cos((double)time_ms/10.0);
            v = lerp(-0.01, 0.01,v);
            float f = sin((double)time_ms/10.0);
            f = lerp(-0.01, 0.01,f);
        //*/
        //vec3 p = tetris.getViewPos();
        /*
        gluLookAt(-0.3,0.3-0.2,5,
          //        p.x,p.y,p.z,
                  0,0,0,
                  0,1,0);
        glScalef(1,1,1);//*/


        tetris.render(time_ms, center);

        //glPopAttrib();
        glDisable(GL_DEPTH_TEST);

        render->projection.pop();

        //glMatrixMode(GL_PROJECTION);
        //glPopMatrix();
        //glMatrixMode(GL_MODELVIEW);


        render->model.top =
            translate(-1, -1, 0) *
            scale(1.0f / center.x, 1.0f / center.y, 1);

        if (tetris.isPause()) {

            render->drawTexture_center(paused, center);
            render->drawTexture_center(cursor, mouse);

            //paused.centerDraw(center);
            //cursor->centerDraw(mouse);
        }

        if (tetris.isGameEnd()) {

            char txt[32];
            sprintf(txt, "Score: %04u \n\nEND GAME!!!!", tetris.getScore());

            render->print(font, txt, center + center * vec2(-0.8, 0.8), vec4(1.0f), 0.75f);

            /*
            font->printf(40,
                PM_NONE, center + center * vec2(-0.8, 0.8), "Score: %04u \nEND GAME!!!!", tetris.getScore());
                */

            render->drawTexture_center(cursor, mouse);
            //cursor->centerDraw(mouse);
        }
        else {

            char txt[32];
            sprintf(txt, "Score: %04u", tetris.getScore());

            render->print(font, txt, center + center * vec2(-0.8, 0.8), vec4(1.0f), 0.75f);

            //font->printf(40,
                //PM_NONE, center + center * vec2(-0.8, 0.8), "Score: %04u", tetris.getScore());
        }


        int lvl = tetris.getScore() / 10000 + 1;
        if (lvl > 4) lvl = 4;


        char txt[32];
        sprintf(txt, "Level:%i", lvl);

        render->print(font, txt, center + center * vec2(0.4, 0.8), vec4(1.0f), 0.75f);

        //font->printf(40, PM_NONE, center + center * vec2(0.4, 0.8), "Level:%i", lvl);
    }
    void setMousePos(const vec2 &pos) {
        mouse = pos;
    }
    bool mouseDown(const int btn, const vec2 &pos) {
        return true;
    }
    bool keyDown(const int k) {
        if (tetris.isGameEnd()) {
            switch (k) {
                //case 13:
            case sf::Keyboard::Return:
                //case 27:
            case sf::Keyboard::Escape:
                setActionReturn(6);
                break;
            }
            return true;
        }
        if (tetris.isPause()) {
            switch (k) {
                //case 13:
            case sf::Keyboard::Return:
                tetris.pause(); break;
                //case 27:
            case sf::Keyboard::Escape:
                setActionReturn(6); break;
            }
            return true;
        }
        //    printf("k: %d\n",k>>16);
        switch (k) {
            //case 103 << 16:
        case sf::Keyboard::Down:
            tetris.player1PushBtn(Down);
            break;
            //case 102 << 16:
        case sf::Keyboard::Right:
            tetris.player1PushBtn(Left);
            break;
            //case 101 << 16:
        case sf::Keyboard::Up:
            tetris.player1PushBtn(Rotate);
            break;
            //case 100 << 16:
        case sf::Keyboard::Left:
            tetris.player1PushBtn(Right);
            break;
            //case 27:
        case sf::Keyboard::Escape:
            tetris.pause();
        }
        return true;
    }
    bool keyUp(const int k) {
        if (tetris.isGameEnd()) return true;
        //    printf("r: %d\n",k>>16);
        switch (k) {
            //case 103 << 16:
        case sf::Keyboard::Down:
            tetris.player1ReleaseBtn(Down);
            break;
        }
        return true;
    }
    int getScore() {
        return tetris.getScore();
    }
};


class Game //: public GameAbstraction
{
    vec2 cursorPos,
        dimension;
    int w, h;
    GLTexture *cursor;
    Interface *activeInterface;
    AlMultiBuffer music;
    AlMultiSource click, hover;
    bool roundMusic;
    ConfigFile *config;

    GLFont *font;

public:
    Game(int w, int h) : //cursor("./images/cursor.png"),
        music("resources/musicas/musica-menu.ogg"),
        click("resources/sons/click.ogg"),
        hover("resources/sons/hover.ogg")
        //font(5, config.getString(FONT_CONFIG_BRANCH, IMAGE, "./font/scapsWhite.png"),
            //config.getString(FONT_CONFIG_BRANCH, METRICS, "./font/scaps.dat")
        //)

    {
        std::string path = PlatformPath::getSaveGamePath("AlessandroRibeiro","game-tetris");

        config = new ConfigFile( (path + PlatformPath::SEPARATOR + std::string("config.xml")) .c_str() );

        //music.play();

        //load font
        {
            Font* font_raw = Font::LoadFromBasof(
                config->getString(FONT_CONFIG_BRANCH, FONTFILE, "resources/font/SmartieCAPS.basof")
            );

            font = font_raw->createGLFont();
            setNullAndDelete(font_raw);
        }

        cursor = GLTexture::loadFromFile("resources/images/cursor.png");

        //windowFullScreen();


        music.setGain(config->getFloat(SOUND_CONFIG_BRANCH, MUSIC_LEVEL, 1.0));
        float Fxlvl = config->getFloat(SOUND_CONFIG_BRANCH, FX_LEVEL, 1.0);
        click.setGain(Fxlvl);
        hover.setGain(Fxlvl);

        dimension = vec2(w, h);
        this->w = w;
        this->h = h;
        //    glLineWidth(3);


        RenderSystem::getSingleton();
        /*
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 1.0 / 255.0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


        //glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearDepth(1.0);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);

        //glEnable(GL_LINE_SMOOTH);
        //glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

        */


        /*
#ifdef WIN32
        if (GLEE_WGL_EXT_swap_control) {
            wglSwapIntervalEXT(0);//vsync off
        }
#endif
        */

        //glPixelZoom(0.4,0.4);
        activeInterface = new MainMenu(cursor, config, &click, &hover, font);
        //activeInterface = new MainOptions(&cursor);
        roundMusic = false;


        resizeWindow(w, h);

    }
    ~Game() {
        if (activeInterface != NULL) delete activeInterface;
        activeInterface = NULL;

        setNullAndDelete(cursor);
        setNullAndDelete(font);
        setNullAndDelete(config);

    }
    void resizeWindow(const int w, const int h) {
        dimension = vec2(w, h);
        this->w = w;
        this->h = h;
        glViewport(0, 0, w, h);
        if (activeInterface != NULL)
            activeInterface->resizeWindow(w, h);
    }
    void idleDraw(const unsigned int time_ms) {
        if (activeInterface != NULL) {
            int actionID = activeInterface->actionReturn();
            if (actionID >= 0) {
                //pode fazer alguma coisa com o gerenciador de interfaces
                activeInterface->setActionReturn(-1);//reset do action antigo
                //trocar de interface
                switch (actionID) {
                case 3: //saveAndApplyOptions and Go to main menu
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    music.setGain(config->getFloat(SOUND_CONFIG_BRANCH, MUSIC_LEVEL, 1.0));
                    {
                        float Fxlvl = config->getFloat(SOUND_CONFIG_BRANCH, FX_LEVEL, 1.0);
                        click.setGain(Fxlvl);
                        hover.setGain(Fxlvl);
                    }
                case 0: //mainmenu
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new MainMenu(cursor, config, &click, &hover, font);
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    music.changeStreamFile("resources/musicas/musica-menu.ogg");
                    roundMusic = false;
                    break;
                case 1: //game
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new TetrisGame(cursor, config, font);
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    music.stop();
                    roundMusic = true;
                    break;
                case 2: //soundMenu
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new MainOptions(cursor, config, &click, &hover, font);
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    //            music.stop();
                    roundMusic = false;
                    break;
                case 5://scores
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new ScoreMenu(cursor, config, &click, &hover, font, false, 0);
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    break;
                case 6://enter new score from game
                {
                    int scorePoint = ((TetrisGame*)activeInterface)->getScore();
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new ScoreMenu(cursor, config, &click, &hover, font, true, scorePoint);
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                }
                break;
                }
            }
            else {
                if (!music.isPlaying()) {
                    if (roundMusic) {
                        static int count = 0;


                        const char *m[] = {
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/01-WhyWeAreInARush.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/02-TheWorldOfMyIlusion.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/03-DigitalHeart.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/04-ThePathWeMakeWalking.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/05-Together.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/06-NotesThatWispersToMyEars.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/07-SunIsMovingToSunset.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/08-HeartInMyMouth.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/09-WithoutPressure.ogg",
                            "resources/AlessandroRibeiro/APieceOfMyHeartPt1/ogg/10-Anxiety.ogg"};


                        count = (count + 1) % 10;

                        music.stop();
                        music.changeStreamFile(m[count]);
                    }
                    else
                        music.play();
                }
                else
                    music.checkBuffersRoundUp();
            }
            activeInterface->render(time_ms);
        }
    }
    void keyDown(const int key) {
        if (activeInterface != NULL)
            activeInterface->keyDown(key);
    }
    void keyUp(const int key) {
        if (activeInterface != NULL)
            activeInterface->keyUp(key);
    }
    void mouseDown(const int btn) {
        if (activeInterface != NULL)
            activeInterface->mouseDown(btn, cursorPos);
    }
    void mouseUp(const int btn) {
        if (activeInterface != NULL)
            activeInterface->mouseUp(btn, cursorPos);
    }
    void mouseMove(const int x, const int y) {
        //cursorPos = clamp(cursorPos + vec2(x,y), vec2(0,0), dimension);
        cursorPos = vec2(x, dimension.y - y);
        if (activeInterface != NULL)
            activeInterface->setMousePos(cursorPos);
    }

    SSE2_CLASS_NEW_OPERATOR
};

