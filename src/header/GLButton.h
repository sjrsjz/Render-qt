#pragma once
#include "Common.h"


class GLButton {
private:
	float TextAreaW{};
	float TextAreaH{};
	unsigned int TabSize = 4;
	float LineSpace=0.5;
	std::vector<std::vector<float>> TextPosX{};
	std::vector<std::vector<float>> TextPosY{};

	std::vector<float> TextPosX_L{};
	std::vector<float> TextPosY_L{};

	std::vector<float> TextCenterY{};


	std::vector<std::vector<float>> TextSizeW{};
	std::vector<float> TextSizeH{};

	void GenText(wchar_t c);
public:
	QOpenGLContext* context{};
	
	GLRECT rect;
	std::wstring text;
	float size = 1.0f;
	float text_color[4];
	float background_color[4];
	bool visible = true;
	bool LeftAlign = false;
	bool TopAlign = false;
	bool YAlign = false;
	float YOffset = 0.275;
	float XOffset = 0.275;
	GLuint BlendFactorS = GL_SRC_ALPHA;
	GLuint BlendFactorD = GL_ONE_MINUS_SRC_ALPHA;
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