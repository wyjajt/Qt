#include "userinterface.h"
#include <QApplication>

#define PLC
#define Camera

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UserInterface ui;


#ifdef PLC
    PLC_Adi     plc;
    if( plc.initDevice() == -2 )
    {
        QMessageBox::warning( NULL, "操作错误", "PLC连接超时" );
        return -1;
    }
#endif

#ifdef Camera
    MainWindow  debugUI;
    if( debugUI.status == CAMERA_NO_CONNECT )
    {
        QMessageBox::warning( NULL, "操作错误", "相机未连接" );
        return -1;
    }

#ifdef PLC
    debugUI.setPLC( &plc );
#endif

#endif

#ifdef PLC
    /*  PLC <---> UI    */
    QObject::connect( &ui, SIGNAL( startProcess(int) ),
                &plc, SLOT( rcv_startProcess(int) ) );
    QObject::connect( &ui, SIGNAL( stopProcess() ),
                &plc, SLOT( rcv_stopProcess() ) );
    QObject::connect( &ui, SIGNAL( quitProcess() ),
                &plc, SLOT( rcv_quitProcess() ) );

    QObject::connect( &plc, SIGNAL( sendActionInfo(int) ),
                &ui, SLOT( rcvActionInfo(int) ) );

    //测试流程
    QObject::connect( &ui, SIGNAL( sendTestInfo(int) ),
                      &plc, SIGNAL( sendActionInfo(int) ) );

    QObject::connect( &ui, SIGNAL( sendResultPosition(int,int) ),
                      &plc, SLOT( rcvResultPosition(int,int) ) );

    QObject::connect( &ui, SIGNAL( sendReDetectResult(bool)),
                      &plc, SLOT( rcvReDetectResult(bool) ) );
#endif


#ifdef Camera
    /*  UI <---> Camera    */
    QWidget *window = debugUI.getMainWidget();
    ui.addTabWidget( "调试界面", window );

    QObject::connect( &ui, SIGNAL( takePhoto(int)),
                &debugUI, SLOT( rcv_takePhoto(int)) );

    QObject::connect( &debugUI, SIGNAL( sendImageOut(QString, QImage)),
                &ui, SLOT( rcvImage(QString, QImage)) );

    QObject::connect( &debugUI, SIGNAL( takePhotoAllOver() ),
                &ui, SLOT( rcvTakePhotoEnd() ) );

#endif



    ui.show();

    return a.exec();
}
