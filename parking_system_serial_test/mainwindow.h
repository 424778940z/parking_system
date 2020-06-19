#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

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
    void on_pb_serial_oepn_clicked();

    void on_pb_serial_close_clicked();

    void on_pb_gate_open_clicked();

    void on_pb_gate_close_clicked();

    void on_pb_gate_ignore_clicked();

    void on_pb_display_lp_clicked();

    void on_comboBox_serial_ports_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
