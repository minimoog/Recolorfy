#include <QtGui/QApplication>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "colormanipulator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<ColorManipulator>("recolorfy.qmlcomponents", 1, 0, "ColorManipulator");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setAttribute(Qt::WA_NoSystemBackground); //Symbian^3
    viewer.setMainQmlFile(QLatin1String("qml/Recolorfy/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
