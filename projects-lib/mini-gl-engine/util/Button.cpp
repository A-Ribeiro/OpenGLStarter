#include "Button.h"

namespace GLEngine {

    Button::Button(int _position, 
        bool _left, 
        const std::string &_id, 
        const std::string &_text, 
        openglWrapper::GLFont2Builder *_fontBuilder) {
            
        position = _position;
        left = _left;
        id = _id;
        fontBuilder = _fontBuilder;
        
        //rendered_text = GlobalButtonState.StateToString(text);
        rendered_text = _text;
        
        root = new Transform();
        
        materialBackground = NULL;
        componentFontToMesh = NULL;
        
        resize(GLEngine::Engine::Instance()->app->WindowSize);
        
        selected = false;
    }
    
    void Button::update(const aRibeiro::vec3 &mousePosition) {
        if (aRibeiro::collision::AABB::pointInsideAABB(mousePosition, aabb)){
            materialBackground->unlit.color = aRibeiro::vec4(1,1,0,0.5);
            selected = true;
        }else{
            materialBackground->unlit.color = aRibeiro::vec4(0,0,0,0.5);
            selected = false;
        }
    }
    
    void Button::updateText(const std::string &newText) {
        rendered_text = newText;
        resize(GLEngine::Engine::Instance()->app->WindowSize);
    }
    
    void Button::resize(const sf::Vector2i &size){
        float margin = 32.0f;
        float indexSpacing = 32.0f + 10.0f;
        
        float centerx = (float)size.x/2.0f;
        float centery = -(float)size.y/2.0f;
        
        float spacingIndex = indexSpacing * position;
        
        if (left){
            root->setLocalPosition(aRibeiro::vec3(margin-centerx,-centery - (margin+spacingIndex),0));
        } else {
            root->setLocalPosition(aRibeiro::vec3(centerx-margin,-centery - (margin+spacingIndex),0));
        }
        
        //if (materialBackground == NULL || componentFontToMesh == NULL)
        {
            float xmin,xmax,ymin,ymax;
            
            if (left) {
                fontBuilder->faceColor = aRibeiro::vec4(1, 1, 1, 1);
                fontBuilder->strokeColor = aRibeiro::vec4(0.25, 0.25, 0.25, 1);
                fontBuilder->horizontalAlign = openglWrapper::GLFont2HorizontalAlign_left;
                fontBuilder->verticalAlign = openglWrapper::GLFont2VerticalAlign_top;
                fontBuilder->strokeOffset = aRibeiro::vec3(0, 0, -0.001f);
                fontBuilder->drawFace = true;
                fontBuilder->drawStroke = false;
                fontBuilder->computeBox(rendered_text.c_str(), &xmin, &xmax, &ymin, &ymax);
            }else{
                fontBuilder->faceColor = aRibeiro::vec4(1, 1, 1, 1);
                fontBuilder->strokeColor = aRibeiro::vec4(0.25, 0.25, 0.25, 1);
                fontBuilder->horizontalAlign = openglWrapper::GLFont2HorizontalAlign_right;
                fontBuilder->verticalAlign = openglWrapper::GLFont2VerticalAlign_top;
                fontBuilder->strokeOffset = aRibeiro::vec3(0, 0, -0.001f);
                fontBuilder->drawFace = true;
                fontBuilder->drawStroke = false;
                fontBuilder->computeBox(rendered_text.c_str(), &xmin, &xmax, &ymin, &ymax);
            }
            float backMargin = 9.0f;
            xmin-=backMargin;
            xmax+=backMargin;
            ymin-=backMargin;
            ymax+=backMargin;
            
            aabb = aRibeiro::collision::AABB(aRibeiro::vec3(xmin,ymin,-1),aRibeiro::vec3(xmax,ymax,1));
            aabb.min_box += root->getLocalPosition();
            aabb.max_box += root->getLocalPosition();
            
            // back square
            if (materialBackground == NULL)
            {
                Transform *cursorNode = root->addChild(new Transform());
                cursorNode->addComponent(materialBackground = new Components::ComponentMaterial());
                cursorNode->addComponent(Components::ComponentMesh::createPlaneXY(xmax-xmin,ymax-ymin));
                //cursorNode->LocalRotation = aRibeiro::quatFromEuler(DEG2RAD(-90.0f), 0, 0);
                cursorNode->LocalPosition = aRibeiro::vec3( (xmin+xmax)*0.5f,(ymin+ymax)*0.5f,0 );
                
                materialBackground->type = Components::MaterialUnlit;
                materialBackground->unlit.blendMode = BlendModeAlpha;
                materialBackground->unlit.color = aRibeiro::vec4(0,0,0,0.5);
            } else {
                ReferenceCounter<GLEngine::Component*> *refCounter = &Engine::Instance()->componentReferenceCounter;

                Transform *cursorNode = materialBackground->transform[0];

                Component *_mesh = cursorNode->removeComponent(cursorNode->findComponent(Components::ComponentMesh::Type));
                
                refCounter->remove(_mesh);


                cursorNode->addComponent(Components::ComponentMesh::createPlaneXY(xmax-xmin,ymax-ymin));
                //cursorNode->LocalRotation = aRibeiro::quatFromEuler(DEG2RAD(-90.0f), 0, 0);
                cursorNode->LocalPosition = aRibeiro::vec3( (xmin+xmax)*0.5f,(ymin+ymax)*0.5f,0 );
            }
            //font
            if (componentFontToMesh == NULL)
            {
                Transform *textTransform = root->addChild(new Transform());
                componentFontToMesh = (Components::ComponentFontToMesh*)textTransform->addComponent(new Components::ComponentFontToMesh());
                
            }
            fontBuilder->build(rendered_text.c_str());
            componentFontToMesh->toMesh(*fontBuilder, true);
        }
        
    }
    
    Transform * Button::getTransform(){
        return root;
    }

}
