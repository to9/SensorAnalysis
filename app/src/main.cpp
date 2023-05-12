#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QScreen>
#include <QtGui>
#include <QFile>
#include <QDir>
#include "config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("VP");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("SensorAnalysis");
    QCoreApplication::setApplicationVersion(SOFT_VERSION_GIT);

    MainWindow* w = nullptr;

    QPixmap main_pixmap;
    QDir path(QCoreApplication::applicationDirPath());
    QFile file = path.filePath("sa_splashscreen.png");
    if (file.exists())
    {
      main_pixmap = QPixmap(file.fileName());
    }

    QSplashScreen splash(main_pixmap, Qt::WindowStaysOnTopHint);
    QScreen * screen = QGuiApplication::primaryScreen();
    const QPoint center = screen->availableVirtualGeometry().center();
    splash.move(center - splash.rect().center());
    splash.show();
    a.processEvents();

    auto deadline = QDateTime::currentDateTime().addMSecs(500);
    while (QDateTime::currentDateTime() < deadline)
    {
      a.processEvents();
    }

    w = new MainWindow();

    deadline = QDateTime::currentDateTime().addMSecs(1000);
    while (QDateTime::currentDateTime() < deadline && !splash.isHidden())
    {
      a.processEvents();
    }

#if 0
    QPixmap pix(":/resource/sa_splashscreen.gif");
    QSplashScreen splash(pix);
    QLabel splashlabel(&splash);
    QMovie splashgif(":/resource/splash2.gif");
    splashlabel.setMovie(&splashgif);
    splashgif.start();
    splash.show();
    splash.setCursor(Qt::BlankCursor);
    for(int i=0;i<15000;i+=splashgif.speed())
    {
        QCoreApplication::processEvents();
        Sleep(splashgif.speed()/5);
    }
#endif

    w->show();
    splash.finish(w);
    return a.exec();
}
