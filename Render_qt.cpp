#include "Render_qt.h"

static QTimer RenderTimer;

long WindowStyle = 0;

lstring workPath{};
lstring MLangError{};

RenderSystem renderSystem;

bool SettedUp = false;

Render_qt::Render_qt(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint | Qt::Window | Qt::WindowMinimizeButtonHint)
{
    //AllocConsole();
    LOCALE_WCOUT

    //_wfreopen(L"CONOUT$", L"w+t", stdout);

    ui.setupUi(this);

    setWindowFlags(
        Qt::FramelessWindowHint | Qt::Window | Qt::WindowMinimizeButtonHint
    );
    
    setAttribute(Qt::WA_TranslucentBackground);

    ui.openGLWidget->setGeometry(1, 1, width() - 2, height() - 2);

    QScreen* screen = QGuiApplication::primaryScreen();
    RenderTimer.setInterval(1000 / screen->refreshRate());
    RenderTimer.start();
    connect(&RenderTimer, &QTimer::timeout, [=]() {
        updateGLUI();
	});


    Main_Exit.background_color[0] = 50.0 / 255;
    Main_Exit.background_color[1] = 50.0 / 255;
    Main_Exit.background_color[2] = 60.0 / 255;
    Main_Exit.background_color[3] = 0.5;
    Main_Exit.text_color[0] = 1.0;
    Main_Exit.text_color[1] = 1.0;
    Main_Exit.text_color[2] = 1.0;
    Main_Exit.text_color[3] = 1.0;
    Main_Exit.size = 50.0f;
    Main_Maximize.background_color[0] = 50.0/255;
    Main_Maximize.background_color[1] = 50.0/255;
    Main_Maximize.background_color[2] = 60.0/255;
    Main_Maximize.background_color[3] = 0.5;
    Main_Maximize.text_color[0] = 1.0;
    Main_Maximize.text_color[1] = 1.0;
    Main_Maximize.text_color[2] = 1.0;
    Main_Maximize.text_color[3] = 1.0;
    Main_Maximize.size = 40.0f;
    Main_Minimize.background_color[0] = 50.0/255;
    Main_Minimize.background_color[1] = 50.0/255;
    Main_Minimize.background_color[2] = 60.0/255;
    Main_Minimize.background_color[3] = 0.5;
    Main_Minimize.text_color[0] = 1.0;
    Main_Minimize.text_color[1] = 1.0;
    Main_Minimize.text_color[2] = 1.0;
    Main_Minimize.text_color[3] = 1.0;
    Main_Minimize.size = 50.0f;


    Main_TitleBar.text_color[0] = 1.0;
    Main_TitleBar.text_color[1] = 1.0;
    Main_TitleBar.text_color[2] = 1.0;
    Main_TitleBar.text_color[3] = 1.0;
    Main_TitleBar.background_color[0] = 50.0/255;
    Main_TitleBar.background_color[1] = 50.0/255;
    Main_TitleBar.background_color[2] = 60.0/255;
    Main_TitleBar.background_color[3] = 0.5;
    Main_TitleBar.size = 35.0f;

    Main_Status.XOffset = 0;
    Main_Status.YOffset = 0;
    Main_Status.visible = true;
    Main_Status.text_color[0] = 1;
    Main_Status.text_color[1] = 1;
    Main_Status.text_color[2] = 1;
    Main_Status.text_color[3] = 1;
    Main_Status.background_color[0] = 0;
    Main_Status.background_color[1] = 0;
    Main_Status.background_color[2] = 0;
    Main_Status.background_color[3] = 0;
    Main_Status.size = 25.0f;

    renderSystem.Render_SmoothScale.SetTotalDuration(50);
    renderSystem.Render_SmoothScale.SetStartPosition(1.0, GetTickCount());

}
Render_qt::~Render_qt()
{
    RenderTimer.stop();
    DeleteObject(GLhFont);
}

bool MouseInTitleBar = false;
bool lMouseInTitleBar = false;
SmoothMove TitleBarMove;
const int BorderThickness = 10;
bool MouseOnBorderN = false;
bool MouseOnBorderW = false;
bool MouseOnBorderS = false;
bool MouseOnBorderE = false;
bool DragN = false;
bool DragW = false;
bool DragS = false;
bool DragE = false;
bool DragNW = false;
bool DragNE = false;
bool DragSW = false;
bool DragSE = false;
bool MouseDrag = false;
bool DragBorder = false;
int MouseDragX = 0;
int MouseDragY = 0;
int MouseDragX_L = 0;
int MouseDragY_L = 0;
int DragStartX = 0;
int DragStartY = 0;
int DragStartW = 0;
int DragStartH = 0;
extern int StartUp_Width;
extern int StartUp_Height;
void Render_qt::paintEvent(QPaintEvent* e)
{

}

