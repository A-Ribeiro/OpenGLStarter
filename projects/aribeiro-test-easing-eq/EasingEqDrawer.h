#ifndef easingeq__h
#define easingeq__h


//
// Example of using with vec2:
//
// easeOutBounce<vec2>(vec2(1, 0), vec2(3, 5), 0.5);
//

#include "PointRenderer.h"
#include "LineRenderer.h"

typedef float (*interplFunc) (const float& _startValue, const float& _endValue, float lerp);

class EasingEqDrawer {

    PointRenderer points;
    float lrp;

    LineRenderer lineSquare;
    LineRenderer lineGraphic;

    GLSquare square;

    public:

    EasingEqDrawer():points(40), lineSquare(4), lineGraphic(256) {

        lineSquare.data[0] = vec3(0,0,0);
        lineSquare.data[1] = vec3(1,0,0);
        lineSquare.data[2] = vec3(1,1,0);
        lineSquare.data[3] = vec3(0,1,0);

        lrp = 0.0f;

    }

    void draw(interplFunc interpolationfunction, GLShaderColor * shader, GLFont *font , const mat4 &mvp, const wchar_t * text){

        // compute buffers
        for (int i=0;i<(int)lineGraphic.data.size();i++){
            float lerp = (float)i/float(lineGraphic.data.size()-1);
            lineGraphic.data[i] = vec3(lerp, interpolationfunction(0,1,lerp), 0.0f );
        }

        shader->enable();

        //
        // draw border
        //
        shader->setMatrix(mvp);
        shader->setColor( vec4(vec3(0.0f,0.0f,0.0f),1.0f) );
        lineSquare.drawLoop(shader);

        //
        // draw chart
        //
        shader->setColor( vec4(vec3(1.0f,0.6f,0.2f),1.0f) );
        lineGraphic.draw(shader);

        //draw font
        const mat4 fontTransform = translate(0.5f, 1.0f, 0) * scale(0.0025f, 0.0025f, 0.0025f);

        collision::AABB aabb = font->computeBounds(text);
        vec3 center = -aabb.max_box * 0.5f;
        center.y += font->getLineHeight();

        font->getShader()->enable();
        font->getShader()->setMatrix(mvp * fontTransform * translate(center));
        font->print(vec3(0.0f, 0.0f, 0.0f), text );

        //back original shader
        shader->enable();

    }

    void drawAllFunctions(const mat4 &baseMatrix, GLShaderColor * shader, GLFont *font, PlatformTime *time) {

        //mat4 base_inv = inv_faster(baseMatrix);
        mat4 base_inv = inv(baseMatrix);

        lrp += time->deltaTime;
        lrp = fmod(lrp, 1.0f);

        //
        // draw background
        //
        shader->setMatrix(baseMatrix * scale(1.2f, 1.0f, 1.0f));
        shader->setColor(vec4(vec3(0.9f, 0.9f, 0.9f), 1.0f));
        square.draw(shader);

        mat4 mvp = baseMatrix * scale(0.25, 0.25, 1.0) * translate(-5+0.5f+0.25f, -3.5, 0);

        mat4 aux;

        const interplFunc funcs[] = {
            easeInQuad,     easeOutQuad,    easeInOutQuad,      easeOutInQuad,
            easeInCubic,    easeOutCubic,   easeInOutCubic,     easeOutInQuad,
            easeInQuart,    easeOutQuart,   easeInOutQuart,     easeOutInQuart,
            easeInQuint,    easeOutQuint,   easeInOutQuint,     easeOutInQuint,
            easeInSine,     easeOutSine,    easeInOutSine,      easeOutInSine,
            easeInExpo,     easeOutExpo,    easeInOutExpo,      easeOutInExpo,
            easeInCirc,     easeOutCirc,    easeInOutCirc,      easeOutInCirc,
            easeInElastic,  easeOutElastic, easeInOutElastic,   easeOutInElastic,
            easeInBack,     easeOutBack,    easeInOutBack,      easeOutInBack,
            easeInBounce,   easeOutBounce,  easeInOutBounce,    easeOutInBounce
        };

        const wchar_t *funcs_names[] = {
            L"easeInQuad",     L"easeOutQuad",    L"easeInOutQuad",      L"easeOutInQuad",
            L"easeInCubic",    L"easeOutCubic",   L"easeInOutCubic",     L"easeOutInQuad",
            L"easeInQuart",    L"easeOutQuart",   L"easeInOutQuart",     L"easeOutInQuart",
            L"easeInQuint",    L"easeOutQuint",   L"easeInOutQuint",     L"easeOutInQuint",
            L"easeInSine",     L"easeOutSine",    L"easeInOutSine",      L"easeOutInSine",
            L"easeInExpo",     L"easeOutExpo",    L"easeInOutExpo",      L"easeOutInExpo",
            L"easeInCirc",     L"easeOutCirc",    L"easeInOutCirc",      L"easeOutInCirc",
            L"easeInElastic",  L"easeOutElastic", L"easeInOutElastic",   L"easeOutInElastic",
            L"easeInBack",     L"easeOutBack",    L"easeInOutBack",      L"easeOutInBack",
            L"easeInBounce",   L"easeOutBounce",  L"easeInOutBounce",    L"easeOutInBounce"
        };

        int total = 40;// (sizeof(funcs) / sizeof(interplFunc));

        glLineWidth(2.0f);
        //
        // draw charts
        //
        for (int i = 0; i < total; i += 8) {
            aux = mvp;
            for (int j = 0; j < 8; j++) {
                draw(funcs[i + j], shader, font, aux, funcs_names[i + j]);

                vec4 ptn = vec4(lrp, funcs[i + j](0.0f, 1.0f, lrp), 0.0f, 1.0f);
                ptn = (base_inv * aux) * ptn;

                points.data[i + j] = toVec3( ptn );

                aux = aux * translate(1.0f, 0, 0);
                if (j == 3)
                    aux = aux * translate(0.5f, 0, 0);

            }
            mvp = mvp * translate(0, 1.5f, 0);
        }

        //
        // draw points
        //
        shader->setMatrix(baseMatrix);
        shader->setColor(vec4(0.0f,0.0f,1.0f,1.0f));
        points.draw(shader);

    }
};



#endif
