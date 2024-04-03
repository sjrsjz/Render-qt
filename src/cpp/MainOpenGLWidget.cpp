#include "../header/MainOpenGLWidget.h"


extern GLButton Main_Exit;
extern GLButton Main_Minimize;
extern GLButton Main_Maximize;
extern GLButton Main_TitleBar;

MainOpenGLWidget::MainOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
	QSurfaceFormat surfaceFormat;
	surfaceFormat.setSamples(4);//多重采样
	setFormat(surfaceFormat); //setFormat是QOpenGLWidget的函数
}
void MainOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GLhFont = CreateFontW(1, 1, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 2, 54, L"微软雅黑");
	HDC hDC = GetDC(HWND(winId()));
	QOpenGLContext* context = this->context();
	glEnable(GL_MULTISAMPLE);
	Main_Exit.hDC = hDC;
	Main_Maximize.hDC = hDC;
	Main_Minimize.hDC = hDC;
	Main_TitleBar.hDC = hDC;
	Main_Exit.context = context;
	Main_Maximize.context = context;
	Main_Minimize.context = context;
	Main_TitleBar.context = context;
	Main_Exit.UpdateText(L"×");
	Main_Exit.YAlign = true;
	Main_Maximize.UpdateText(L"□");
	Main_Maximize.YAlign = true;
	Main_Minimize.UpdateText(L"-");
	Main_Minimize.YAlign = true;
	Main_TitleBar.UpdateText(L"Render(Qt version)");
	Main_TitleBar.LeftAlign = true;
}
void MainOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void MainOpenGLWidget::DrawBorder() {

	glLineWidth(2.0f);
	GLdouble WorldPos[12];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluUnProject(0, 0, 0, modelview, projection, viewport, &WorldPos[0], &WorldPos[1], &WorldPos[2]);
	gluUnProject(width(), 0, 0, modelview, projection, viewport, &WorldPos[3], &WorldPos[4], &WorldPos[5]);
	gluUnProject(width(), height(), 0, modelview, projection, viewport, &WorldPos[6], &WorldPos[7], &WorldPos[8]);
	gluUnProject(0, height(), 0, modelview, projection, viewport, &WorldPos[9], &WorldPos[10], &WorldPos[11]);

	glBegin(GL_LINES);
	glColor3f(75.0f/255, 75.0f/255, 100.0f/255);
	glVertex3d(WorldPos[0], WorldPos[1], WorldPos[2]);
	glVertex3d(WorldPos[3], WorldPos[4], WorldPos[5]);
	glVertex3d(WorldPos[3], WorldPos[4], WorldPos[5]);
	glVertex3d(WorldPos[6], WorldPos[7], WorldPos[8]);
	glVertex3d(WorldPos[6], WorldPos[7], WorldPos[8]);
	glVertex3d(WorldPos[9], WorldPos[10], WorldPos[11]);
	glVertex3d(WorldPos[9], WorldPos[10], WorldPos[11]);
	glVertex3d(WorldPos[0], WorldPos[1], WorldPos[2]);
	glEnd();

}
void MainOpenGLWidget::PaintUI() {
	Main_Exit.Draw();
	Main_Maximize.Draw();
	Main_Minimize.Draw();
	Main_TitleBar.Draw();
}

void MainOpenGLWidget::DrawScene() {
	//draw a rotating triangle
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glPushMatrix();
	glRotatef(GetTickCount()/10.0, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glEnd();
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}	

void MainOpenGLWidget::paintGL()
{
	glClearColor(43.f/255.0, 43.f/255.0, 45.f/255.0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double scale = double(height()) / width();
	glOrtho(-1, 1, -scale, scale, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawScene();
	PaintUI();
	DrawBorder();

}