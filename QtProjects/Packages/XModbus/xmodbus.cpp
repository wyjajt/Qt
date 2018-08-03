#include "xmodbus.h"

XModbus::XModbus( QString logName )
{
    modbusClient = nullptr;
    dataBuffer   = nullptr;
    isConnected  = false;

    modbusParameter.protocol        = "TcpModbus";
    modbusParameter.portName        = "COM1";
    modbusParameter.baudRate        = 9600;
    modbusParameter.parity          = "N";
    modbusParameter.dataBit         = 8;
    modbusParameter.stopBit         = 1.0;
    modbusParameter.ipAddress       = "127.0.0.1";
    modbusParameter.portNumber      = 502;
    modbusParameter.startRegisterID = 0;
    modbusParameter.registerNumber  = 100;
    modbusParameter.slaveAddress    = 1;

    auto initLog = InitClassLog() ;
    initLog( logger, logName.toStdString(), "XModbus" );

    Log_Debug( logger, "init XModbus ..." );
}

XModbus::~XModbus()
{
    if( nullptr != modbusClient )
    {
        modbus_close(modbusClient);
        modbus_free(modbusClient);
    }

    Log_Debug( logger, "~ XModbus" );
}

void XModbus::setParameters( ConnectParameter para )
{
    modbusParameter.protocol        = para.protocol;
    modbusParameter.portName        = para.portName;
    modbusParameter.baudRate        = para.baudRate;
    modbusParameter.parity          = para.parity;
    modbusParameter.dataBit         = para.dataBit;
    modbusParameter.stopBit         = para.stopBit;
    modbusParameter.ipAddress       = para.ipAddress;
    modbusParameter.portNumber      = para.portNumber;
    modbusParameter.startRegisterID = para.startRegisterID;
    modbusParameter.registerNumber  = para.registerNumber;
    modbusParameter.slaveAddress    = para.slaveAddress;

    if( dataBuffer )
    {
        delete dataBuffer;
        dataBuffer = new ushort[modbusParameter.registerNumber];
    }
    else
    {
        dataBuffer = new ushort[modbusParameter.registerNumber];
    }
}

bool XModbus::connectToModbus()
{
    Log_Debug( logger, "enter ..." );
    int res = 0;

    if( nullptr != modbusClient )
    {
        modbus_close( modbusClient );
    }

    if( modbusParameter.protocol == PROTOCOL_SERIAL )
    {
        modbusClient = modbus_new_rtu( modbusParameter.portName.toLatin1().data(),
                                       modbusParameter.baudRate,
                                       modbusParameter.parity.toLatin1().data()[0],
                                       modbusParameter.dataBit,
                                       modbusParameter.stopBit );
        Log_Debug( logger, "portName --> %s, baudRate --> %d, parity --> %c, "
                           "dataBit --> %d, stopBit --> %f, slaveAddress --> %d ."
                         , modbusParameter.portName.toLatin1().data()
                         , modbusParameter.baudRate
                         , modbusParameter.parity.toLatin1().data()[0]
                         , modbusParameter.dataBit
                         , modbusParameter.stopBit
                         , modbusParameter.slaveAddress );
    }
    else
    {
        modbusClient = modbus_new_tcp( modbusParameter.ipAddress.toLatin1().data(),
                                       modbusParameter.portNumber );
        Log_Debug( logger, "ipAddress --> %s, portNumber --> %d, slaveAddress --> %d ."
                         , modbusParameter.ipAddress.toLatin1().data()
                         , modbusParameter.portNumber
                         , modbusParameter.slaveAddress );

    }

    if( nullptr == modbusClient )
    {
        return false;
    }

    res = modbus_set_slave( modbusClient, modbusParameter.slaveAddress );  //设置modbus从机地址
    if( res == -1 )
    {
        Log_Debug( logger, "Invalid slave ID %d", modbusParameter.slaveAddress );

        return false;
    }

    res = modbus_connect( modbusClient ); //链接modbus设备
    if( res == -1 )
    {
        Log_Error(logger, "connect to modbus failed !!" );

        return false;
    }

    //设置modbus超时时间为900毫秒
    res = modbus_set_response_timeout( modbusClient, 0, 900000 );
    if( res == -1 )
    {
        Log_Error( logger, "modbus set response timeout failed !!" );

        return false;
    }

    isConnected = true;
    Log_Debug( logger, "connect to modbus success ..." );

    initDataBuffer();

    return true;
}

void XModbus::disconnectFromSlave()
{
    if( modbusClient )
    {
        modbus_close( modbusClient );
        modbus_free( modbusClient );
    }

    modbusClient = nullptr;
}

