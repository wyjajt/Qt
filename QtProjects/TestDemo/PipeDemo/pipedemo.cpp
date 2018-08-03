#include "pipedemo.h"
#include "ui_pipedemo.h"

using namespace std;

PipeDemo::PipeDemo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PipeDemo)
{
    ui->setupUi(this);
    QObject::connect( this, SIGNAL( sendInfo(QString) ),
             this, SLOT( rcvInfo(QString) ) );
}

PipeDemo::~PipeDemo()
{
    delete ui;
    std::cout << "55555";
}

void PipeDemo::execmd( const char* cmd, char* result )
{
    char buffer[128]; //定义缓冲区
    FILE* pipe = _popen( cmd, "r" ); //打开管道，并执行命令
    if(!pipe)
    {
        return; //返回0表示运行失败
    }

    std::cout << "11111"<< endl;


//    if( fgets( buffer, 128, pipe ) != NULL )
    while( !feof( pipe ) )
    {
        if( fgets( buffer, 128, pipe ) )
        { //将管道输出到result中
            strcat( result, buffer );
            std::cout << "22222"<< buffer;
            emit sendInfo( QString::fromLocal8Bit(buffer) );

            break;
        }
    }

    std::cout << "33333"<< endl;

    _pclose( pipe ); //关闭管道

    std::cout << "44444"<< endl;
    return; //返回1表示运行成功
}

void PipeDemo::on_pushButton_clicked()
{
    char result[128] = ""; //定义存放结果的字符串数组

//    QString str = ui->lineEdit->text();
//    if( 1 == execmd( str.toLatin1().data(), result ) )
//    {
//        ui->textEdit->append( result );
//    }

    thd = std::thread( &PipeDemo::execmd, this, "hello.exe", result );

//    ui->textEdit->append( result );
//    execmd( "hello.exe", result );
//    system( "hello.exe" );
}

void PipeDemo::rcvInfo( QString str )
{
    ui->textEdit->append( str );
}
