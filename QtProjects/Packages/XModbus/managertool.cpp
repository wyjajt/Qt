#include "managertool.h"
#include "ui_managertool.h"

ManagerTool::ManagerTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManagerTool)
{
    ui->setupUi(this);

    auto func = InitModuleLog();
    func( logger, LOG_NAME, PatternType::PatternDate, "" );

    Log_Debug( logger, "ManagerTool loaded ..." );

    xModbus = new XModbus( LOG_NAME );

    initParameterAndUi();
    initSignalConnection();

}

ManagerTool::~ManagerTool()
{
    delete ui;

    if( xModbus )
    {
        delete xModbus;
    }

    xModbus = nullptr;
}

void ManagerTool::initParameterAndUi()
{
    parameter.protocol        = PROTOCOL_SERIAL;
    parameter.portName        = "COM1";
    parameter.baudRate        = 9600;
    parameter.parity          = "N";
    parameter.dataBit         = 8;
    parameter.stopBit         = 1.0;
    parameter.ipAddress       = "192.168.0.165";
    parameter.portNumber      = 502;
    parameter.startRegisterID = 1000;
    parameter.registerNumber  = 200;
    parameter.slaveAddress    = 1;

    //串口
    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        ui->comboBox_portname->addItem( info.portName() );
    }

    //波特率
    QString baudrate = "1200,1800,2400,4800,9600,14400,19200,38400,56000,57600,76800,115200,128000,256000";
    ui->comboBox_baudrate->addItems( baudrate.split(",") );


//    Log_Debug( logger, " 1: %d; 2: %d; 3: %d",
//               xModbus->dataBuffer[0],
//               xModbus->dataBuffer[1],
//               xModbus->dataBuffer[2] );


    ui->comboBox_connect_type->setCurrentText( parameter.protocol );
    ui->comboBox_portname->setCurrentText( parameter.portName );
    ui->comboBox_baudrate->setCurrentText( QString::number(parameter.baudRate) );
    ui->comboBox_parity->setCurrentText( parameter.parity );
    ui->comboBox_databit->setCurrentText( QString::number(parameter.dataBit) );
    ui->comboBox_stopbit->setCurrentText( QString::number(parameter.stopBit, 'f', 1) );

    ui->lineEdit_ip_address->setText( parameter.ipAddress );
    ui->lineEdit_slave_address->setText( QString::number(parameter.slaveAddress) );
    ui->lineEdit_port_number->setText( QString::number(parameter.portNumber) );
    ui->lineEdit_start_address->setText( QString::number(parameter.startRegisterID) );
    ui->lineEdit_register_number->setText( QString::number(parameter.registerNumber) );
    ui->lineEdit_slave_address->setText( QString::number(parameter.slaveAddress) );

    ui->stackedWidget->setCurrentIndex( 0 );
    ui->pushButton_disconnect->setEnabled( false );
}

void ManagerTool::getParameterFromUi()
{
    parameter.protocol        = ui->comboBox_connect_type->currentText();
    parameter.portName        = ui->comboBox_portname->currentText();
    parameter.baudRate        = ui->comboBox_baudrate->currentText().toInt();
    parameter.parity          = ui->comboBox_parity->currentText();
    parameter.dataBit         = ui->comboBox_databit->currentText().toInt();
    parameter.stopBit         = ui->comboBox_stopbit->currentText().toDouble();
    parameter.ipAddress       = ui->lineEdit_ip_address->text();
    parameter.portNumber      = ui->lineEdit_port_number->text().toInt();
    parameter.startRegisterID = ui->lineEdit_start_address->text().toInt();
    parameter.registerNumber  = ui->lineEdit_register_number->text().toInt();
    parameter.slaveAddress    = ui->lineEdit_slave_address->text().toInt();
}

void ManagerTool::initSignalConnection()
{
    /*  控件相互关联  */
    QObject::connect( ui->comboBox_connect_type, SIGNAL( currentIndexChanged(int) ),
                      ui->stackedWidget, SLOT( setCurrentIndex(int) ) );
    QObject::connect( ui->pushButton_clear_tips, SIGNAL( pressed() ),
                      ui->textEdit, SLOT( clear() ) );

    /*  控件和私有槽函数关联  */
    QObject::connect( ui->pushButton_connect,       SIGNAL( pressed() ), this, SLOT( connectToModbus() ) );
    QObject::connect( ui->pushButton_disconnect,    SIGNAL( pressed() ), this, SLOT( disConnectFromModbus() ) );
    QObject::connect( ui->pushButton_read,          SIGNAL( pressed() ), this, SLOT( readRegisterValues() ) );
    QObject::connect( ui->pushButton_search,        SIGNAL( pressed() ), this, SLOT( readDataBuffer() ) );
    QObject::connect( ui->pushButton_single_write,  SIGNAL( pressed() ), this, SLOT( writeSingleValueToRegisters() ) );
    QObject::connect( ui->pushButton_multiple_write,SIGNAL( pressed() ), this, SLOT( writeMultipleValuesToRegisters() ) );

    Log_Debug( logger, "init all SignalConnection success ..." );
}


