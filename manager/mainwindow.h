#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "connectiondialog.h"
#include "optionsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum PlayState { Paused, Playing };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTranslator *translator;
    PlayState playState = Paused;

    QStringList getOpenFileNames();
    QString getSaveFileName();

private slots:
    void open();
    void save();
    void options();
    void language();
    void rewind();
    void play();
    void stop();
    void forward();
};

#endif // MAINWINDOW_H
