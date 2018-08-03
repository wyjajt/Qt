#include "userinterface.h"
#include "ui_userinterface.h"
#include <QDebug>

static auto delay_ui = AccurateDelay_ms();

UserInterface::UserInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UserInterface)
{
    ui->setupUi(this);
    m_image_item = NULL;

    action_state   = Action_Null;

    is_result_send = true;
    take_photo_end = false;

    detectResult   = 0;
    result_index   = 0;
    tableRowCount  = 0;

    result_pos_x_1   = 0;
    result_pos_y_1   = 0;
    result_pos_x_2   = 0;
    result_pos_y_2   = 0;
    take_photo_times = 0;

    auto fun = InitModuleLog();
    fun( logger, LOG_NAME_ROOT, PatternType::PatternDate, "" );

    initUI();
}

UserInterface::~UserInterface()
{
    emit quitProcess(); //PLC退出监听线程
    Delay_ms( 100 );    //等待线程退出

    delete ui;

    Log_Debug( logger, "~UserInterface succcess ..." );

    auto fun = ReleaseModuleLog();
    fun( logger );
}

void UserInterface::initUI()
{  
    ui->pushButton_stop_process->setEnabled(false);
    ui->pushButton_quit_process->setEnabled(false);

    //TabWidget
    ui->tabWidget->setTabText( 0, "流程界面" );

    m_scene = new QGraphicsScene(this);
//    m_scene->setPos(0, 0);
    ui->gvMain->setScene(m_scene);

    //TableWidget
    initTableWidget();
    Log_Debug( logger, "init UI succcess ..." );
}

void UserInterface::initTableWidget()
{
    ui->lineEdit_position_1->setText( "NULL" );
    ui->lineEdit_position_2->setText( "NULL" );
    ui->lineEdit_max_value->setText( "NULL" );
    ui->lineEdit_maxValue->setText( "NULL" );
    ui->lineEdit_redet_value->setText( "NULL" );
    table_row_number = TABLE_ROW_COUNT;
    table_col_number = TABLE_COL_COUNT;

    ui->tableWidget_vector->clear();
    QStringList headerLabel;
    headerLabel << "x坐标(触发前)" << "y坐标(触发前)" << "x坐标(触发后)" << "y坐标(触发后)" << "坐标差值x" << "坐标差值y" << "光通量大小";
    ui->tableWidget_vector->setRowCount( table_row_number );
    ui->tableWidget_vector->setColumnCount( table_col_number );

    ui->tableWidget_vector->setHorizontalHeaderLabels( headerLabel );
    Log_Debug( logger, "init TableWidget succcess ..." );
}

void UserInterface::rcvImage( QString str, QImage img )
{
    take_photo_times++;

    ReadData data;
    data.fromQString(str);

    if( m_image_item )
    {
        m_scene->removeItem( m_image_item );
        delete m_image_item;
        m_image_item = 0;
    }

    m_image_item = m_scene->addPixmap(QPixmap::fromImage(img));

    if( action_state == Action_Light_Judge )
    {
        emit sendJudgeResult( data.detect_result > 0 );
        return;
    }

    table_row_number++;
    ui->tableWidget_vector->setRowCount( table_row_number );
    for( int i = 0; i < table_col_number - 1; i++ )
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText( "" );
        ui->tableWidget_vector->setItem( table_row_number - 1, i, item );
    }

    //软触发前后的坐标
    ui->tableWidget_vector->item( table_row_number - 1, 0 )->setText( QString::number(data.pos_x_1) );
    ui->tableWidget_vector->item( table_row_number - 1, 1 )->setText( QString::number(data.pos_y_1) );
    ui->tableWidget_vector->item( table_row_number - 1, 2 )->setText( QString::number(data.pos_x_2) );
    ui->tableWidget_vector->item( table_row_number - 1, 3 )->setText( QString::number(data.pos_y_2) );
    //坐标差值
    ui->tableWidget_vector->item( table_row_number - 1, 4 )->setText( QString::number(data.pos_x_2 - data.pos_x_1 ) );
    ui->tableWidget_vector->item( table_row_number - 1, 5 )->setText( QString::number(data.pos_y_2 - data.pos_y_1) );
    //光通量
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setText( QString::number( data.detect_result ) );
    ui->tableWidget_vector->setItem( table_row_number - 1, table_col_number - 1, item );

    dealImgResult( data );
//    thd_dealResult = std::thread( &UserInterface::thd_dealImgResult, this, data );
//    thd_dealResult.detach();
}

void UserInterface::addTabWidget( QString name, QWidget* widget )
{
    QIcon icon;
    ui->tabWidget->addTab( widget, icon, name );
    ui->tabWidget->setCurrentWidget( widget );
}