/*  槽函数 */
void ManagerTool::connectToModbus()
{
    getParameterFromUi();

    xModbus->setParameters( parameter );

    if( !xModbus->connectToModbus() )
    {
        ui->textEdit->append( "modbus connect failed !!!" );
        return;
    }

    ui->textEdit->append( "modbus connect success ..." );
    ui->pushButton_disconnect->setEnabled( true );
}

void ManagerTool::disConnectFromModbus()
{
    if( !xModbus )
    {
        Log_Error( logger, "modbus not connected !!!" );
        ui->textEdit->append( "modbus not connected !!!" );
        return;
    }

    xModbus->disconnectFromSlave();

    ui->pushButton_disconnect->setEnabled( false );
}

void ManagerTool::readDataBuffer()
{
    if( !xModbus )
    {
        Log_Error( logger, "modbus not connected !!!" );
        ui->textEdit->append( "modbus not connected !!!" );
        return;
    }

    QString str_position = ui->lineEdit_search_position->text();
    QString str_number   = ui->lineEdit_search_number->text();
    int     position     = str_position.toInt();
    int     number       = str_number.toInt();

    if( str_position == "" || str_number == "" || number <= 0 || position < 0 ||
        position + number > parameter.registerNumber )
    {
        ui->textEdit->append( "查询参数错误 !!!" );
        return;
    }

    for( int i = 0; i < number; i++ )
    {
        ui->textEdit->append( QString("%1 -> %2")
                              .arg( QString::number( position + i ) )
                              .arg( QString::number( xModbus->dataBuffer[position + i] ) )
                            );
    }
}

void ManagerTool::readRegisterValues()
{
    if( !xModbus )
    {
        Log_Error( logger, "modbus not connected !!!" );
        ui->textEdit->append( "modbus not connected !!!" );
        return;
    }

    QString str_address = ui->lineEdit_target_address->text();
    QString str_number  = ui->lineEdit_target_number->text();
    int     address     = str_address.toInt();
    int     number      = str_number.toInt();

    if( str_address == "" || str_number == "" || number <= 0 || address < parameter.startRegisterID ||
        address + number > parameter.startRegisterID + parameter.registerNumber )
    {
        ui->textEdit->append( "输入参数错误 !!!" );
        return;
    }

    if( !xModbus->readRegisterValues( address, number ) )
    {
        ui->textEdit->append( "read registers error !!!" );
    }
    else
    {
        ui->textEdit->append( "read registers success ..." );
    }
}

void ManagerTool::writeSingleValueToRegisters()
{
    if( !xModbus )
    {
        Log_Error( logger, "modbus not connected !!!" );
        ui->textEdit->append( "modbus not connected !!!" );
        return;
    }

    QString str_address = ui->lineEdit_target_address->text();
    QString str_number  = ui->lineEdit_target_number->text();
    QString str_value   = ui->lineEdit_target_single_value->text();
    int     address     = str_address.toInt();
    int     number      = str_number.toInt();
    int     value       = str_value.toInt();

    if( str_address == "" || str_number == "" || str_value == "" ||
        number <= 0 || address < parameter.startRegisterID ||
        address + number > parameter.startRegisterID + parameter.registerNumber )
    {
        ui->textEdit->append( "输入参数错误 !!!" );
        return;
    }

//    ushort writeData[ number + 1 ];                               //长度不是常量，不能以数组方式创建内存
    ushort *writeData = new ushort[ number + 1 ];
//    memset( writeData, value, ( number + 1 ) * sizeof(short) );   //赋值错误，一个寄存器两个字节
    for( int i = 0; i < number; i++ )
    {
        writeData[i] = (ushort)value;
    }

    if( !xModbus->writeRegisterValues( address, number, writeData ) )
    {
        ui->textEdit->append( "write registers error !!!" );
    }
    else
    {
        ui->textEdit->append( "write registers success ..." );
    }
}

void ManagerTool::writeMultipleValuesToRegisters()
{
    if( !xModbus )
    {
        Log_Error( logger, "modbus not connected !!!" );
        ui->textEdit->append( "modbus not connected !!!" );
        return;
    }

    QString str_address = ui->lineEdit_target_address->text();
    QString str_values  = ui->lineEdit_multiple_values->text();
    int     address     = str_address.toInt();
    int     number      = str_values.split(",").size();

    if( str_address == "" || str_values == "" || address < parameter.startRegisterID ||
        address + number > parameter.startRegisterID + parameter.registerNumber )
    {
        ui->textEdit->append( "输入参数错误 !!!" );
        return;
    }

    ushort *writeData = new ushort[ number + 1 ];
    QStringList list_value = str_values.split(",");
    for( int i = 0; i < number; i++ )
    {
        writeData[i] = list_value.at(i).toInt();
    }

    if( !xModbus->writeRegisterValues( address, number, writeData ) )
    {
        ui->textEdit->append( "write registers error !!!" );
    }
    else
    {
        ui->textEdit->append( "write registers success ..." );
    }
}
