#include "../header/MainOpenGLWidget.h"


extern GLButton Main_Exit;
extern GLButton Main_Minimize;
extern GLButton Main_Maximize;
extern GLButton Main_TitleBar;
extern GLButton Main_Status;

extern RenderSystem renderSystem;

extern bool DisableBackgroundBlur;
extern int StartUp_Width;
extern int StartUp_Height;

static double lastTime = 0;
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
	void* pBits;
	HBITMAP hObj = CreateDIBSection(hDC, &binfo, 0, &pBits, 0, 0);
	SelectObject(hDC, hObj);

	BitBlt(hDC, 0, 0, width, height, WorkerW_Hdc, 0, 0, SRCCOPY);

	shader.gl->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);

	void* p = shader.ex.glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	memcpy(p, pBits, height * width * 3);
	//GetDIBits(hDC, hObj, 0, height, p, &binfo, DIB_RGB_COLORS);
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
void infoCallback(const wchar_t* e) {
	DebugOutput(e);
	std::wcout << e << std::endl;
}
void MainOpenGLWidget::initializeGL()
{
	WorkerW_Hwnd = GetWorkerW();
	WorkerW_Hdc = GetDC(WorkerW_Hwnd);

	initializeOpenGLFunctions();
	//enable debug output
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)GLDebugMessageCallback, nullptr);
	//set clear color
	DisableBackgroundBlur = false;

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

	Main_Status.hDC = hDC;
	Main_Status.context = context;
	Main_Status.UpdateText(L"Status: Ready");
	Main_Status.LeftAlign = true;
	Main_Status.TopAlign = true;
	Main_Status.XOffset = 0.25;
	Main_Status.visible = false;

	shader.gl = context->functions();
	shader.init();
	QRect rect = QGuiApplication::primaryScreen()->geometry();
	context->functions()->glGenBuffers(1, &DesktopPBO_write);
	context->functions()->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, DesktopPBO_write);
	context->functions()->glBufferData(GL_PIXEL_UNPACK_BUFFER, rect.width() * rect.height() * 4, NULL, GL_STREAM_DRAW);
	context->functions()->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	DesktopImageTex = shader.Create2DImageTex32F(rect.width(), rect.height());
	DesktopImageTex_Swap = shader.Create2DImageTex32F(rect.width(), rect.height());
	GaussianBlurProgram = shader.CompileComputeShader(gaussian_blur_compute_shader, &GaussianBlurComputeShader);

	renderSystem.gl.initializeOpenGLFunctions();
	renderSystem.shader.gl = context->functions();
	renderSystem.shader.init();

	renderSystem.info_callback = infoCallback;
	
	QStringList args = QCoreApplication::arguments();
	std::vector<std::wstring> argv;
	for (auto& x : args) {
		argv.push_back(x.toStdWString());
	}
	bool a = false;
	try {
		for (size_t i{}; i < argv.size(); i++) {
			auto& x = argv[i];
			if (x == L"--debug") {
				renderSystem.info_callback = infoCallback;
				Main_Status.visible = true;
			}
			if (x == L"--fullscreen") {
				renderSystem.Render_Width = rect.width();
				renderSystem.Render_Height = rect.height();
			}
			if (x == L"--width") {
				i++;
				renderSystem.Render_Width = std::stoi(argv[i]);
			}
			if (x == L"--height") {
				i++;
				renderSystem.Render_Height = std::stoi(argv[i]);
			}
			if (x == L"--window_width") {
				i++;
				StartUp_Width = std::stoi(argv[i]);
			}
			if (x == L"--window_height") {
				i++;
				StartUp_Height = std::stoi(argv[i]);
			}
			if (x == L"--file") {
				i++;
				renderSystem.build(argv[i]);
				a = true;
			}
			if (x == L"--no_background_blur") {
				DisableBackgroundBlur = true;
			}
		}
	}
	catch (...) {
		renderSystem.info(L"Error in command line arguments");
		return;
	}
	if (!a) {
		renderSystem.info(L"Please specify a project file");
		return;
	}
	renderSystem.Render_CmdLineArgs = argv;
	lastTime = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
	renderSystem.create();
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
	Main_Status.Draw();
	Main_Exit.Draw();
	Main_Maximize.Draw();
	Main_Minimize.Draw();
	Main_TitleBar.Draw();
}

void MainOpenGLWidget::DrawScene() {
	if (renderSystem.Error) return;
	double ltime = lastTime;
	for (auto& x : renderSystem.updatedVars) {
		renderSystem.updateVar(x.c_str());
	}
	renderSystem.updatedVars.clear();
	renderSystem.enterUpdate();
	double currentTime = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
	renderSystem.update(currentTime-ltime);
	lastTime=currentTime;
	DrawFullWindowTexture(renderSystem.leaveUpdate(), false);

}	

void MainOpenGLWidget::DrawFullWindowTexture(GLuint Tex, bool resize) {
	//scale tex and draw it to full window
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, 0, -0.1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//draw texture to full window
	//确保纹理不被拉伸


	glBindTexture(GL_TEXTURE_2D, Tex);
	GLint tex_W{}, tex_H{};
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_W);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_H);
	glBegin(GL_QUADS);

	float scale = double(tex_H) / tex_W;
	float scale2 = double(height()) / width();
	if (scale2 < scale) {
		scale2 = 1;
	}
	else {
		scale2 = scale2 / scale;
	
	}
	glColor4f(1, 1, 1, 1);
	glTexCoord2f(0, 0);
	glVertex3f(-scale2, -scale * scale2, 0);
	glTexCoord2f(1, 0);
	glVertex3f(scale2, -scale * scale2, 0);
	glTexCoord2f(1, 1);
	glVertex3f(scale2, scale * scale2, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-scale2, scale * scale2, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
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
	glLoadIdentity();

	if(!DisableBackgroundBlur)
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

void MainOpenGLWidget::closeEvent(QCloseEvent* event) {
	renderSystem.release();
}