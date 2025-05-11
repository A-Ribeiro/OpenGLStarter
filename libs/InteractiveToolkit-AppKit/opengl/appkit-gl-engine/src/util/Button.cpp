#include <appkit-gl-engine/util/Button.h>

namespace AppKit
{
	namespace GLEngine
	{

		Button::Button(int _position,
			bool _left,
			const std::string& _id,
			const std::string& _text,
			AppKit::OpenGL::GLFont2Builder* _fontBuilder)
		{

			position = _position;
			left = _left;
			id = _id;
			fontBuilder = _fontBuilder;

			// rendered_text = GlobalButtonState.StateToString(text);
			rendered_text = _text;

			root = Transform::CreateShared();

			materialBackground = nullptr;
			componentFontToMesh = nullptr;

			// resize(AppKit::GLEngine::Engine::Instance()->app->WindowSize);
			auto renderWindowRegion = ToShared(root->renderWindowRegion);
			resize(MathCore::vec2i(renderWindowRegion->CameraViewport.c_ptr()->w, renderWindowRegion->CameraViewport.c_ptr()->h));

			selected = false;
		}

		void Button::update(const MathCore::vec3f& mousePosition)
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

		void Button::updateText(const std::string& newText)
		{
			rendered_text = newText;
			auto renderWindowRegion = ToShared(root->renderWindowRegion);
			resize(MathCore::vec2i(renderWindowRegion->CameraViewport.c_ptr()->w, renderWindowRegion->CameraViewport.c_ptr()->h));
			// resize(AppKit::GLEngine::Engine::Instance()->app->WindowSize);
		}

		void Button::resize(const MathCore::vec2i& size)
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

			// if (materialBackground == nullptr || componentFontToMesh == nullptr)
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
				float size = 32.0f / fontBuilder->glFont2.size;
				xmin *= size;
				xmax *= size; 
				ymin *= size; 
				ymax *= size;

				float backMargin = 9.0f;
				xmin -= backMargin;
				xmax += backMargin;
				ymin -= backMargin;
				ymax += backMargin;

				aabb = CollisionCore::AABB<MathCore::vec3f>(MathCore::vec3f(xmin, ymin, -1), MathCore::vec3f(xmax, ymax, 1));
				aabb.min_box += root->getLocalPosition();
				aabb.max_box += root->getLocalPosition();

				// back square
				if (materialBackground == nullptr)
				{
					auto cursorNode = root->addChild(Transform::CreateShared());
					materialBackground = cursorNode->addNewComponent<Components::ComponentMaterial>();
					cursorNode->addComponent(Components::ComponentMesh::createPlaneXY(xmax - xmin, ymax - ymin));
					// cursorNode->LocalRotation = MathCore::GEN<MathCore::quatf>::fromEuler(MathCore::OP<float>::deg_2_rad(-90.0f), 0, 0);
					cursorNode->LocalPosition = MathCore::vec3f((xmin + xmax) * 0.5f, (ymin + ymax) * 0.5f, 0);

					materialBackground->type = Components::MaterialUnlit;
					materialBackground->unlit.blendMode = BlendModeAlpha;
					materialBackground->unlit.color = MathCore::vec4f(0, 0, 0, 0.5);
				}
				else
				{
					//ReferenceCounter<AppKit::GLEngine::Component *> *refCounter = &Engine::Instance()->componentReferenceCounter;

					auto cursorNode = materialBackground->getTransform();

					auto _mesh = cursorNode->removeComponent(cursorNode->findComponent<Components::ComponentMesh>());

					// refCounter->remove(_mesh);

					cursorNode->addComponent(Components::ComponentMesh::createPlaneXY(xmax - xmin, ymax - ymin));
					// cursorNode->LocalRotation = MathCore::GEN<MathCore::quatf>::fromEuler(MathCore::OP<float>::deg_2_rad(-90.0f), 0, 0);
					cursorNode->LocalPosition = MathCore::vec3f((xmin + xmax) * 0.5f, (ymin + ymax) * 0.5f, 0);
				}
				// font
				if (componentFontToMesh == nullptr)
				{
					auto textTransform = root->addChild(Transform::CreateShared());
					componentFontToMesh = textTransform->addNewComponent<Components::ComponentFontToMesh>();
					textTransform->setLocalScale(MathCore::vec3f(size, size, 1));
				}
				fontBuilder->build(rendered_text.c_str());
				componentFontToMesh->toMesh(*fontBuilder, true);
			}
		}

		std::shared_ptr<Transform> Button::getTransform()
		{
			return root;
		}

	}
}