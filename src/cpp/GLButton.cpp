#include "../header/GLButton.h"
GLButton::GLButton() {
	rect.x = 0;
	rect.y = 0;
	rect.w = 0;
	rect.h = 0;
	text = L"";
	text_color[0] = 0.0f;
	text_color[1] = 0.0f;
	text_color[2] = 0.0f;
	text_color[3] = 1.0f;
	background_color[0] = 1.0f;
	background_color[1] = 1.0f;
	background_color[2] = 1.0f;
	background_color[3] = 1.0f;
}
GLButton::~GLButton() {}

bool GLButton::TestClick(float x, float y) {
	if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h) {
		return true;
	}
	return false;
}
void GLButton::SetRect(float x, float y, float w, float h) {
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
}
void GLButton::SetRect(RECT rect) {
	this->rect.x = rect.left;
	this->rect.y = rect.top;
	this->rect.w = rect.right - rect.left;
	this->rect.h = rect.bottom - rect.top;
}
void GLButton::MoveTo(float x, float y) {
	rect.x = x;
	rect.y = y;
}
void GLButton::MoveBy(float x, float y) {
	rect.x += x;
	rect.y += y;
}
void GLButton::Resize(float w, float h) {
	rect.w = w;
	rect.h = h;
}
void GLButton::ResizeBy(float w, float h) {
	rect.w += w;
	rect.h += h;
}
void GLButton::Draw() {
	if (!visible || !context) return;

	glPushMatrix();
	glLoadIdentity();
	GLdouble ScreenPos[8];
	GLdouble WorldPos[12];
	ScreenPos[0] = rect.x;
	ScreenPos[1] = rect.y;
	ScreenPos[2] = rect.x;
	ScreenPos[3] = rect.y + rect.h;
	ScreenPos[4] = rect.x + rect.w;
	ScreenPos[5] = rect.y + rect.h;
	ScreenPos[6] = rect.x + rect.w;
	ScreenPos[7] = rect.y;
	//transform to world pos
	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(ScreenPos[0], ScreenPos[1], 0.0f, modelview, projection, viewport, &WorldPos[0], &WorldPos[1], &WorldPos[2]);
	gluUnProject(ScreenPos[2], ScreenPos[3], 0.0f, modelview, projection, viewport, &WorldPos[3], &WorldPos[4], &WorldPos[5]);
	gluUnProject(ScreenPos[4], ScreenPos[5], 0.0f, modelview, projection, viewport, &WorldPos[6], &WorldPos[7], &WorldPos[8]);
	gluUnProject(ScreenPos[6], ScreenPos[7], 0.0f, modelview, projection, viewport, &WorldPos[9], &WorldPos[10], &WorldPos[11]);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(BlendMode, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glColor4f(background_color[0], background_color[1], background_color[2], background_color[3]);
	glVertex3f(WorldPos[0],WorldPos[1],WorldPos[2]);
	glVertex3f(WorldPos[3], WorldPos[4], WorldPos[5]);
	glVertex3f(WorldPos[6], WorldPos[7], WorldPos[8]);
	glVertex3f(WorldPos[9], WorldPos[10], WorldPos[11]);
	glEnd();
	
	glPushMatrix();
	glLoadIdentity();
	//compute unit
	GLdouble UnitPos[6];
	gluUnProject(0, 0, 0.0f, modelview, projection, viewport, &UnitPos[0], &UnitPos[1], &UnitPos[2]);
	gluUnProject(size, 0, 0.0f, modelview, projection, viewport, &UnitPos[3], &UnitPos[4], &UnitPos[5]);
	float unit = sqrt((UnitPos[3] - UnitPos[0]) * (UnitPos[3] - UnitPos[0]) + (UnitPos[4] - UnitPos[1]) * (UnitPos[4] - UnitPos[1]));

	if (LeftAlign) {
		//move to WorldPos center
		glTranslatef(WorldPos[0], (WorldPos[1] + WorldPos[4]) / 2, 0);
	}
	else {
		//move to WorldPos left
		glTranslatef((WorldPos[0] + WorldPos[9]) / 2, (WorldPos[1] + WorldPos[4]) / 2, 0);
	}

	if (TopAlign) {
		//move to WorldPos top
		glTranslatef(0, (-WorldPos[1] + WorldPos[4]) / 2 - unit, 0);
	}
	else {
		//move to WorldPos center
		glTranslatef(0, 0, 0);
	}
	
	//scale to unit
	glScalef(unit, unit, unit);
	glTranslatef(0, 0, -0.5f);
	//draw text

	
	if (LeftAlign) 
		glTranslatef(XOffset, 0, 0);
	else 
		glTranslatef(-TextAreaW / 2, 0, 0);

	size_t i = 0;
	for (auto&x:text) {
		glPushMatrix();
		glTranslatef(TextPosX_L[i], TextPosY_L[i] - (YAlign ? TextCenterY[i] : YOffset), 0);
		glColor4f(text_color[0], text_color[1], text_color[2], text_color[3]);
		glCallList(_GLCharacterList.list[x]);
		glPopMatrix();
		i++;
	}
	glPopMatrix();

	glDisable(GL_BLEND);
	glPopMatrix();

}

void GLButton::GenText(wchar_t x) {
	SelectObject(hDC, GLhFont);
	int list = glGenLists(1);
	_GLCharacterList.list[x] = list;
	GLYPHMETRICSFLOAT  lpGLyphMetricsFloat;
	wglUseFontOutlinesW(hDC, x, 1, list, 0.0f, 0.01f, WGL_FONT_POLYGONS, &lpGLyphMetricsFloat);
	_GLCharacterList.width[x] = lpGLyphMetricsFloat.gmfBlackBoxX;
	_GLCharacterList.height[x] = lpGLyphMetricsFloat.gmfBlackBoxY;
	_GLCharacterList.dx[x] = lpGLyphMetricsFloat.gmfCellIncX;
	_GLCharacterList.dy[x] = lpGLyphMetricsFloat.gmfCellIncY;
	_GLCharacterList.cx[x] = lpGLyphMetricsFloat.gmfptGlyphOrigin.x - lpGLyphMetricsFloat.gmfBlackBoxX / 2;
	_GLCharacterList.cy[x] = lpGLyphMetricsFloat.gmfptGlyphOrigin.y - lpGLyphMetricsFloat.gmfBlackBoxY / 2;
}

void GLButton::UpdateText(std::wstring text) {
	this->text = text;
	TextPosX.clear();
	TextPosY.clear();
	TextSizeW.clear();
	TextCenterY.clear();
	TextSizeH.clear();
	TextPosX_L.clear();
	TextPosY_L.clear();

	
	std::vector<float> linePosX{};
	std::vector<float> linePosY{};
	std::vector<float> lineSizeW{};

	float CurrY = 0;
	float CurrX = 0;
	float Max_dy = 0;


	if (_GLCharacterList.list.find(L' ') == _GLCharacterList.list.end())
	{
		GenText(L' ');
	}


	for (auto& x : text) {
		if (_GLCharacterList.list.find(x) == _GLCharacterList.list.end())
		{
			switch (x)
			{
			case L'\n':
			case L'\t':
				_GLCharacterList.list[x] = -1;
				break;
			default:
				GenText(x);
				break;
			}
		}
		TextCenterY.push_back(_GLCharacterList.cy[x]);
		switch (x)
		{
		case L'\n':
			linePosX.push_back(CurrX);
			linePosY.push_back(CurrY);
			lineSizeW.push_back(0);
			TextPosX.push_back(linePosX);
			TextPosY.push_back(linePosY);
			TextPosX_L.insert(TextPosX_L.end(), linePosX.begin(), linePosX.end());
			TextPosY_L.insert(TextPosY_L.end(), linePosY.begin(), linePosY.end());
			TextSizeW.push_back(lineSizeW);
			TextSizeH.push_back(Max_dy);
			linePosX.clear();
			linePosY.clear();
			lineSizeW.clear();
			CurrY -= Max_dy + LineSpace;
			CurrX = 0;
			Max_dy = 0;
			break;
		case L'\t':
			linePosX.push_back(CurrX);
			linePosY.push_back(CurrY);
			CurrX += _GLCharacterList.dx[L' '] * TabSize;
			if (_GLCharacterList.height[L' '] > Max_dy) Max_dy = _GLCharacterList.height[L' '];
			lineSizeW.push_back(_GLCharacterList.dx[L' '] * TabSize);
			break;
		default:
			linePosX.push_back(CurrX);
			linePosY.push_back(CurrY);
			CurrX += _GLCharacterList.dx[x];
			if (_GLCharacterList.height[x] > Max_dy) Max_dy = _GLCharacterList.height[x];
			lineSizeW.push_back(_GLCharacterList.dx[x]);
			break;
		}		
	}
	linePosX.push_back(CurrX);
	linePosY.push_back(CurrY);
	lineSizeW.push_back(0);
	TextPosX.push_back(linePosX);
	TextPosY.push_back(linePosY);
	TextPosX_L.insert(TextPosX_L.end(), linePosX.begin(), linePosX.end());
	TextPosY_L.insert(TextPosY_L.end(), linePosY.begin(), linePosY.end());
	TextSizeW.push_back(lineSizeW);
	TextSizeH.push_back(Max_dy);

	//compute TextArea
	float W{}, H{};
	for (size_t i{}; i < TextPosX.size(); i++) {
		if (TextPosX[i].size()) {
			float W0 = TextPosX[i][TextPosX[i].size() - 1] + TextSizeW[i][TextPosX[i].size() - 1];
			if (W0 > W) {
				W = W0;
			}
		}
	}
	H = CurrY + Max_dy;
	
	TextAreaH = H;
	TextAreaW = W;
}
	


