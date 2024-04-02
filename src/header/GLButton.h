#pragma once
#include "Common.h"


class GLButton {
public:
	QOpenGLContext* context{};
	
	GLRECT rect;
	std::wstring text;
	float size = 1.0f;
	float text_color[4];
	float background_color[4];
	bool visible = true;
	HDC hDC{};
	GLButton();
	~GLButton();
	bool TestClick(float x, float y);
	void SetRect(float x, float y, float w, float h);
	void SetRect(RECT rect);
	void MoveTo(float x, float y);
	void MoveBy(float x, float y);
	void Resize(float w, float h);
	void ResizeBy(float w, float h);
	void Draw();
	void UpdateText(std::wstring text);
};