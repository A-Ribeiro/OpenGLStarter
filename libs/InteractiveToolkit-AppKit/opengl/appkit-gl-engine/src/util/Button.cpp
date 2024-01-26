#include <appkit-gl-engine/util/Button.h>

namespace AppKit
{
    namespace GLEngine
    {

        Button::Button(int _position,
                       bool _left,
                       const std::string &_id,
                       const std::string &_text,
                       AppKit::OpenGL::GLFont2Builder *_fontBuilder)
        {

            position = _position;
            left = _left;
            id = _id;
            fontBuilder = _fontBuilder;

            // rendered_text = GlobalButtonState.StateToString(text);
            rendered_text = _text;

            root = new Transform();

            materialBackground = NULL;
            componentFontToMesh = NULL;

            // resize(AppKit::GLEngine::Engine::Instance()->app->WindowSize);

            resize(MathCore::vec2i(root->renderWindowRegion->Viewport.c_ptr()->w, root->renderWindowRegion->Viewport.c_ptr()->h));

            selected = false;
        }

        void Button::update(const MathCore::vec3f &mousePosition)
        {
            if (CollisionCore::AABB<MathCore::vec3f>::pointInsideAABB(mousePosition, aabb))
            {
                materialBackground->unlit.color = MathCore::vec4f(1, 1, 0, 0.5);
                selected = true;
            }
            else
            {
                materialBackground->unlit.color = MathCore::vec4f(0, 0, 0, 0.5);
                selected = false;
            }
        }

        void Button::updateText(const std::string &newText)
        {
            rendered_text = newText;
            resize(MathCore::vec2i(root->renderWindowRegion->Viewport.c_ptr()->w, root->renderWindowRegion->Viewport.c_ptr()->h));
            // resize(AppKit::GLEngine::Engine::Instance()->app->WindowSize);
        }

        void Button::resize(const MathCore::vec2i &size)
        {
            float margin = 32.0f;
            float indexSpacing = 32.0f + 10.0f;

            float centerx = (float)size.x / 2.0f;
            float centery = -(float)size.y / 2.0f;

            float spacingIndex = indexSpacing * position;

            if (left)
            {
                root->setLocalPosition(MathCore::vec3f(margin - centerx, -centery - (margin + spacingIndex), 0));
            }
            else
            {
                root->setLocalPosition(MathCore::vec3f(centerx - margin, -centery - (margin + spacingIndex), 0));
            }

            // if (materialBackground == NULL || componentFontToMesh == NULL)
            {
                float xmin, xmax, ymin, ymax;

                if (left)
                {
                    fontBuilder->faceColor = MathCore::vec4f(1, 1, 1, 1);
                    fontBuilder->strokeColor = MathCore::vec4f(0.25, 0.25, 0.25, 1);
                    fontBuilder->horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_left;
                    fontBuilder->verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_top;
                    fontBuilder->strokeOffset = MathCore::vec3f(0, 0, -0.001f);
                    fontBuilder->drawFace = true;
                    fontBuilder->drawStroke = false;
                    fontBuilder->computeBox(rendered_text.c_str(), &xmin, &xmax, &ymin, &ymax);
                }
                else
                {
                    fontBuilder->faceColor = MathCore::vec4f(1, 1, 1, 1);
                    fontBuilder->strokeColor = MathCore::vec4f(0.25, 0.25, 0.25, 1);
                    fontBuilder->horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_right;
                    fontBuilder->verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_top;
                    fontBuilder->strokeOffset = MathCore::vec3f(0, 0, -0.001f);
                    fontBuilder->drawFace = true;
                    fontBuilder->drawStroke = false;
                    fontBuilder->computeBox(rendered_text.c_str(), &xmin, &xmax, &ymin, &ymax);
                }
                float backMargin = 9.0f;
                xmin -= backMargin;
                xmax += backMargin;
                ymin -= backMargin;
                ymax += backMargin;

                aabb = CollisionCore::AABB<MathCore::vec3f>(MathCore::vec3f(xmin, ymin, -1), MathCore::vec3f(xmax, ymax, 1));
                aabb.min_box += root->getLocalPosition();
                aabb.max_box += root->getLocalPosition();

                // back square
                if (materialBackground == NULL)
                {
                    Transform *cursorNode = root->addChild(new Transform());
                    cursorNode->addComponent(materialBackground = new Components::ComponentMaterial());
                    cursorNode->addComponent(Components::ComponentMesh::createPlaneXY(xmax - xmin, ymax - ymin));
                    // cursorNode->LocalRotation = MathCore::GEN<MathCore::quatf>::fromEuler(MathCore::OP<float>::deg_2_rad(-90.0f), 0, 0);
                    cursorNode->LocalPosition = MathCore::vec3f((xmin + xmax) * 0.5f, (ymin + ymax) * 0.5f, 0);

                    materialBackground->type = Components::MaterialUnlit;
                    materialBackground->unlit.blendMode = BlendModeAlpha;
                    materialBackground->unlit.color = MathCore::vec4f(0, 0, 0, 0.5);
                }
                else
                {
                    ReferenceCounter<AppKit::GLEngine::Component *> *refCounter = &Engine::Instance()->componentReferenceCounter;

                    Transform *cursorNode = materialBackground->transform[0];

                    Component *_mesh = cursorNode->removeComponent(cursorNode->findComponent(Components::ComponentMesh::Type));

                    refCounter->remove(_mesh);

                    cursorNode->addComponent(Components::ComponentMesh::createPlaneXY(xmax - xmin, ymax - ymin));
                    // cursorNode->LocalRotation = MathCore::GEN<MathCore::quatf>::fromEuler(MathCore::OP<float>::deg_2_rad(-90.0f), 0, 0);
                    cursorNode->LocalPosition = MathCore::vec3f((xmin + xmax) * 0.5f, (ymin + ymax) * 0.5f, 0);
                }
                // font
                if (componentFontToMesh == NULL)
                {
                    Transform *textTransform = root->addChild(new Transform());
                    componentFontToMesh = (Components::ComponentFontToMesh *)textTransform->addComponent(new Components::ComponentFontToMesh());
                }
                fontBuilder->build(rendered_text.c_str());
                componentFontToMesh->toMesh(*fontBuilder, true);
            }
        }

        Transform *Button::getTransform()
        {
            return root;
        }

    }
}