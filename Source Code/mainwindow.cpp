#include "mainwindow.h"
#include "ui_mainwindow.h"


//定义一个托盘类，用来显示托盘和气泡提示
QSystemTrayIcon *tray;

QTimer *scan_timer;

//定义一个容器，用来存储当前的COM口列表
QVector<QString> com_now_vec;
//定义一个容器，用来存储扫面的COM口列表
QVector<QString> com_scan_vec;
//定义一个容器，用来存储显示的COM口列表
QVector<QString> com_view_vec;

QList<QSerialPortInfo>list;

bool notify_en = true;

//配置托盘
static void _tray_config (QMainWindow * MainWindow)
{

    tray = new QSystemTrayIcon(QIcon(":/ico/res/com.ico"), MainWindow);
    QMenu *menu = new QMenu(MainWindow);


    QAction *autorun_action = new QAction(QObject::tr("开机自启"));
    QAction *msg_action = new QAction(QObject::tr("启用通知"));
    QAction *exit_action = new QAction(QObject::tr("退出"));


    autorun_action->setCheckable(true);
    autorun_action->setChecked(true);
    menu->addAction(autorun_action);
    msg_action->setCheckable(true);
    msg_action->setChecked(true);
    menu->addAction(msg_action);
    menu->addAction(exit_action);


    MainWindow::connect(exit_action,SIGNAL(triggered()),MainWindow,SLOT(exit_action_triggered()));
    MainWindow::connect(autorun_action,SIGNAL(triggered(bool)),MainWindow,SLOT(autorun_action_triggered(bool)));
    MainWindow::connect(msg_action,SIGNAL(triggered(bool)),MainWindow,SLOT(msg_action_triggered(bool)));

    tray->setContextMenu(menu);

    tray->setToolTip("ComNotify");
    // 显示图标
    tray->show();
}

//保存启动时的可用串口
static void _com_port_vec_init (void)
{
    /*获取当前可用的串口*/
    list=QSerialPortInfo::availablePorts();

    for(int i=0; i<list.size();i++){
        /*将串口号保存到vector*/
        com_scan_vec.append(list.at(i).portName());
        com_now_vec.append(list.at(i).portName());
    }
}

//扫描当前所有串口,并检测新加入串口
void MainWindow::scan_serial_com_port(void)
{
    /*获取当前可用的串口*/
    QString str=NULL;
    /*连续扫描*/
//    for(int i=0;i<5;i++){
        list=QSerialPortInfo::availablePorts();
    //     QThread::msleep(30);
    // }
    com_scan_vec.clear();
    for(int i=0; i<list.size();i++){

        /*新增串口*/
        if(com_now_vec.contains(list.at(i).portName())==false && notify_en == true){
                /*连续扫描，保证同时插入的串口能一次扫描到*/
                for(int i=0;i<5;i++){
                    list=QSerialPortInfo::availablePorts();
                    QThread::msleep(30);
                }
                for(int i=0; i<list.size();i++){
                    /*记录扫描到的串口*/
                    com_scan_vec.append(list.at(i).portName());
                    if(com_now_vec.contains(list.at(i).portName())==false && notify_en == true){
                        str+=list.at(i).description()+"("+list.at(i).portName()+")\r\n";
                    }
                }
                break;
        }
        /*记录扫描到的串口*/
        com_scan_vec.append(list.at(i).portName());
    }
    if(str != NULL){
        tray->showMessage(QObject::tr("检测到新串口"),str,QIcon(":/ico/res/com.ico"));
    }
    com_now_vec = com_scan_vec;

}

// /*新增串口*/
// if(com_now_vec.contains(list.at(i).portName())==false && notify_en == true){

// }
// com_scan_vec.append(list.at(i).portName());

//右键退出
void MainWindow::exit_action_triggered (void)
{
    QApplication::exit(0);
}

//设置程序自启动
void MainWindow::autorun_action_triggered(bool checked)
{
    //打开注册表
    QSettings *reg = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    //以程序名称作为注册表中的键
    QString key = qApp->applicationName();
    QString value = qApp->applicationFilePath().replace("/","\\");

    if(checked == true){//打开开机自启
        QString _val = reg->value(key).toString();
         if(_val !=value){
             reg->setValue(key,value);
        }
    }else{//关闭开机自启
        reg->remove(key);
    }
    delete reg;
}

//弹窗控制
void MainWindow::msg_action_triggered(bool checked)
{
    if(checked == true){//打开消息弹窗,启动定时器
        notify_en = true;
    }else{//关闭消息弹窗，关闭定时器
        notify_en = false;
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _tray_config(this);
    _com_port_vec_init();
    //启动后默认自启
    autorun_action_triggered(true);
    //创建定时器，定时扫描串口
    scan_timer = new QTimer(this);
    connect(scan_timer, SIGNAL(timeout()), this, SLOT(scan_serial_com_port()));
    scan_timer->start(200);
    tray->showMessage("",QObject::tr("ComNotify已启动"),QSystemTrayIcon::NoIcon);
}

MainWindow::~MainWindow()
{
    delete ui;
}
