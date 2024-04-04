#include "../header/MainOpenGLWidget.h"


extern GLButton Main_Exit;
extern GLButton Main_Minimize;
extern GLButton Main_Maximize;
extern GLButton Main_TitleBar;

void __stdcall GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	OutputDebugStringA(message);
	OutputDebugStringA("\n");
}

void MainOpenGLWidget::GaussianBlur( GLuint input_image, GLuint output_image ,GLRECT BlurRect, bool WE) {
	shader.ex.glUseProgram(GaussianBlurProgram);
	shader.ex.glUniform1i(shader.ex.glGetUniformLocation(GaussianBlurProgram, "width"), BlurRect.w);
	shader.ex.glUniform1i(shader.ex.glGetUniformLocation(GaussianBlurProgram, "height"), BlurRect.h);
	shader.ex.glUniform1i(shader.ex.glGetUniformLocation(GaussianBlurProgram, "top"), BlurRect.y);
	shader.ex.glUniform1i(shader.ex.glGetUniformLocation(GaussianBlurProgram, "left"), BlurRect.x);
	shader.ex.glUniform1i(shader.ex.glGetUniformLocation(GaussianBlurProgram, "WE"), WE);
	GLuint width = 0, height = 0;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, input_image);
	shader.ex.glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&width);
	shader.ex.glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&height);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input_image);
	shader.ex.glBindImageTexture(0, output_image, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	
	shader.ex.glDispatchCompute(width / 16, height / 16, 1);
	shader.ex.glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	shader.ex.glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
}

HWND WorkerW_Hwnd = NULL;
HDC WorkerW_Hdc = NULL;

HWND GetWorkerW() {
	//return FindWindowA("Progman", "Program Manager");
	HWND hWorkerW = nullptr;
	HWND hShellDefView = nullptr;
	HWND hDesktop = GetDesktopWindow();
	hWorkerW = FindWindowEx(hDesktop, nullptr, L"WorkerW", nullptr);
	hShellDefView = FindWindowEx(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
	return FindWindowEx(hShellDefView, nullptr, L"SysListView32", nullptr);

}

void MainOpenGLWidget::CaptureScreen(GLuint PBO, int width, int height, GLuint Tex) {
	glEnable(GL_TEXTURE_2D);

	BITMAPINFO binfo{};
	binfo.bmiHeader.biBitCount = 24;
	binfo.bmiHeader.biCompression = BI_RGB;
	binfo.bmiHeader.biHeight = height;
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biWidth = width;
	binfo.bmiHeader.biSizeImage = 0;

	HDC hDC = CreateCompatibleDC(0);
	HBITMAP hObj = CreateDIBSection(hDC, &binfo, 0, 0, 0, 0);
	SelectObject(hDC, hObj);
	
	BitBlt(hDC, 0, 0, width, height, WorkerW_Hdc, 0, 0, SRCCOPY);
	
	shader.gl->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
	
	void* p = shader.ex.glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	GetDIBits(hDC, hObj, 0, height, p, &binfo, DIB_RGB_COLORS);
	shader.ex.glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	DeleteObject(hObj);
	DeleteDC(hDC);

	shader.gl->glBindTexture(GL_TEXTURE_2D, Tex);
	shader.gl->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, 0);
	shader.gl->glBindTexture(GL_TEXTURE_2D, 0);
	shader.gl->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glDisable(GL_TEXTURE_2D);
}
MainOpenGLWidget::MainOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
	QSurfaceFormat surfaceFormat;
	surfaceFormat.setSamples(4);//多重采样
	setFormat(surfaceFormat); //setFormat是QOpenGLWidget的函数
}
void MainOpenGLWidget::initializeGL()
{
	WorkerW_Hwnd = GetWorkerW();
	WorkerW_Hdc = GetDC(WorkerW_Hwnd);

	initializeOpenGLFunctions();
	//enable debug output
	//glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)GLDebugMessageCallback, nullptr);
	//set clear color


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

	shader.gl = context->functions();
	shader.init();
	bool a=shader.ex.d_func()->initialized;
	QRect rect = QGuiApplication::primaryScreen()->geometry();
	context->functions()->glGenBuffers(1, &DesktopPBO_write);
	context->functions()->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, DesktopPBO_write);
	context->functions()->glBufferData(GL_PIXEL_UNPACK_BUFFER, rect.width() * rect.height() * 4, NULL, GL_STREAM_DRAW);
	context->functions()->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	DesktopImageTex = shader.Create2DImageTex32F(rect.width(), rect.height());
	DesktopImageTex_Swap = shader.Create2DImageTex32F(rect.width(), rect.height());
	GaussianBlurProgram = shader.CompileComputeShader(gaussian_blur_compute_shader, &GaussianBlurComputeShader);
}
void MainOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            

void MainOpenGLWidget::DrawBlurredBackground() {

	QRect rect = QGuiApplication::primaryScreen()->geometry();

	CaptureScreen(DesktopPBO_write, rect.width(), rect.height(), DesktopImageTex);
	GLRECT BlurRect;
	QPoint pos = mapToGlobal(QPoint(0, 0));
	BlurRect.x = pos.x();
	BlurRect.y = rect.height() - pos.y() - height();
	BlurRect.w = width();
	BlurRect.h = height();
	GaussianBlur(DesktopImageTex, DesktopImageTex_Swap, BlurRect, false);
	GaussianBlur(DesktopImageTex_Swap, DesktopImageTex, BlurRect, true);
	GaussianBlur(DesktopImageTex, DesktopImageTex_Swap, BlurRect, false);
	GaussianBlur(DesktopImageTex_Swap, DesktopImageTex, BlurRect, true);


	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, 0, -0.1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, DesktopImageTex);
	glBegin(GL_QUADS);

	QPoint widgetPos = mapToGlobal(QPoint(0, 0));

	GLRECT TexRect;
	TexRect.x = float(widgetPos.x()) / rect.width();
	TexRect.y = 1 - float(widgetPos.y()+height()) / rect.height();
	TexRect.w = float(width()) / rect.width();
	TexRect.h = float(height()) / rect.height();

	float scale = double(height()) / width();
	
	glColor4f(1, 1, 1, 0.75);
	glTexCoord2f(TexRect.x, TexRect.y);
	glVertex3f(-1, -scale, 0);
	glTexCoord2f(TexRect.x + TexRect.w, TexRect.y);
	glVertex3f(1, -scale, 0);
	glTexCoord2f(TexRect.x + TexRect.w, TexRect.y + TexRect.h);
	glVertex3f(1, scale, 0);
	glTexCoord2f(TexRect.x, TexRect.y + TexRect.h);
	glVertex3f(-1, scale, 0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	/*glBegin(GL_QUADS);
	glColor4f(43.0/255, 43.0/255, 45.0/255, 0.25);
	glVertex3f(-1, -scale, 0);
	glVertex3f(1, -scale, 0);
	glVertex3f(1, scale, 0);
	glVertex3f(-1, scale, 0);

	glEnd();*/
	glDisable(GL_BLEND);
	glPopMatrix();


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

	DrawBlurredBackground();
	DrawScene();
	PaintUI();
	DrawBorder();

}

MainOpenGLWidget::~MainOpenGLWidget()
{
	glDeleteBuffers(1, &DesktopPBO_write);
	glDeleteTextures(1, &DesktopImageTex);
	glDeleteTextures(1, &DesktopImageTex_Swap);
	glDeleteShader(GaussianBlurComputeShader);
	glDeleteProgram(GaussianBlurProgram);
}