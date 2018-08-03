#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    view = new QWebEngineView(this);
//    view->load(QUrl("http://www.baidu.com/"));
    view->load(QUrl("http://www.webxml.com.cn/webservices/qqOnlineWebService.asmx?wsdl"));
    view->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    view->resize(this->size());
}
