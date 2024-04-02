#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore/qdebug.h>
#include <QtCore/QByteArray>
#include <QtCore/qtimer.h>
#include <QtGui/qevent.h>
#include "ui_Render_qt.h"
#include "src/header/Common.h"
#include "src/header/GLButton.h"
#ifdef Q_OS_WIN
#include <windows.h>
#include <WinUser.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <objidl.h> // Fixes error C2504: 'IUnknown' : base class undefined
#include <gdiplus.h>
#include <GdiPlusColor.h>
#pragma comment (lib,"Dwmapi.lib") // Adds missing library, fixes error LNK2019: unresolved external symbol __imp__DwmExtendFrameIntoClientArea
#pragma comment (lib,"user32.lib")
#pragma comment (lib,"opengl32.lib")
#endif

extern GLButton Main_Exit;
extern int Cursor_X;
extern int Cursor_Y;

class Render_qt : public QMainWindow
{
    Q_OBJECT

public:
    Render_qt(QWidget *parent = nullptr);
    ~Render_qt();

    bool event(QEvent* e);
    void resizeEvent(QResizeEvent* e);
    void mouseReleaseEvent(QMouseEvent* event);
    void updateGLUI();
private:
    Ui::Render_qtClass ui;
};
