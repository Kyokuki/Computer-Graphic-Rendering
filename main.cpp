#include "myglwidget.h"
#include<qobject.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyGLWidget w;
	w.setWindowTitle("Homework 2");
    w.show();
    return a.exec();
}
