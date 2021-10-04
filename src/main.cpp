#include <QApplication>
#include <QDebug>
#include "Viewport/Viewport.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Viewport viewport{};
    viewport.resize(800, 600);
    viewport.show();
    return QApplication::exec();
}
