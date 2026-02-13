#include <appkit-gl-engine/Renderer/Fade.h>

#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/GL/GLRenderState.h>

// #include <appkit-gl-engine/mini-gl-engine.h>

namespace AppKit
{
	namespace GLEngine
	{

		void Fade::setColor(Platform::Time* time)
		{

			if (completeOnNextFrame)
			{
				AppBase* app = Engine::Instance()->app;

				if (auto eventHandlerSet = eventHandlerSetRef.lock())
					eventHandlerSet->OnUpdate.remove(&Fade::setColor, this);

				if (reset_draw_visible)
					draw_visible = false;
				isFading = false;
				time->timeScale = oldTimeScale;

				if (OnEndCall != nullptr)
					OnEndCall();
				return;
			}

			lrp = MathCore::OP<float>::move(lrp, 1.0f, time->unscaledDeltaTime / sec);
			color = MathCore::OP<MathCore::vec4f>::lerp(colorSrc, colorTarget, lrp);

			// textTransform->setLocalPosition( lerp(src, target,easeOutExpo(0.0f, 1.0f, lrp)) );
			if (lrp == 1.0f)
			{
				completeOnNextFrame = true;
			}
		}

		void Fade::createScreenVertex()
		{
			vertex.clear();

			vertex.push_back(MathCore::vec3f(-1, -1, 0));
			vertex.push_back(MathCore::vec3f(1, -1, 0));
			vertex.push_back(MathCore::vec3f(1, 1, 0));

			vertex.push_back(MathCore::vec3f(-1, -1, 0));
			vertex.push_back(MathCore::vec3f(1, 1, 0));
			vertex.push_back(MathCore::vec3f(-1, 1, 0));
		}

		Fade::Fade(Platform::Time* _time, std::shared_ptr<EventHandlerSet> eventHandlerSet)
		{
			this->eventHandlerSetRef = eventHandlerSet;
			// if (eventHandlerSet == nullptr)
			//     this->eventHandlerSetRef = AppKit::GLEngine::Engine::Instance()->app->screenRenderWindow;

			time = _time;
			createScreenVertex();
			isFading = false;
			draw_visible = false;
			completeOnNextFrame = false;
		}

		Fade::~Fade()
		{
			// AppBase *app = Engine::Instance()->app;
			if (auto eventHandlerSet = eventHandlerSetRef.lock())
				eventHandlerSet->OnUpdate.remove(&Fade::setColor, this);
		}

		void Fade::fadeIn(float _sec, const EventCore::Callback<void()>& _OnEndCall)
		{
			auto eventHandlerSet = eventHandlerSetRef.lock();

			if (!isFading)
				oldTimeScale = time->timeScale;

			time->timeScale = 0.0f;

			OnEndCall = _OnEndCall;
			AppKit::GLEngine::AppBase* app = AppKit::GLEngine::Engine::Instance()->app;
			if (eventHandlerSet != nullptr) {
				eventHandlerSet->OnUpdate.remove(&Fade::setColor, this);
				eventHandlerSet->OnUpdate.add(&Fade::setColor, this);
			}
			lrp = 0.0f;
			sec = _sec;
			colorSrc = MathCore::vec4f(0, 0, 0, 0);
			colorTarget = MathCore::vec4f(0, 0, 0, 1);
			color = MathCore::OP<MathCore::vec4f>::lerp(colorSrc, colorTarget, lrp);
			reset_draw_visible = false;
			draw_visible = true;
			isFading = true;
			completeOnNextFrame = false;
		}

		void Fade::fadeOut(float _sec, const EventCore::Callback<void()>& _OnEndCall)
		{
			auto eventHandlerSet = eventHandlerSetRef.lock();

			if (!isFading)
				oldTimeScale = time->timeScale;

			time->timeScale = 0.0f;

			OnEndCall = _OnEndCall;
			AppKit::GLEngine::AppBase* app = AppKit::GLEngine::Engine::Instance()->app;
			if (eventHandlerSet != nullptr) {
				eventHandlerSet->OnUpdate.remove(&Fade::setColor, this);
				eventHandlerSet->OnUpdate.add(&Fade::setColor, this);
			}
			lrp = 0.0f;
			sec = _sec;
			colorTarget = MathCore::vec4f(0, 0, 0, 0);
			colorSrc = MathCore::vec4f(0, 0, 0, 1);
			color = MathCore::OP<MathCore::vec4f>::lerp(colorSrc, colorTarget, lrp);
			reset_draw_visible = true;
			draw_visible = true;
			isFading = true;
			completeOnNextFrame = false;
		}

		void Fade::draw()
		{

			if (!draw_visible)
				return;

			AppKit::GLEngine::GLRenderState* renderstate = AppKit::GLEngine::GLRenderState::Instance();

			// AppKit::OpenGL::GLShader* oldShader = renderstate->CurrentShader;
			AppKit::GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
			bool oldDepthTestEnabled = renderstate->DepthWrite;
			AppKit::GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;

			renderstate->CurrentShader = &shaderColor;
			renderstate->DepthTest = AppKit::GLEngine::DepthTestDisabled;
			renderstate->DepthWrite = false;
			renderstate->BlendMode = AppKit::GLEngine::BlendModeAlpha;

			shaderColor.setMatrix(MathCore::mat4f());
			shaderColor.setColor(color);

			// direct draw commands
			OPENGL_CMD(glEnableVertexAttribArray(shaderColor.vPosition));
			OPENGL_CMD(glVertexAttribPointer(shaderColor.vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));

			OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

			OPENGL_CMD(glDisableVertexAttribArray(shaderColor.vPosition));

			// renderstate->CurrentShader = oldShader;
			renderstate->DepthTest = oldDepthTest;
			renderstate->DepthWrite = oldDepthTestEnabled;
			renderstate->BlendMode = oldBlendMode;
		}

	}

}
