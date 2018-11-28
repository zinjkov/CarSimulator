#include <iostream>
//#include <opencv2/opencv.hpp>
#include <QApplication>
#include "mainwindow.hpp"


#include <QStyleFactory>
//#include <QPrinterInfo>

void makeDark(QApplication &a)
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);

    palette.setColor(QPalette::Highlight, QColor(61,142,201).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    a.setStyle(QStyleFactory::create("fusion"));
    a.setPalette(palette);
}


int main(int argc, char *argv[])
{
    std::cout << "started" << std::endl;
    QApplication app(argc, argv);
    makeDark(app);
    mainwindow w;
    w.show();
    return app.exec();
}
