#include "mainwindow.h"

#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/icons/app_icon.jpg"));

    a.setStyle(QStyleFactory::create("Fusion"));

    QPalette p;
    p.setColor(QPalette::Window, QColor("#ffffff"));
    p.setColor(QPalette::WindowText, QColor("#222222"));
    p.setColor(QPalette::Base, QColor("#ffffff"));
    p.setColor(QPalette::AlternateBase, QColor("#f5f5f5"));
    p.setColor(QPalette::Text, QColor("#222222"));
    p.setColor(QPalette::Button, QColor("#ffffff"));
    p.setColor(QPalette::ButtonText, QColor("#222222"));
    p.setColor(QPalette::ToolTipBase, QColor("#ffffff"));
    p.setColor(QPalette::ToolTipText, QColor("#222222"));
    p.setColor(QPalette::PlaceholderText, QColor("#999999"));
    p.setColor(QPalette::Highlight, QColor("#ff7a1a"));
    p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    a.setPalette(p);

    MainWindow w;
    w.show();
    return a.exec();
}