void Render_qt::updateStatus() {
    std::wstring s;
    s += L"Time:" + std::to_wstring(renderSystem.Render_Time) + L"\n";
    s += L"FPS:" + std::to_wstring(1.0 / renderSystem.Render_dTime) + L"\n";
    s += L"Render Width:" + std::to_wstring(ui.openGLWidget->width()) + L"\n";
    s += L"Render Height:" + std::to_wstring(ui.openGLWidget->height()) + L"\n";
    s += L"Window Width:" + std::to_wstring(width()) + L"\n";
    s += L"Window Height:" + std::to_wstring(height()) + L"\n";
    s += L"Image Width:" + std::to_wstring(renderSystem.Render_Width) + L"\n";
    s += L"Image Height:" + std::to_wstring(renderSystem.Render_Height) + L"\n";
    s += L"SmoothScale:" + std::to_wstring(renderSystem.Render_SmoothScale.X()) + L"\n";
    s += L"MousePos:" + std::to_wstring(renderSystem.Render_MousePos[0]) + L"," + std::to_wstring(renderSystem.Render_MousePos[1]) + L"\n";
    s += L"MousePosInt:" + std::to_wstring(renderSystem.Render_MousePosInt[0]) + L"," + std::to_wstring(renderSystem.Render_MousePosInt[1]) + L"\n";
    s += L"Mouse:" + std::to_wstring(renderSystem.Render_Mouse[0]) + L"," + std::to_wstring(renderSystem.Render_Mouse[1]) + L"," + std::to_wstring(renderSystem.Render_Mouse[2]) + L"\n";
    s += L"KeyBoard:";
    size_t k = 0;
    for (auto& key : renderSystem.Render_KeyBoard) {
        if (key != 0) {
            s += std::to_wstring(k) + L" ";
        }
        k++;
    }
    s += L"\n";

    s += L"Matrix:\n";
    double Matrix[16];
    renderSystem.camera.getMatrix(Matrix);
    k = 0;
    for (int i = 0; i < 16; i++) {
		s += std::to_wstring(Matrix[i]) + L" ";
        k++;
        if (k%4 == 0) {
			s += L"\n";
			k = 0;
		}
	}
    s += L"\n";
    s += L"Y/P/R:" + std::to_wstring(renderSystem.camera.yaw) + L"," + std::to_wstring(renderSystem.camera.pitch) + L"," + std::to_wstring(renderSystem.camera.roll) + L"\n";
    s += L"X/Y 2D:" + std::to_wstring(renderSystem.camera.x_2D) + L"," + std::to_wstring(renderSystem.camera.y_2D) + L"\n";
    s += L"WorkPath:" + workPath + L"\n";
    Main_Status.UpdateText(s);
}