bool XModbus::reconnectToModbus()
{
    Log_Debug( logger, "enter ..." );

    modbus_close( modbusClient );

    int res = modbus_connect( modbusClient );

    //第一次连接失败
    if( -1 == res )
    {
        res = modbus_connect( modbusClient );

        //第二次连接失败
        if( -1 == res )
        {
            Log_Warn( logger, "重新连接modbus从机失败！！！ --> %d", res );

            return false;
        }

        res = modbus_set_response_timeout( modbusClient, 0, 900000 );

        //设置回复超时时间失败
        if( -1 == res )
        {
            Log_Warn( logger, "设置modbus从机回复超时时间失败！！！ --> %d", res );
            return false;
        }

        Log_Debug( logger, "重新连接modbus从机成功！！！" );
        return true;
    }

    res = modbus_set_response_timeout( modbusClient, 0, 900000 );

    //设置回复超时时间失败
    if( -1 == res )
    {
        Log_Warn( logger, "设置modbus从机回复超时时间失败！！！ --> %d", res );
        return false;
    }

    Log_Debug( logger, "重新连接modbus从机成功！！！" );
    return true;
}

void XModbus::initDataBuffer()
{
    Log_Debug( logger, "enter init dataBuffer ..." );

    int number = modbusParameter.registerNumber;
    memset( dataBuffer, 0, ( number + 1 ) * sizeof(short) );

    bool result = readRegisterValues( modbusParameter.startRegisterID, number );

    if( !result )
    {
        Log_Error( logger, "init dataBuffer failed !!!" );
        return;
    }

    Log_Debug( logger, "init dataBuffer success ..." );
}

bool XModbus::readRegisterValues( int address, int number )
{
    bool readResult = true;
    int  readTimes  = number / 125 + ( number % 125 == 0 ? 0 : 1 );

    for( int i = 0; i < readTimes; i++ )
    {
        Log_Debug( logger, "loop --> %d", i );

        readResult = readAllowedLengthOfValues( address + i * 125,
                                                i < readTimes - 1 ? 125 : number - i * 125 );
                                        //( i < (readTimes - 1) ) ? 125 : ( number - i * 125 )
        if( !readResult )
        {
            break;
        }
        else
        {
            Delay_ms(1);
        }
    }

    return readResult;
}

bool XModbus::writeRegisterValues( int address, int number, ushort *data )
{
    bool writeResult = true;
    int  writeTimes  = number / 125 + ( number % 125 == 0 ? 0 : 1 );

    for( int i = 0; i < writeTimes; i++ )
    {
        Log_Debug( logger, "loop --> %d", i );

        writeResult = writeAllowedLengthOfValues( address + i * 125,
                                                  i < writeTimes - 1 ? 125 : number - i * 125,
                                                  data + i * 125 );
        if( !writeResult )
        {
            break;
        }
        else
        {
            Delay_ms(1);
        }
    }

    if( writeResult )
    {
        memcpy( dataBuffer + address - modbusParameter.startRegisterID, data, number );
    }

    return writeResult;
}

bool XModbus::readAllowedLengthOfValues( int address, int number )
{
    TIME_TYPE startTime, endTime;
    int rc;
    TIME_NOW( startTime );

    while( true )
    {
        rc = modbus_read_registers( modbusClient, address, number,
                                    dataBuffer + address - modbusParameter.startRegisterID
                                    );//读取寄存器的数据并存入缓冲区
        if( rc >= 0 )
        {
            return true;
        }
        else
        {
            Log_Warn( logger, "modbus read register error !!! error info --> %s, "
                              "res --> %d, reg --> %d",
                              modbus_strerror(errno), rc, address );
        }

        reconnectToModbus();

        TIME_NOW( endTime );

        if( DIFF_MS(startTime, endTime) / 1000 > 5 )
        {
            Log_Error( logger, "读取寄存器数据超时！！！" );
            return false;
        }

        Delay_ms( 100 );
    }
}

bool XModbus::writeAllowedLengthOfValues( int address, int number, ushort *data )
{
    TIME_TYPE startTime, endTime;
    int rc;
    TIME_NOW( startTime );

    while( true )
    {
        rc = modbus_write_registers( modbusClient, address, number, data ); //缓冲区的数据写入寄存器

        if( rc >= 0 )
        {
            return true;
        }
        else
        {
            Log_Warn( logger, "modbus write register error !!! error info --> %s, "
                              "res --> %d, reg --> %d",
                              modbus_strerror(errno), rc, address );
        }

        reconnectToModbus();

        TIME_NOW( endTime );

        if( DIFF_MS(startTime, endTime) / 1000 > 10 )
        {
            Log_Error( logger, "写寄存器数据超时！！！" );
            return false;
        }

        Delay_ms( 100 );
    }
}

