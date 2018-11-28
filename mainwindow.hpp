#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include "urho3dbase.hpp"

class mainwindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit mainwindow(QWidget *parent = nullptr);

    void keyReleaseEvent(QKeyEvent *ke);
    void keyPressEvent(QKeyEvent *ke);

    void changeEvent(QEvent *ev);
signals:

public slots:

private:
    Urho3DBase *centralWidget;

};

#endif // MAINWINDOW_HPP
