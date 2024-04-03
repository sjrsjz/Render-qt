#include "../header/MainOpenGLWidget.h"


extern GLButton Main_Exit;
extern GLButton Main_Minimize;
extern GLButton Main_Maximize;
extern GLButton Main_TitleBar;

MainOpenGLWidget::MainOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
}
void MainOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GLhFont = CreateFontW(1, 1, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 2, 54, L"Î¢ÈíÑÅºÚ");
	HDC hDC = GetDC(HWND(winId()));
	QOpenGLContext* context = this->context();

	Main_Exit.hDC = hDC;
	Main_Maximize.hDC = hDC;
	Main_Minimize.hDC = hDC;
	Main_TitleBar.hDC = hDC;
	Main_Exit.context = context;
	Main_Maximize.context = context;
	Main_Minimize.context = context;
	Main_TitleBar.context = context;
	Main_Exit.UpdateText(L"¡Á");
	Main_Exit.YAlign = true;
	Main_Maximize.UpdateText(L"¡õ");
	Main_Maximize.YAlign = true;
	Main_Minimize.UpdateText(L"-");
	Main_Minimize.YAlign = true;
	Main_TitleBar.UpdateText(L"Title");
	Main_TitleBar.LeftAlign = true;
}
void MainOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);

}
void MainOpenGLWidget::paintGL()
{
	glClearColor(43.f/255.0, 43.f/255.0, 45.f/255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double scale = double(height()) / width();
	glOrtho(-1, 1, -scale, scale, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	Main_Exit.Draw();
	Main_Maximize.Draw();
	Main_Minimize.Draw();
	Main_TitleBar.Draw();

}