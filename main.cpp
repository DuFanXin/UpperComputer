#include <QtGui/QApplication>
#include "uppercomputer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UpperComputer w;
    w.setWindowIcon(QIcon(":src/park.jpg"));
    w.showMaximized();

    return a.exec();
}
