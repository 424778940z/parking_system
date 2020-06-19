#include "mainwindow.h"
#include "ui_mainwindow.h"

SerialPortService * sp_svc;
db_wrapper * dbw = new db_wrapper("main_db.sqlite");
QSqlTableModel  *dbmd_current = new QSqlTableModel;
QSqlTableModel  *dbmd_history = new QSqlTableModel;

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

    //main window
    this->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowTitle("Parking System");

    //serial
    ui->cb_serial_status->setEnabled(false);
    ui->pb_serial_oepn->setEnabled(false);
    ui->pb_serial_close->setEnabled(false);
    ui->le_lp->setMaxLength(LP_LEN);

    //db
    qDebug() << dbw->open();
    qDebug() << dbw->init();
    qDebug() << dbw->purge();
    ui->tview_db_current->setModel(dbw->dbmd_current);
    ui->tview_db_current->setAlternatingRowColors(true);
    ui->tview_db_current->horizontalHeader()->setStretchLastSection(true);
    ui->tview_db_current->verticalHeader()->hide();
    ui->tview_db_current->hideColumn(3);
    //ui->tview_db_current->resizeColumnsToContents();

    ui->tview_db_history->setModel(dbw->dbmd_history);
    ui->tview_db_history->setAlternatingRowColors(true);
    ui->tview_db_history->horizontalHeader()->setStretchLastSection(true);
    ui->tview_db_history->verticalHeader()->hide();
    ui->tview_db_history->hideColumn(0);
    //ui->tview_db_history->resizeColumnsToContents();

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
    qDebug() << dbw->close();
    delete ui;
}

bool MainWindow::vehicle_process(bool in_out)
{
    if(in_out) //true in
    {
        //open file instead of taking picture
        this->on_pb_opencv_test_clicked();
        if(ui->le_opencv_lp->text().isEmpty())
        {
            this->on_pb_gate_ignore_clicked();
            return false;
        }
        ui->pte_terminal->insertPlainText("Plate Detected:" + ui->le_opencv_lp->text() + "\n\r");
        dbw->update(true, ui->le_opencv_lp->text());
        ui->le_lp->setText(ui->le_opencv_lp->text().right(6));
        this->on_pb_display_lp_clicked();
        QThread::msleep(1500);
        this->on_pb_gate_open_clicked();
        return true;

    }
    else //false out
    {
        //open file instead of taking picture
        this->on_pb_opencv_test_clicked();
        if(ui->le_opencv_lp->text().isEmpty())
        {
            this->on_pb_gate_ignore_clicked();
            return false;
        }
        ui->pte_terminal->insertPlainText("Plate Detected:" + ui->le_opencv_lp->text() + "\n\r");
        dbw->update(false, ui->le_opencv_lp->text());
        ui->le_lp->setText(ui->le_opencv_lp->text().right(6));
        this->on_pb_display_lp_clicked();
        QThread::msleep(1500);
        this->on_pb_gate_open_clicked();
        return true;
    }
    return false;
}


//opencv total
void MainWindow::on_pb_opencv_test_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this,tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp)"));
    if(file_path.isNull())
        return;

    cv::Mat image = cv::imread(file_path.toStdString());

    struct bzh_LPRService_pkg pkg = lpr_image(image);

    if(pkg.isSucceed)
    {
        ui->le_opencv_lp->setText(pkg.text.left(7)); //remove the last letter if it's there since model provided it for plate type
        ui->pbar_confidence->setValue(pkg.confidence);
        ui->lb_pic->setPixmap(pkg.image.scaled(ui->lb_pic->width(),ui->lb_pic->height(),Qt::KeepAspectRatio));
    }
    else
    {
        ui->le_opencv_lp->clear();
        ui->pbar_confidence->setValue(0);
        ui->lb_pic->setText("Error: No plate found!");
    }
}

//serial supeorio
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
        ui->cb_sensor_in_status->setCheckState(Qt::Checked);
        this->isGateOpen = this->vehicle_process(true);
        break;
    case SIG_SENSOR_IN_RELEASED:
        ui->pte_terminal->insertPlainText("SIG_SENSOR_IN_RELEASED received \n\r");
        ui->cb_sensor_in_status->setCheckState(Qt::Unchecked);
        if(this->isGateOpen)
        {
            this->on_pb_gate_close_clicked();
            this->isGateOpen = false;
        }
        break;
    case SIG_SENSOR_OUT_PRESSED:
        ui->pte_terminal->insertPlainText("SIG_SENSOR_OUT_PRESSED received \n\r");
        ui->cb_sensor_out_status->setCheckState(Qt::Checked);
        this->isGateOpen = this->vehicle_process(false);
        break;
    case SIG_SENSOR_OUT_RELEASED:
        ui->pte_terminal->insertPlainText("SIG_SENSOR_OUT_RELEASED received \n\r");
        ui->cb_sensor_out_status->setCheckState(Qt::Unchecked);
        if(this->isGateOpen)
        {
            this->on_pb_gate_close_clicked();
            this->isGateOpen = false;
        }
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
    sp_svc->bzh_SerialPortService_pkg.send_data[6] = 0;
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
    sp_svc->bzh_SerialPortService_pkg.send_data[6] = 0;
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
    sp_svc->bzh_SerialPortService_pkg.send_data[6] = 0;
    emit signal_serialport_svc_write_panding();
}

void MainWindow::on_pb_display_lp_clicked()
{
    if(ui->le_lp->text().length() != LP_LEN)
    {
        QMessageBox::critical(this,"Error","License plate must be 6 characters!");
        return ;
    }

    QString lp = ui->le_lp->text();
    sp_svc->bzh_SerialPortService_pkg.send_data[0] = CMD_DISPLAY_LP;
    sp_svc->bzh_SerialPortService_pkg.send_data[1] = lp.at(0).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[2] = lp.at(1).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[3] = lp.at(2).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[4] = lp.at(3).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[5] = lp.at(4).toLatin1();
    sp_svc->bzh_SerialPortService_pkg.send_data[6] = lp.at(5).toLatin1();
    emit signal_serialport_svc_write_panding();
}

void MainWindow::on_comboBox_serial_ports_currentIndexChanged(int index)
{
    if(ui->comboBox_serial_ports->currentIndex()!=-1)
    {
        emit signal_serialport_svc_port_changed(sp_svc->bzh_SerialPortService_pkg.serialports.at(index));
    }
}

void MainWindow::on_pb_a_clicked()
{
    qDebug() << dbw->update(true,"京FK5358");
    ui->tview_db_current->resizeColumnsToContents();
    ui->tview_db_history->resizeColumnsToContents();
}

void MainWindow::on_pb_b_clicked()
{
    qDebug() << dbw->update(false,"京FK5358");
    ui->tview_db_current->resizeColumnsToContents();
    ui->tview_db_history->resizeColumnsToContents();
}

void MainWindow::on_pb_c_clicked()
{
    dbw->clear();
    ui->tview_db_current->hideColumn(3);
    ui->tview_db_history->hideColumn(0);
    ui->tview_db_history->resizeColumnsToContents();
}
