#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QVector>
#include <QSet>
#include <QDebug>
#include <QTimer>

#include <QSystemTrayIcon>

#include <QDir>
#include <QSettings>

#include <QLocale>
#include <QTranslator>

#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void scan_serial_com_port();
    void exit_action_triggered();
    void autorun_action_triggered(bool checked);
    void msg_action_triggered(bool checked);
};
#endif // MAINWINDOW_H
