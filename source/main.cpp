#include <QApplication>
#include "CMainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CMainWindow window;
    window.show();
    return app.exec();
}