void Render_qt::updateVariables() {
    double dX = renderSystem.Render_KeyBoard[Qt::Key_W] - renderSystem.Render_KeyBoard[Qt::Key_S];
    double dZ = renderSystem.Render_KeyBoard[Qt::Key_A] - renderSystem.Render_KeyBoard[Qt::Key_D];
    renderSystem.camera.move(renderSystem.Render_SmoothScale.X() * renderSystem.Render_dTime * dX, 0, renderSystem.Render_SmoothScale.X() * renderSystem.Render_dTime * dZ);
    renderSystem.camera.x_2D -= renderSystem.Render_SmoothScale.X() * renderSystem.Render_dTime * dZ;
    renderSystem.camera.y_2D += renderSystem.Render_SmoothScale.X() * renderSystem.Render_dTime * dX;
    double dRoll = renderSystem.Render_KeyBoard[Qt::Key_Q] - renderSystem.Render_KeyBoard[Qt::Key_E];
    renderSystem.camera.moveRoll(renderSystem.Render_dTime * dRoll * renderSystem.Render_SmoothScale.X());
}
void Render_qt::updateGLUI() {
    updateStatus();

    if (!SettedUp) {
        resize(StartUp_Width, StartUp_Height);
        setMinimumHeight(16*9);
        setMinimumWidth(256);
        SettedUp = true;
    }
    ui.openGLWidget->update();
    Cursor_X = this->mapFromGlobal(QCursor().pos()).x();
    Cursor_Y = ui.centralWidget->height() - this->mapFromGlobal(QCursor().pos()).y();
    renderSystem.Render_MousePos[0] = double(Cursor_X) / ui.centralWidget->width();
    renderSystem.Render_MousePos[1] = double(Cursor_Y) / ui.centralWidget->height();
    renderSystem.Render_MousePosInt[0] = Cursor_X;
    renderSystem.Render_MousePosInt[1] = Cursor_Y;

    Qt::MouseButtons buttons = QApplication::mouseButtons();
    renderSystem.Render_Mouse[0] = buttons.testFlag(Qt::LeftButton);
    renderSystem.Render_Mouse[1] = buttons.testFlag(Qt::RightButton);
    renderSystem.Render_Mouse[2] = buttons.testFlag(Qt::MiddleButton);
   
    DWORD tick = GetTickCount();
    MouseInTitleBar = this->underMouse() && (Cursor_Y > ui.centralWidget->height() - Main_Exit.rect.h && Cursor_Y <= ui.centralWidget->height() - BorderThickness && Cursor_X > BorderThickness && Cursor_X < ui.centralWidget->width() - BorderThickness);
    if (MouseInTitleBar && !lMouseInTitleBar) {
        TitleBarMove.NewEndPositon(ui.openGLWidget->height() - Main_Exit.rect.h, tick);
    }
    else if (!MouseInTitleBar && lMouseInTitleBar) {
        TitleBarMove.NewEndPositon(ui.openGLWidget->height(), tick);
    }

    if (MouseInTitleBar) {
        TitleBarMove.SetTotalDuration(100);
    }
    else {
        TitleBarMove.SetTotalDuration(500);
    }
    renderSystem.Render_SmoothScale.Update(GetTickCount());
    updateVariables();

    TitleBarMove.Update(tick);
    Main_Exit.rect.y = TitleBarMove.X();
    Main_Maximize.rect.y = TitleBarMove.X();
    Main_Minimize.rect.y = TitleBarMove.X();
    Main_TitleBar.rect.y = TitleBarMove.X();
    Main_Status.rect.w = ui.openGLWidget->width();
    Main_Status.rect.h = ui.openGLWidget->height();

    lMouseInTitleBar = MouseInTitleBar;

    bool MouseOnMainExit = Main_Exit.TestClick(Cursor_X, Cursor_Y);
    Main_Exit.background_color[0] = MouseOnMainExit ? 1.0 : 50.0 / 255;
    Main_Exit.background_color[1] = MouseOnMainExit ? 0.0 : 50.0 / 255;
    Main_Exit.background_color[2] = MouseOnMainExit ? 0.0 : 60.0 / 255;
    Main_Exit.background_color[3] = MouseOnMainExit ? 0.75 : 0.5;


    Main_Maximize.background_color[3] = Main_Maximize.TestClick(Cursor_X, Cursor_Y) ? 1.0 : 0.5;
    Main_Minimize.background_color[3] = Main_Minimize.TestClick(Cursor_X, Cursor_Y) ? 1.0 : 0.5;


    MouseOnBorderW = Cursor_X<BorderThickness && Cursor_X>-BorderThickness;
    MouseOnBorderE = Cursor_X>ui.openGLWidget->width() - BorderThickness && Cursor_X<ui.openGLWidget->width() + BorderThickness;
    MouseOnBorderS = Cursor_Y<BorderThickness && Cursor_Y>-BorderThickness;
    MouseOnBorderN = Cursor_Y>ui.openGLWidget->height() - BorderThickness && Cursor_Y<ui.openGLWidget->height() + BorderThickness;
    
    if (MouseOnBorderN && MouseOnBorderW || MouseOnBorderS && MouseOnBorderE) {
        SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
    }
    else if (MouseOnBorderN && MouseOnBorderE || MouseOnBorderS && MouseOnBorderW) {
        SetCursor(LoadCursor(NULL, IDC_SIZENESW));
    }
    else if (MouseOnBorderW || MouseOnBorderE) {
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
	}
    else if (MouseOnBorderN || MouseOnBorderS) {
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
	}
    else {
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}

bool Render_qt::event(QEvent* message) {
    return QWidget::event(message);
}

bool Render_qt::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    return QWidget::nativeEvent(eventType, message, result);
}


void Render_qt::keyPressEvent(QKeyEvent* e) {
    if (e->key() >= sizeof(renderSystem.Render_KeyBoard) / sizeof(renderSystem.Render_KeyBoard[0])) return;
	renderSystem.Render_KeyBoard[e->key()] = true;
}

