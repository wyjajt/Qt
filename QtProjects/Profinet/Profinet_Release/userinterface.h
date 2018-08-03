#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include "math.h"

#include "plc_adi.h"
#include "mainwindow.h"
#include "defines.h"

namespace Ui {
class UserInterface;
}

class UserInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserInterface(QWidget *parent = 0);
    ~UserInterface();
    void    addTabWidget( QString name, QWidget* widget );

private:
    LONG __stdcall CA_ContourAreaSimple( int m_szheight, int m_szWidth, int m_szChannels,
                                                            unsigned char* m_szpFrameBuffer, BYTE m_szThreshold);
    void    initUI();
    void    initTableWidget();
    void    getNextPositionOfMaxValue();        //获取最大光通量后面一个点的坐标, 补偿系统误差
    void    dealImgResult( ReadData data );
    void    thd_getMaxResult();

signals:
    void    startProcess( int flag );           //流程开始, 判断动作信息
    void    stopProcess();                      //流程结束, 停止接收动作信息
    void    quitProcess();                      //流程结束, 退出监听线程

    void    takePhoto( int type );              //拍照信号
    void    sendSoftTakePhoto();                //相机软触发
    void    sendResultPosition( bool res, int x, int y );
    void    sendReDetectResult( bool result );
    void    sendJudgeResult( bool result );

    void    sendTestInfo( int action );

public slots:
    void    rcvImage( QString str, QImage img );
    void    rcvActionInfo( int action );
    void    rcvTakePhotoEnd();
    void    rcvDeviceInfoFromPLC( QString info );   //接收PLC发来的设备运行日志信息

private slots:
    void    on_pushButton_run_process_clicked();
    void    on_pushButton_stop_process_clicked();
    void    on_pushButton_quit_process_clicked();
    void    on_pushButton_test_clicked();
    void    on_pushButton_search_value_clicked();
    void    on_pushButton_search_max_value_clicked();

private:
    Ui::UserInterface   *ui;
    QGraphicsScene      *m_scene;
    QGraphicsPixmapItem *m_image_item;

    XLogPtr logger;
    int     table_row_number;
    int     table_col_number;

    int     result_pos_x_2;
    int     result_pos_y_2;
    int     result_pos_x_1;
    int     result_pos_y_1;
    int     detectResult;       //最大光通量
    int     result_index;       //最大光通量在表格中的行下标
    int     tableRowCount;

    int     action_state;

    bool    is_result_send;     //复检结果是否发送

    QString str_data;
    bool    take_photo_end;     //所有拍照结束
    int     take_photo_times;   //当前拍照次数

//    std::thread thd_dealResult;
    std::thread thd_maxResult;
};

#endif // USERINTERFACE_H
