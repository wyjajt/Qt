#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <vector>
#include <thread>
#include "capturethread.h"
#include "ui_mainwindow.h"
#include "plc_adi.h"

//解决中文显示乱码问题
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <windows.h>
#include "CameraApi.h"

#include "readdata.h"
#include "imgbuffer.h"

//显示画板的大小和图像大小
typedef struct _WIDTH_HEIGHT{
    int     display_width;
    int     display_height;
    int     xOffsetFOV;
    int     yOffsetFOV;
    int     sensor_width;
    int     sensor_height;
    int     buffer_size;
}Width_Height;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void    closeEvent(QCloseEvent *);
    int     status;     //相机连接状态, 0未连接, 1连接成功
    QWidget *getMainWidget();
    void    setPLC( PLC_Adi *argPLC);

signals:
    void    sendImageOut( QString str, QImage img );
    void    takePhotoAllOver();
public slots:
    void    rcv_takePhoto( int type );


protected:
    void    changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    int     init_SDK();
    int     GUI_init_parameter( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_init_Resolution( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_set_Resolution( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_init_exposure( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_init_WB( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_init_mmap( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_init_isp( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_init_speed( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_set_speed( int hCamera, tSdkCameraCapbility *pCameraInfo );
    int     GUI_init_Trigger( int hCamera, tSdkCameraCapbility *pCameraInfo );

private slots:
    void on_radioButton_trigger_hardware_clicked(bool checked);
    void on_radioButton_software_trigger_clicked(bool checked);
    void on_radioButton_collect_clicked(bool checked);
    void on_flashlight_l_clicked(bool checked);
    void on_flashlight_h_clicked(bool checked);
    void on_flashlight_manual_clicked(bool checked);
    void on_flashlight_auto_clicked(bool checked);
    void on_lineEdit_exposure_time_returnPressed();
    //图片保存类型
    void on_radioButton_jpg_clicked();
    void on_radioButton_png_clicked();
    void on_radioButton_raw_clicked();
    void on_radioButton_bmp_clicked();

    void on_pushButton_snap_catch_released();
    void on_pushButton_snap_path_released();
    void on_pushButton_para_load_released();
    void on_pushButton_para_save_released();
    void on_pushButton_para_acquiesce_released();
    void on_radioButton_D_clicked(bool checked);
    void on_radioButton_C_clicked(bool checked);
    void on_radioButton_B_clicked(bool checked);
    void on_horizontalSlider_isp_acutance_valueChanged(int value);
    void on_checkBox_isp_v_clicked(bool checked);
    void on_checkBox_isp_h_clicked(bool checked);
    void on_contrast_horizontalSlider_valueChanged(int value);
    void on_gamma_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_saturation_valueChanged(int value);
    void on_horizontalSlider_gain_b_valueChanged(int value);
    void on_horizontalSlider_gain_g_valueChanged(int value);
    void on_horizontalSlider_gain_r_valueChanged(int value);
    void on_horizontalSlider_exposure_time_valueChanged(int value);
    void on_horizontalSlider_gain_valueChanged(int value);
    void on_AWB_once_button_released();
    void on_res_combobox_activated(int index);

    void Image_process(QImage img);
    void camera_statues();

    void on_radioButton_A_clicked(bool checked);
    void on_exposure_mode_manual_clicked(bool checked);
    void on_exposure_mode_auto_clicked(bool checked);
    void on_radioButton_60HZ_clicked(bool checked);
    void on_radioButton_50HZ_clicked(bool checked);
    void on_software_trigger_once_button_clicked();
    void on_flicker_checkBox_clicked(bool checked);
    void on_AE_horizontalSlider_valueChanged(int value);

    void radioChange();

private:
//    QTimer  *m_timer;
    QLabel  *m_camera_statuesFps;
    bool    exposure_time_lineedit_status;
    int     pbySnapMode;    //触发模式
    QGraphicsScene      *m_scene;
    QGraphicsPixmapItem *m_image_item;
    QRadioButton *radioButton_speed[3];

    CaptureThread   *m_thread;

    QSize           image_size;
    XLogPtr         logger;

//    std::thread     thd_soft_trigger;       //获取算法结果线程
    std::thread     thd_detect;             //获取算法结果线程

    PLC_Adi                 *plc;
    ReadData                readData;
    QVector<QRgb>           grayColourTable;
    std::vector<ImgBuffer>  vec_buffer;

    bool    stop_take_photo;    //运动完成状态停止拍照

private:
    void initVector();
    void addBufferToVector();
    void saveImageInSoftMode( int type );   //软触发线程
    long ImageAnalysis( uchar* img,int bufferSize, BYTE threshold );    //计算光通量
    LONG __stdcall CA_ContourAreaSimple( int m_szheight, int m_szWidth, int m_szChannels,
        unsigned char* m_szpFrameBuffer, BYTE m_szThreshold);

};

#endif // MAINWINDOW_H