void Render_qt::keyReleaseEvent(QKeyEvent* e) {
    if (e->key() >= sizeof(renderSystem.Render_KeyBoard) / sizeof(renderSystem.Render_KeyBoard[0])) return;
	renderSystem.Render_KeyBoard[e->key()] = false;
}

void Render_qt::mousePressEvent(QMouseEvent* e) {
    MouseDragX_L = QCursor::pos().x();
    MouseDragY_L = QCursor::pos().y();
    if (MouseOnBorderE || MouseOnBorderW || MouseOnBorderN || MouseOnBorderS) {
		DragBorder = true;
        MouseDragX = MouseDragX_L;
        MouseDragY = MouseDragY_L;
        DragStartX = this->x();
        DragStartY = this->y();
        DragStartW = this->width();
        DragStartH = this->height();

        DragN = DragW = DragS = DragE = DragNW = DragNE = DragSW = DragSE = false;

        if (MouseOnBorderN && MouseOnBorderW) {
			DragNW = true;
		}
        else if (MouseOnBorderN && MouseOnBorderE) {
			DragNE = true;
		}
        else if (MouseOnBorderS && MouseOnBorderW) {
			DragSW = true;
		}
        else if (MouseOnBorderS && MouseOnBorderE) {
			DragSE = true;
		}
        else if (MouseOnBorderW) {
			DragW = true;
		}
        else if (MouseOnBorderE) {
			DragE = true;
		}
        else if (MouseOnBorderN) {
			DragN = true;
		}
        else if (MouseOnBorderS) {
			DragS = true;
		}
	}
    else if (Main_TitleBar.TestClick(Cursor_X, Cursor_Y)) {
        MouseDrag = true;
        MouseDragX=QCursor::pos().x();
        MouseDragY=QCursor::pos().y();
        DragStartX = this->x();
        DragStartY = this->y();
    }
}

void Render_qt::wheelEvent(QWheelEvent* e) {
    if(e->delta()>0)
		renderSystem.Render_SmoothScale.NewEndPositon(renderSystem.Render_SmoothScale.X() / 1.5, GetTickCount());
	else
        renderSystem.Render_SmoothScale.NewEndPositon(renderSystem.Render_SmoothScale.X() * 1.5, GetTickCount());
}

void Render_qt::mouseMoveEvent(QMouseEvent* e) {
    if (MouseDrag) {
        this->move(QCursor::pos().x() - MouseDragX + DragStartX, QCursor::pos().y() - MouseDragY + DragStartY);
        QWidget::mouseMoveEvent(e);
    }
    else if (DragBorder) {
        
        if (DragN) {
            this->move(DragStartX, QCursor::pos().y() - MouseDragY + DragStartY);
            this->resize(DragStartW, DragStartH - QCursor::pos().y() + MouseDragY);
        }
        else if (DragW) {
			this->move(QCursor::pos().x() - MouseDragX + DragStartX, DragStartY);
			this->resize(DragStartW - QCursor::pos().x() + MouseDragX, DragStartH);
		}
        else if (DragS) {
			this->resize(DragStartW, DragStartH + QCursor::pos().y() - MouseDragY);
		}
        else if (DragE) {
            this->resize(DragStartW + QCursor::pos().x() - MouseDragX, DragStartH);
        }
        else if (DragNW) {
			this->move(QCursor::pos().x() - MouseDragX + DragStartX, QCursor::pos().y() - MouseDragY + DragStartY);
			this->resize(DragStartW - QCursor::pos().x() + MouseDragX, DragStartH - QCursor::pos().y() + MouseDragY);
		}
        else if (DragNE) {
			this->move(DragStartX, QCursor::pos().y() - MouseDragY + DragStartY);
			this->resize(DragStartW + QCursor::pos().x() - MouseDragX, DragStartH - QCursor::pos().y() + MouseDragY);
		}
        else if (DragSW) {
			this->move(QCursor::pos().x() - MouseDragX + DragStartX, DragStartY);
			this->resize(DragStartW - QCursor::pos().x() + MouseDragX, DragStartH + QCursor::pos().y() - MouseDragY);
		}
        else if (DragSE) {
			this->resize(DragStartW + QCursor::pos().x() - MouseDragX, DragStartH + QCursor::pos().y() - MouseDragY);
		}

	}
	else {
        QScreen* screen = QGuiApplication::primaryScreen();
        double dX = double(QCursor::pos().x() - MouseDragX_L) / screen->geometry().height();
        double dY = double(QCursor::pos().y() - MouseDragY_L) / screen->geometry().height();

        double dX_2D = double(QCursor::pos().x() - MouseDragX_L) / max(this->geometry().width(), this->geometry().height());
        double dY_2D = double(QCursor::pos().y() - MouseDragY_L) / max(this->geometry().width(), this->geometry().height());

        double scale = renderSystem.Render_SmoothScale.X();
        if (e->buttons() & Qt::RightButton) {
			renderSystem.camera.move(0, dY * scale, dX * scale);
		}
        else if (e->buttons() & Qt::MiddleButton) {
			renderSystem.camera.move(dY * scale, 0, dX * scale);
		}
        else if (e->buttons() & Qt::LeftButton) {
            renderSystem.camera.moveYaw(dX * 3.14159265358 * 4);
            renderSystem.camera.movePitch(dY * 3.14159265358 * 4);

            renderSystem.camera.x_2D -= dX_2D * scale;
            renderSystem.camera.y_2D += dY_2D * scale;

		}
        else {

		}        
        MouseDragX_L = QCursor::pos().x();
        MouseDragY_L = QCursor::pos().y();
		QWidget::mouseMoveEvent(e);
	
    }
    
}

