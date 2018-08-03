#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    // 矩形
//    QRectF rect(90.0, 90.0, 80.0, 90.0);
    QRectF rect( 100, 100, 200, 200 );  //(100,100)起始坐标, (200,200)矩形大小
    // 起始角度
    int startAngle = 0 * 16;
    // 跨越度数
    int spanAngle = 360 * 16;

    QPainter painter(this);

    // 反走样
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 设置画笔颜色、宽度
    painter.setPen(QPen(QColor(0, 160, 230), 2));

    // 绘制弧线
    painter.drawArc(rect, startAngle, spanAngle);
}
