#include "Render_qt.h"

static QTimer RenderTimer;

long WindowStyle = 0;

Render_qt::Render_qt(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint | Qt::Window | Qt::WindowMinimizeButtonHint)
{
    ui.setupUi(this);

    setWindowFlags(
        Qt::FramelessWindowHint | Qt::Window | Qt::WindowMinimizeButtonHint
    );
    
    setAttribute(Qt::WA_TranslucentBackground);

    ui.openGLWidget->setGeometry(1, 1, width() - 2, height() - 2);


    RenderTimer.setInterval(1000 / 60);
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
int DragStartX = 0;
int DragStartY = 0;
int DragStartW = 0;
int DragStartH = 0;

void Render_qt::paintEvent(QPaintEvent* e)
{

}


void Render_qt::updateGLUI() {
    ui.openGLWidget->update();
    Cursor_X = this->mapFromGlobal(QCursor().pos()).x();
    Cursor_Y = ui.centralWidget->height() - this->mapFromGlobal(QCursor().pos()).y();
    DWORD tick = GetTickCount();
    MouseInTitleBar = Cursor_Y > ui.centralWidget->height() - Main_Exit.rect.h && Cursor_Y <= ui.centralWidget->height() - BorderThickness && Cursor_X > BorderThickness && Cursor_X < ui.centralWidget->width() - BorderThickness;
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

    TitleBarMove.Update(tick);
    Main_Exit.rect.y = TitleBarMove.X();
    Main_Maximize.rect.y = TitleBarMove.X();
    Main_Minimize.rect.y = TitleBarMove.X();
    Main_TitleBar.rect.y = TitleBarMove.X();

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




void Render_qt::mousePressEvent(QMouseEvent* e) {
    if (MouseOnBorderE || MouseOnBorderW || MouseOnBorderN || MouseOnBorderS) {
		DragBorder = true;
		MouseDragX = QCursor::pos().x();
		MouseDragY = QCursor::pos().y();
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