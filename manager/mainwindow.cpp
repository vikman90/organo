#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectiondialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    translator = new QTranslator(this);

    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), SLOT(save()));
    connect(ui->actionEnglish, SIGNAL(triggered()), SLOT(language()));
    connect(ui->actionSpanish, SIGNAL(triggered()), SLOT(language()));
    connect(ui->actionItalian, SIGNAL(triggered()), SLOT(language()));
    connect(ui->actionOptions, SIGNAL(triggered()), SLOT(options()));
    connect(ui->btRewind, SIGNAL(clicked()), SLOT(rewind()));
    connect(ui->btPlay, SIGNAL(clicked()), SLOT(play()));
    connect(ui->btStop, SIGNAL(clicked()), SLOT(stop()));
    connect(ui->btForward, SIGNAL(clicked()), SLOT(forward()));

    switch (QLocale::system().language()) {
    case QLocale::Spanish:
        translator->load(":/languages/spanish");
        ui->actionSpanish->setChecked(true);
        break;
    case QLocale::Italian:
        break;
        translator->load(":/languages/italian");
        ui->actionItalian->setChecked(true);
    default:
        translator->load(":/languages/english");
        ui->actionEnglish->setChecked(true);
    }

    qApp->installTranslator(translator);
    ui->retranslateUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
    getOpenFileNames();
}

void MainWindow::save()
{
    getSaveFileName();
}

void MainWindow::options()
{
    (new OptionsDialog())->exec();
}

void MainWindow::language()
{
    QObject *sender = this->sender();

    if (sender == ui->actionEnglish) {
        translator->load(":/languages/english");
        ui->actionEnglish->setChecked(true);
        ui->actionSpanish->setChecked(false);
        ui->actionItalian->setChecked(false);
    } else if (sender == ui->actionSpanish) {
        translator->load(":/languages/spanish");
        ui->actionEnglish->setChecked(false);
        ui->actionSpanish->setChecked(true);
        ui->actionItalian->setChecked(false);
    } else if (sender == ui->actionItalian) {
        translator->load(":/languages/italian");
        ui->actionEnglish->setChecked(false);
        ui->actionSpanish->setChecked(false);
        ui->actionItalian->setChecked(true);
    }

    ui->retranslateUi(this);
}

void MainWindow::rewind()
{

}

void MainWindow::play()
{
    switch (playState) {
    case Paused:
        playState = Playing;
        ui->btPlay->setIcon(QIcon(":/icons/pause.png"));
        (new ConnectionDialog(this))->exec();
        break;
    case Playing:
        playState = Paused;
        ui->btPlay->setIcon(QIcon(":/icons/play.png"));
    default:
        ;
    }
}

void MainWindow::stop()
{

}

void MainWindow::forward()
{

}

QStringList MainWindow::getOpenFileNames()
{
    return QFileDialog::getOpenFileNames(this, QString(), QString(), tr("MIDI files (*.mid);;MIDI playlists (*.mpl)"));
}

QString MainWindow::getSaveFileName()
{
    return QFileDialog::getSaveFileName(this, QString(), QString(), tr("MIDI playlists (*.mpl)"));
}
