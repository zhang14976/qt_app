#include "widget.h"

#include <QApplication>
#include <QProcess>
#include <QStringList>
#include <QRegularExpression>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
