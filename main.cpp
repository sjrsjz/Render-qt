
#define _MAIN_
#include "src/header/Common.h"
#undef _MAIN_
#include "Render_qt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Render_qt w;
    w.show();
    return a.exec();
}
