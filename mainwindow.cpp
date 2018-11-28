#include "mainwindow.hpp"
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QToolBar>
#include <QDebug>
#include "keycontroller.hpp"
#include <QFileDialog>
#include <QTime>

mainwindow::mainwindow(QWidget *parent) : QMainWindow(parent), centralWidget(nullptr)
{
    centralWidget = new Urho3DBase("", new Urho3D::Context(), this);
    setCentralWidget(centralWidget);
    centralWidget->Setup();
    centralWidget->Start();
}

void mainwindow::keyReleaseEvent(QKeyEvent *ke)
{
    QWidget::keyReleaseEvent(ke);
    KeyController::instance().KeyReleased(ke);
}

void mainwindow::keyPressEvent(QKeyEvent *ke)
{
    QWidget::keyPressEvent(ke);
    KeyController::instance().KeyPressed(ke);
}

void mainwindow::changeEvent(QEvent *ev)
{
    QWidget::changeEvent(ev);
}
