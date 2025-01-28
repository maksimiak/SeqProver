#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MyMainWindow window;
    window.setWindowTitle("Sequent Proof GUI");
    window.resize(600, 300);
    window.show();

    return app.exec();
}
