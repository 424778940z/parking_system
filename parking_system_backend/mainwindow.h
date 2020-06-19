#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>

#include <QMessageBox>
#include <QDebug>
#include <QThread>

//database
#include "db_wrapper.h"

//opencv
#include "lpr_wrapper.h"

//serial superio
#include "serial_service.h"
#include "parking_system_superio_protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void signal_serialport_svc_port_changed(QSerialPortInfo info);
    void signal_serialport_svc_port_connect();
    void signal_serialport_svc_port_disconnect();
    void signal_serialport_svc_write_panding();

public slots:
    void slot_serialport_svc_init_ready();
    void slot_serialport_svc_data_ready();

private slots:

    //db
    void on_pb_a_clicked();

    void on_pb_b_clicked();

    void on_pb_c_clicked();

    //lpr
    void on_pb_opencv_test_clicked();

    //serial superio
    void on_pb_serial_oepn_clicked();

    void on_pb_serial_close_clicked();

    void on_pb_gate_open_clicked();

    void on_pb_gate_close_clicked();

    void on_pb_gate_ignore_clicked();

    void on_pb_display_lp_clicked();

    void on_comboBox_serial_ports_currentIndexChanged(int index);
private:    
    Ui::MainWindow *ui;

private:
    bool vehicle_process(bool in_out);
    bool isGateOpen  = false;
    //bool cmd_send(uint8_t cmd);
};
#endif // MAINWINDOW_H
