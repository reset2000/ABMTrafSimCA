
#include "maintrafficsimulator.h"

#include <QApplication>
#include <QChart>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainTrafficSimulator w;
    w.show();
    return a.exec();
}
