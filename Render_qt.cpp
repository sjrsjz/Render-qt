#include "Render_qt.h"

static QTimer RenderTimer;


Render_qt::Render_qt(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Window | \
        Qt::FramelessWindowHint | \
        Qt::WindowSystemMenuHint | \
        Qt::WindowMinMaxButtonsHint \
    );

    SetWindowLongPtr(HWND(winId()), GWL_STYLE, WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
    //把client区域扩展到标题栏上和边框上，只留1个像素的余量
    const MARGINS shadow = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(HWND(winId()), &shadow);
    
    GLhFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");


    ui.openGLWidget->setGeometry(1, 1, width() - 2, height() - 2);


    RenderTimer.setInterval(1000 / 60);
    RenderTimer.start();
    connect(&RenderTimer, &QTimer::timeout, [=]() {
        updateGLUI();
	});

    Main_Exit.Resize(100, 50);
    Main_Exit.background_color[0] = 1.0;
    Main_Exit.background_color[1] = 0.0;
    Main_Exit.background_color[2] = 0.0;
    Main_Exit.background_color[3] = 0.5;
}
Render_qt::~Render_qt()
{
    RenderTimer.stop();
    DeleteObject(GLhFont);
}


void Render_qt::updateGLUI() {
    ui.openGLWidget->update();
    Cursor_X = this->mapFromGlobal(QCursor().pos()).x();
    Cursor_Y = ui.centralWidget->height() - this->mapFromGlobal(QCursor().pos()).y();

    Main_Exit.background_color[3] = Main_Exit.TestClick(Cursor_X, Cursor_Y) ? 1.0 : 0.5;

}

bool Render_qt::event(QEvent* message) {
    return QWidget::event(message);
}
void Render_qt::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        auto x = e->x();
        auto y = ui.centralWidget->height() - e->y();
        
        if (Main_Exit.TestClick(x, y)) {
            MessageBoxW(0, L"Click", L"Click", 0);
            this->close();
        }
    }
}

void Render_qt::resizeEvent(QResizeEvent* e) {
    ui.openGLWidget->setGeometry(1, 1, width() - 2, height() - 2);
    Main_Exit.MoveTo(ui.openGLWidget->width()-Main_Exit.rect.w, ui.openGLWidget->height() - Main_Exit.rect.h);
}