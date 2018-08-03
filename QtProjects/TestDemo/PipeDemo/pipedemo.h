#ifndef PIPEDEMO_H
#define PIPEDEMO_H

#include <QWidget>
#include <QObject>

#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>


namespace Ui {
class PipeDemo;
}

class PipeDemo : public QWidget
{
    Q_OBJECT

public:
    explicit PipeDemo(QWidget *parent = 0);
    ~PipeDemo();

signals:
    void sendInfo( QString str );

private slots:
    void on_pushButton_clicked();
    void rcvInfo( QString str );

private:
    void execmd( const char* cmd, char* result );


private:
    Ui::PipeDemo *ui;
    std::thread thd;

};

#endif // PIPEDEMO_H
