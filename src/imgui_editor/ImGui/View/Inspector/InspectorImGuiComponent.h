#pragma once

#include "../common.h"

class InspectorImGuiComponent {
protected:
	virtual void childRenderAndLogic() = 0;
	InspectorImGuiComponent(const std::string &title);

	std::weak_ptr<InspectorImGuiComponent> mSelf;
public:

	std::string title;
	bool opened;

	virtual ~InspectorImGuiComponent();

	void renderAndLogic(int count);

	inline std::shared_ptr<InspectorImGuiComponent> self()
	{
		return AppKit::GLEngine::ToShared(mSelf);
	}

	template <typename _ComponentType, typename... _param_args,
				typename std::enable_if<
					std::is_base_of<InspectorImGuiComponent, _ComponentType>::value,
					bool>::type = true>
	static inline std::shared_ptr<_ComponentType> CreateShared(_param_args &&...args)
	{
		auto result = std::make_shared<_ComponentType>(std::forward<_param_args>(args)...);
		result->mSelf = std::weak_ptr<InspectorImGuiComponent>(result);
		return result;
	}

};

