#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    itoaProxy service;

    double a, b;
    a = ui->lineEdit_a->text().toDouble();
    b = ui->lineEdit_b->text().toDouble();

    int connect_result;
    double add_res;
    connect_result = service.add( a, b, add_res );

    qDebug( "%d  %f", connect_result, add_res );
    ui->lineEdit_addresult->setText( QString("%1").arg( add_res, 0, 'f', 2 ) );
}
