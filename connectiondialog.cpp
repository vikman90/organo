#include "connectiondialog.h"
#include "ui_connectiondialog.h"

ConnectionDialog::ConnectionDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}
