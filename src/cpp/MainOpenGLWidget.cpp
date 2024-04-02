#include "../header/MainOpenGLWidget.h"


extern GLButton Main_Exit;

MainOpenGLWidget::MainOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
}
void MainOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	Main_Exit.context = context();
}
void MainOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}
void MainOpenGLWidget::paintGL()
{
	glClearColor(43.f/255.0, 43.f/255.0, 45.f/255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	Main_Exit.Draw();
}