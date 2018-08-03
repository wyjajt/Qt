#include "dialog.h"
#include "ui_dialog.h"
#include <string>
#include <QDebug>

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
    int     connect_reult;
    std::string str_number, *str_state;
    str_number = ui->lineEdit_qq_number->text().toStdString();

    qqOnlineWebServiceSoapProxy     service;    //使用代理类创建服务
    _ns1__qqCheckOnline             request;    //函数调用需要的参数（包含qq号码）
    _ns1__qqCheckOnlineResponse     response;   //函数调用需要的参数（包含在线状态）

    qDebug( "%d", 111111 );
    request.qqCode = &str_number;
    qDebug() << request.qqCode;

    connect_reult = service.qqCheckOnline( &request, response );
    str_state     = response.qqCheckOnlineResult;

    if( SOAP_OK != connect_reult )
    {
        ui->lineEdit_qq_state->setText("soap连接失败");
        return;
    }

    QString state = QString::fromStdString( *str_state );

    if(state == "Y")
    {
        ui->lineEdit_qq_state->setText("在线");
    }
    else if(state == "N")
    {
        ui->lineEdit_qq_state->setText("离线");
    }
    else if(state == "E")
    {
        ui->lineEdit_qq_state->setText("QQ号码错误");
    }
    else if(state == "A")
    {
        ui->lineEdit_qq_state->setText("商业用户验证失败");
    }
    else if(state == "V")
    {
        ui->lineEdit_qq_state->setText("免费用户超过数量");
    }
    else
    {
        ui->lineEdit_qq_state->setText("未知错误");
    }
}
