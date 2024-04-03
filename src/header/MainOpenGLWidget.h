#pragma once
#include "Common.h"
#include "GLButton.h"
#include <QtGui/QResizeEvent>
#include <QtGui/qpainter.h>

// 2.继承相关类
class MainOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Compatibility
{
    Q_OBJECT
public:
    explicit MainOpenGLWidget(QWidget* parent = nullptr);

private:
    void DrawBorder();
    void PaintUI();
    void DrawScene();
protected:
    // 3.重载相关虚函数
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

signals:

};
