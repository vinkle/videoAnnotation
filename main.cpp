//==============================================================================
/*
    \author    Vinkle Srivastav
*/
//==============================================================================

#include "interface.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    interface w;
    w.showMaximized();
    return a.exec();
}
