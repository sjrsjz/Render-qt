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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glColor4f(background_color[0], background_color[1], background_color[2], background_color[3]);
	glVertex3f(WorldPos[0],WorldPos[1],WorldPos[2]);
	glVertex3f(WorldPos[3], WorldPos[4], WorldPos[5]);
	glVertex3f(WorldPos[6], WorldPos[7], WorldPos[8]);
	glVertex3f(WorldPos[9], WorldPos[10], WorldPos[11]);
	glEnd();
	glDisable(GL_BLEND);
	glPopMatrix();

}
void GLButton::UpdateText(std::wstring text) {
	this->text = text;
	for (auto& x : text) {
		if (_GLCharacterList.list.find(x)== _GLCharacterList.list.end())
		{
			SelectObject(hDC, GLhFont);
			int list = glGenLists(1);
			_GLCharacterList.list[x] = list;
			//LPGLYPHMETRICSFLOAT  lpGLyphMetricsFloat;
			wglUseFontOutlinesW(hDC, x, 1, list, 0.0f, 0.2f, WGL_FONT_POLYGONS, NULL);
		}
	}
}