void Render_qt::mouseDoubleClickEvent(QMouseEvent* e) {
    if (Main_TitleBar.TestClick(Cursor_X, Cursor_Y)) {
        if (this->isMaximized()) {
			this->showNormal();
		}
        else {
			this->showMaximized();
		}
		QResizeEvent event(QSize(ui.centralWidget->width(), ui.centralWidget->height()), QSize(ui.centralWidget->width(), ui.centralWidget->height()));
		resizeEvent(&event);
	}
}

void Render_qt::mouseReleaseEvent(QMouseEvent* e) {
    MouseDrag = false;
    DragBorder = false;
    if (e->button() == Qt::LeftButton) {
        auto x = e->x();
        auto y = ui.centralWidget->height() - e->y();
        
        if (Main_Exit.TestClick(x, y)) {
            this->close();
        }
        else if (Main_Maximize.TestClick(x, y)) {
            if (this->isMaximized()) {
                this->showNormal();
            }
            else {
				this->showMaximized();
            }
            QResizeEvent event(QSize(ui.centralWidget->width(), ui.centralWidget->height()), QSize(ui.centralWidget->width(), ui.centralWidget->height()));
            resizeEvent(&event);
        }
        else if (Main_Minimize.TestClick(x, y)) {
            this->showMinimized();

		}
    }
}

void Render_qt::resizeEvent(QResizeEvent* e) {
    ui.openGLWidget->setGeometry(1, 1, width() - 2, height() - 2);
    Main_Exit.Resize(80, 50);
    Main_Maximize.Resize(80, 50);
    Main_Minimize.Resize(80, 50);
    Main_TitleBar.Resize(ui.openGLWidget->width() - Main_Exit.rect.w - Main_Maximize.rect.w - Main_Minimize.rect.w, 50);

    
    if (MouseInTitleBar) {
        Main_Exit.MoveTo(ui.openGLWidget->width() - Main_Exit.rect.w, ui.openGLWidget->height() - Main_Exit.rect.h);
        Main_Maximize.MoveTo(ui.openGLWidget->width() - Main_Exit.rect.w - Main_Maximize.rect.w, ui.openGLWidget->height() - Main_Exit.rect.h);
        Main_Minimize.MoveTo(ui.openGLWidget->width() - Main_Exit.rect.w - Main_Maximize.rect.w - Main_Minimize.rect.w, ui.openGLWidget->height() - Main_Exit.rect.h);
        Main_TitleBar.MoveTo(0, ui.openGLWidget->height() - Main_Exit.rect.h);
        TitleBarMove.SetStartPosition(ui.openGLWidget->height() - Main_Exit.rect.h, GetTickCount());
    }
    else {
		Main_Exit.MoveTo(ui.openGLWidget->width() - Main_Exit.rect.w, ui.openGLWidget->height());
        Main_Maximize.MoveTo(ui.openGLWidget->width() - Main_Exit.rect.w - Main_Maximize.rect.w, ui.openGLWidget->height());
        Main_Minimize.MoveTo(ui.openGLWidget->width() - Main_Exit.rect.w - Main_Maximize.rect.w - Main_Minimize.rect.w, ui.openGLWidget->height());
        Main_TitleBar.MoveTo(0, ui.openGLWidget->height());
        TitleBarMove.SetStartPosition(ui.openGLWidget->height(), GetTickCount());
    }
}