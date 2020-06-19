#include "mainwindow.h"
#include "ui_mainwindow.h"

SerialPortService * sp_svc;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    sp_svc = new SerialPortService;
    sp_svc->bzh_SerialPortService_pkg_init();

    QObject::connect(this,
                     SIGNAL(signal_serialport_svc_port_changed(QSerialPortInfo)),
                     sp_svc,
                     SLOT(slot_serialport_svc_port_changed(QSerialPortInfo)));
    QObject::connect(this,
                     SIGNAL(signal_serialport_svc_port_connect()),
                     sp_svc,
                     SLOT(slot_serialport_svc_port_connect()));
    QObject::connect(this,
                     SIGNAL(signal_serialport_svc_port_disconnect()),
                     sp_svc,
                     SLOT(slot_serialport_svc_port_disconnect()));
    QObject::connect(this,
                     SIGNAL(signal_serialport_svc_write_panding()),
                     sp_svc,
                     SLOT(slot_serialport_svc_write_panding()));

    QObject::connect(sp_svc,
                     SIGNAL(signal_serialport_svc_init_ready()),
                     this,
                     SLOT(slot_serialport_svc_init_ready()));

    QObject::connect(sp_svc,
                     SIGNAL(signal_serialport_svc_data_ready()),
                     this,
                     SLOT(slot_serialport_svc_data_ready()));

    ui->setupUi(this);

    //ui init
    this->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    ui->cb_serial_status->setEnabled(false);
    ui->cb_sensor_status->setEnabled(false);
    ui->pb_serial_oepn->setEnabled(false);
    ui->pb_serial_close->setEnabled(false);
    ui->gb_control->setEnabled(false);
    ui->le_lp->setMaxLength(LP_LEN);

    //serial init
    ui->comboBox_serial_ports->clear();
    sp_svc->bzh_SerialPortService_available_port_refresh();

    foreach(QSerialPortInfo info, sp_svc->bzh_SerialPortService_pkg.serialports)
    {
        ui->comboBox_serial_ports->addItem(info.portName());
    }
    if(ui->comboBox_serial_ports->count()>0)
        ui->pb_serial_oepn->setEnabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::slot_serialport_svc_init_ready()
{
    qDebug() << "init ready" << Qt::endl;
    qDebug() << sp_svc->bzh_SerialPortService_pkg.serial_port_p->portName() << Qt::endl;
}

void MainWindow::slot_serialport_svc_data_ready()
{
    qDebug() << "data ready" << Qt::endl;
    switch(sp_svc->bzh_SerialPortService_pkg.recv_data[0])
    {
    case SIG_SENSOR_IN_PRESSED:
        ui->pte_terminal->insertPlainText("SIG_SENSOR_IN_PRESSED received \n\r");
        ui->cb_sensor_status->setCheckState(Qt::Checked);
        break;
    case SIG_SENSOR_IN_RELEASED:
        ui->pte_terminal->insertPlainText("SIG_SENSOR_IN_RELEASED received \n\r");
        ui->cb_sensor_status->setCheckState(Qt::Unchecked);
        break;
    case SIG_SENSOR_OUT_PRESSED:
        ui->pte_terminal->insertPlainText("SIG_SENSOR_OUT_PRESSED received \n\r");
        ui->cb_sensor_status->setCheckState(Qt::Checked);
        break;
    case SIG_SENSOR_OUT_RELEASED:
        ui->pte_terminal->insertPlainText("SIG_SENSOR_OUT_RELEASED received \n\r");
        ui->cb_sensor_status->setCheckState(Qt::Unchecked);
        break;
    default:
        break;
    }
    ui->pte_terminal->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
}

void MainWindow::on_pb_serial_oepn_clicked()
{
    sp_svc->bzh_SerialPortService_pkg.loop = true;
    sp_svc->start();
    emit signal_serialport_svc_port_connect();

    ui->pb_serial_oepn->setEnabled(false);
    ui->pb_serial_close->setEnabled(true);
    ui->gb_control->setEnabled(true);
    ui->cb_serial_status->setCheckState(Qt::Checked);
}

void MainWindow::on_pb_serial_close_clicked()
{
    emit signal_serialport_svc_port_disconnect();
    sp_svc->quit();

    ui->pb_serial_oepn->setEnabled(true);
    ui->pb_serial_close->setEnabled(false);
    ui->gb_control->setEnabled(false);
    ui->cb_serial_status->setCheckState(Qt::Unchecked);
}

void MainWindow::on_pb_gate_open_clicked()
{
    sp_svc->bzh_SerialPortService_pkg.send_data[0] = CMD_GATE_OPEN;
    sp_svc->bzh_SerialPortService_pkg.send_data[1] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[2] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[3] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[4] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[5] = 0;
    emit signal_serialport_svc_write_panding();
}

void MainWindow::on_pb_gate_close_clicked()
{
    sp_svc->bzh_SerialPortService_pkg.send_data[0] = CMD_GATE_CLOSE;
    sp_svc->bzh_SerialPortService_pkg.send_data[1] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[2] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[3] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[4] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[5] = 0;
    emit signal_serialport_svc_write_panding();
}

void MainWindow::on_pb_gate_ignore_clicked()
{
    sp_svc->bzh_SerialPortService_pkg.send_data[0] = CMD_GATE_IGNORE;
    sp_svc->bzh_SerialPortService_pkg.send_data[1] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[2] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[3] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[4] = 0;
    sp_svc->bzh_SerialPortService_pkg.send_data[5] = 0;
    emit signal_serialport_svc_write_panding();
}

void MainWindow::on_pb_display_lp_clicked()
{
    if(ui->le_lp->text().length() != LP_LEN)
    {
        QMessageBox::critical(this,"Error","License plate must be 5 characters!");
        return ;
    }

    QString lp = ui->le_lp->text();
    sp_svc->bzh_SerialPortService_pkg.send_data[0] = CMD_DISPLAY_LP;
    sp_svc->bzh_SerialPortService_pkg.send_data[1] = lp.at(0).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[2] = lp.at(1).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[3] = lp.at(2).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[4] = lp.at(3).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[5] = lp.at(4).toLatin1();
    emit signal_serialport_svc_write_panding();
}

void MainWindow::on_comboBox_serial_ports_currentIndexChanged(int index)
{
    if(ui->comboBox_serial_ports->currentIndex()!=-1)
    {
        emit signal_serialport_svc_port_changed(sp_svc->bzh_SerialPortService_pkg.serialports.at(index));
    }
}