void UserInterface::on_pushButton_run_process_clicked()
{
    if( ui->pushButton_run_process->text() == "继续流程" ||
        ui->pushButton_run_process->text() == "开始流程"    )
    {
        emit startProcess(0);   //开始或继续流程
    }
    else if( ui->pushButton_run_process->text() == "重新开始流程" )
    {
        emit startProcess(1);   //重新开启监听线程
    }

    ui->pushButton_run_process->setText("开始流程");
    ui->pushButton_run_process->setEnabled(false);
    ui->pushButton_stop_process->setEnabled(true);
    ui->pushButton_quit_process->setEnabled(true);
}

void UserInterface::on_pushButton_stop_process_clicked()
{
    Log_Debug( logger, "stop process ..." );
//    stop = true;
    emit stopProcess();
    ui->pushButton_run_process->setText("继续流程");
    ui->pushButton_run_process->setEnabled(true);
}

void UserInterface::on_pushButton_quit_process_clicked()
{
    emit quitProcess();
    ui->pushButton_run_process->setText("重新开始流程");
    ui->pushButton_run_process->setEnabled(true);
    ui->pushButton_stop_process->setEnabled(false);
    ui->pushButton_quit_process->setEnabled(false);
}

void UserInterface::on_pushButton_test_clicked()
{
    QString x      = ui->lineEdit_test_pos_x->text();
    QString y      = ui->lineEdit_test_pos_y->text();

    this->rcvActionInfo( ui->comboBox_test_action->currentIndex() );
}

//接收PLC发来的动作消息
void UserInterface::rcvActionInfo( int action )
{
    //收到判断光通量指令
    if( action == Action_Light_Judge )
    {
        emit takePhoto( Type_OnlyOnce );    //只拍一次
    }

    //收到粗扫指令
    if( action_state != Action_Rough_Detect && action == Action_Rough_Detect ||
        action_state != Action_Fine_Detect && action == Action_Fine_Detect )
    {
        take_photo_times = 0;
        initTableWidget();
        detectResult    = 0;        //算法最大值清零
        take_photo_end  = false;    //拍照完成标志位复位
        Log_Debug( logger, "rcvActionInfo ----> Rough or Fine Detect ..." );
        emit takePhoto( Type_Start );     //相机开始拍照
    }

    //收到粗扫运动完成指令
    if( action_state != Action_Rough_MoveOver && action == Action_Rough_MoveOver ||
        action_state != Action_Fine_MoveOver && action == Action_Fine_MoveOver )
    {
        emit takePhoto( Type_Stop );
        Log_Debug( logger, "rcvActionInfo ----> MoveOver ..." );
        thd_maxResult = std::thread( &UserInterface::thd_getMaxResult, this );
        thd_maxResult.detach();
    }

    //收到请求复检指令
    if( action_state != Action_ReDetect && take_photo_end && action == Action_ReDetect )
    {
        Log_Debug( logger, "rcvActionInfo ----> delay and ReDetect ..." );
        if( ui->lineEdit_position_3->text() == "" )
        {
            delay_ui( 20 );
        }
        else
        {
            delay_ui( ui->lineEdit_position_3->text().toInt() );
        }

        is_result_send = false;
        emit takePhoto( Type_ReDtect );    //复检时只拍一次
    }

    action_state = action;
}

void UserInterface::thd_getMaxResult()
{
    Log_Debug( logger, "enter thd_getMaxResult ..." );

    for(;;)
    {
        if( take_photo_end )
        {
            Log_Debug( logger, "write result position ..." );
            ui->lineEdit_max_value->setText( QString::number(detectResult));
            ui->lineEdit_position_1->setText( QString("( %1, %2 )")
                                              .arg( QString::number(result_pos_x_1) )
                                              .arg( QString::number(result_pos_y_1) ) );
            ui->lineEdit_position_2->setText( QString("( %1, %2 )")
                                              .arg( QString::number(result_pos_x_2) )
                                              .arg( QString::number(result_pos_y_2) ) );
//            getNextPositionOfMaxValue();

            emit sendResultPosition( detectResult > 0, result_pos_x_1, result_pos_y_1 );
            break;
        }
        else
        {
            delay_ui(1);
        }
    }

    Log_Debug( logger, "exit thd_getMaxResult ..." );
}

