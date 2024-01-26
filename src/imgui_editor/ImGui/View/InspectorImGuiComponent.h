#pragma once

#include "common.h"

class InspectorImGuiComponent {
protected:
	virtual void childRenderAndLogic() = 0;
	InspectorImGuiComponent(const std::string &title);
public:
	std::string title;
	bool opened;

	virtual ~InspectorImGuiComponent();

	void renderAndLogic(int count);
};

class InspectorImGuiComponent_Transform : public InspectorImGuiComponent {
protected:
	void childRenderAndLogic();
public:


	float p[4] = { 0,1,2,3 };
	float r[4] = { 0,1,2,3 };
	float s[4] = { 0,1,2,3 };

	InspectorImGuiComponent_Transform();

};

