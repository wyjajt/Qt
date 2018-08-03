#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

//解决中文显示乱码问题
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

//#include "gsoap/qqOnlineWebServiceSoap.nsmap"
#include "gsoap/soapqqOnlineWebServiceSoapProxy.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