void UserInterface::dealImgResult( ReadData data )
{
    //当前复检状态, 先显示到界面再取最大值
    if( !is_result_send )
    {
        is_result_send = true;
        ui->lineEdit_redet_value->setText( QString::number( data.detect_result ) );

        Log_Debug( logger, "send ReDetect result === %d : %d", data.detect_result, detectResult );
        sendReDetectResult( abs( data.detect_result - detectResult ) <= 1000 );

        ui->tableWidget->setRowCount( tableRowCount + 1 );
        for( int i = 0; i < 5; i++ )
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText( "" );
            ui->tableWidget->setItem( tableRowCount, i, item );
        }

        int pos_y[3];
        pos_y[0] = ui->tableWidget_vector->item( result_index, 1 )->text().toInt();
        pos_y[1] = ui->tableWidget_vector->item( result_index + 1, 1 )->text().toInt();
        pos_y[2] = ui->tableWidget_vector->item( result_index + 2, 1 )->text().toInt();

        ui->tableWidget->item( tableRowCount, 0 )->setText( QString::number( detectResult ) );
        ui->tableWidget->item( tableRowCount, 1 )->setText( QString::number( data.detect_result ) );
        ui->tableWidget->item( tableRowCount, 2 )->setText( QString::number( abs( pos_y[1] - pos_y[0] ) ) );
        ui->tableWidget->item( tableRowCount, 3 )->setText( QString::number( abs( pos_y[2] - pos_y[0] ) ) );
        ui->tableWidget->item( tableRowCount, 4 )->setText( QString::number( abs( data.detect_result - detectResult ) ) );

        tableRowCount++;

//        Log_Debug( logger, "pos_y: %d ; %d", ui->tableWidget_vector->item( result_index, 1 )->text().toInt(),
//                                             ui->tableWidget_vector->item( result_index + 1, 1 )->text().toInt() );
//        Log_Debug( logger, "exit dealImgResult ==== times: %d", take_photo_times );
        return;
    }

    //更新光通量和坐标的最大值
    if( data.detect_result > detectResult )
    {
        detectResult   = data.detect_result;
        result_pos_x_1 = data.pos_x_1;
        result_pos_y_1 = data.pos_y_1;
        result_pos_x_2 = data.pos_x_2;
        result_pos_y_2 = data.pos_y_2;
        result_index   = take_photo_times - 1;
    }

//    Log_Debug( logger, "exit dealImgResult ==== times: %d", take_photo_times );
}

void UserInterface::rcvTakePhotoEnd()
{
    take_photo_end = true;
}

void UserInterface::on_pushButton_search_value_clicked()
{
    QString     search_value = ui->lineEdit_search_value->text();
    QStringList list_value;

    int selected_row  = -1;
    int search_column = 0;
    int index = ui->comboBox_search_value->currentIndex();

    switch( index )
    {
        case 0:
        case 1:
        {
            search_column = index;
            break;
        }

        case 2:
        case 3:
        case 4:
        {
            search_column = index + 2;
            break;
        }

        default:
        {
            QMessageBox::warning( NULL, "警告", "查找列超出范围" );
            return;
        }
    }

    for( int i = 0; i < table_row_number; i++ )
    {
        if( ui->tableWidget_vector->item( i, search_column )->text() == search_value )
        {
            list_value << QString::number( i );

            if( selected_row  == -1 )
            {
                selected_row = i;
            }
        }
    }

    if( list_value.size() == 0 )
    {
        QMessageBox::warning( NULL, "警告", "No find !!" );
        return;
    }

    //显示查找结果
    ui->lineEdit_search_result->setText( list_value.join(",") );
    ui->tableWidget_vector->item( selected_row, search_column )->setSelected(true);
}

void UserInterface::on_pushButton_search_max_value_clicked()
{
    int max_value     = 0;
    int current_value = 0;

    int selected_row  = -1;
    int search_column = 0;
    int index = ui->comboBox_search_value->currentIndex();

    switch( index )
    {
        case 0:
        case 1:
        {
            search_column = index;
            break;
        }

        case 2:
        case 3:
        case 4:
        {
            search_column = index + 2;
            break;
        }

        default:
        {
            QMessageBox::warning( NULL, "警告", "查找列超出范围" );
            return;
        }
    }

    for( int i = 0; i < table_row_number; i++ )
    {
        current_value = ui->tableWidget_vector->item( i, search_column )->text().toInt();

        if( abs( max_value ) <= abs( current_value ) )
        {
            max_value    = current_value;
            selected_row = i;
        }
    }

    ui->lineEdit_maxValue->setText( QString::number( max_value ) );
    ui->tableWidget_vector->item( selected_row, search_column )->setSelected(true);
}

void UserInterface::getNextPositionOfMaxValue()
{
    QStringList list_row;
    int  current_value = 0;
    int  current_row   = 0;

    int  pos_x, pos_y;
    bool result        = false;

    for( int i = 0; i < table_row_number; i++ )
    {
        current_value = ui->tableWidget_vector->item( i, 6 )->text().toInt();

        if( current_value == detectResult )
        {
            list_row << QString::number( i );
        }
    }

    for( int i = 0; i < list_row.size(); i++ )
    {
        current_row = list_row.at(i).toInt();
        if( result_pos_x_1 == ui->tableWidget_vector->item( current_row, 0 )->text().toInt()
         && result_pos_y_1 == ui->tableWidget_vector->item( current_row, 1 )->text().toInt() )
        {
            result = true;
            pos_x = ui->tableWidget_vector->item( current_row, 0 )->text().toInt();
            pos_y = ui->tableWidget_vector->item( current_row, 1 )->text().toInt();
            ui->lineEdit_position_3->setText( QString("( %1, %2 )")
                                              .arg( QString::number(pos_x) )
                                              .arg( QString::number(pos_y) ) );
            return;
        }
    }

    QMessageBox::warning( this, "警告", "查找最大光通量的下一个位置错误 !!" );
}

void UserInterface::rcvDeviceInfoFromPLC( QString info )
{
    ui->textEdit->append( info );
}
