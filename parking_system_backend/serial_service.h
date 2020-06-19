#ifndef SERIAL_SERVICE_H
#define SERIAL_SERVICE_H

#include <QThread>
#include <QtSerialPort>
#include <QList>
#include <QDebug>

//TODO: MOVE SERIALPORT OBJ TO THREAD

class SerialPortService : public QThread
{
    Q_OBJECT

public slots:
    void slot_serialport_svc_port_changed(QSerialPortInfo info)
    {
        bzh_SerialPortService_pkg.serial_port_p->setPort(info);
    }
    void slot_serialport_svc_port_connect()
    {
        bzh_SerialPortService_pkg.serial_port_p->open(QIODevice::ReadWrite);
        bzh_SerialPortService_pkg.port_opened = true;

        //hard reset Arduino
        bzh_SerialPortService_pkg.serial_port_p->setDataTerminalReady(true);
        this->msleep(100);
        bzh_SerialPortService_pkg.serial_port_p->setDataTerminalReady(false);

    }
    void slot_serialport_svc_port_disconnect()
    {
        bzh_SerialPortService_pkg.serial_port_p->close();
        bzh_SerialPortService_pkg.port_opened = false;
    }
    void slot_serialport_svc_write_panding()
    {
        bzh_SerialPortService_pkg.serial_port_p->write(bzh_SerialPortService_pkg.send_data,7);
        bzh_SerialPortService_pkg.write_panding = true;
    }

signals:
    void signal_serialport_svc_init_ready();
    void signal_serialport_svc_data_ready();

public:

    struct bzh_SerialPortService_pkg
    {
        bool loop;
        unsigned int override_delay;
        qint32 baudrate;
        QSerialPort::StopBits stopbits;
        QList<QSerialPortInfo> serialports;
        bool port_opened;
        QSerialPort * serial_port_p;
        uint8_t recv_data[1];
        char send_data[7];
        bool write_panding;
    }
    bzh_SerialPortService_pkg;

    void bzh_SerialPortService_available_port_refresh()
    {
        bzh_SerialPortService_pkg.serialports = QSerialPortInfo::availablePorts();
    }

    void bzh_SerialPortService_pkg_init()
    {
        //value init
        bzh_SerialPortService_pkg.loop = false;
        bzh_SerialPortService_pkg.override_delay = 100;
        bzh_SerialPortService_pkg.baudrate = QSerialPort::Baud115200;
        bzh_SerialPortService_pkg.stopbits = QSerialPort::OneStop;
        bzh_SerialPortService_pkg.port_opened = false;
        bzh_SerialPortService_pkg.write_panding = false;

        //serial port init
        bzh_SerialPortService_pkg.serial_port_p = new QSerialPort;
        bzh_SerialPortService_pkg.serial_port_p->setBaudRate(bzh_SerialPortService_pkg.baudrate);
        bzh_SerialPortService_pkg.serial_port_p->setStopBits(bzh_SerialPortService_pkg.stopbits);
        bzh_SerialPortService_pkg.serial_port_p->setParity(QSerialPort::NoParity);
    }

    void run()
    {
        qDebug() << "SerialPortService Start" << this->currentThreadId();
        emit signal_serialport_svc_init_ready();
        while(bzh_SerialPortService_pkg.loop)
        {
            //delay
            this->msleep(bzh_SerialPortService_pkg.override_delay);

            if(bzh_SerialPortService_pkg.port_opened)
            {
                //loop code here
                if(bzh_SerialPortService_pkg.write_panding)
                {

                    //bzh_SerialPortService_pkg.serial_port_p->write(bzh_SerialPortService_pkg.send_data,6);
                    qDebug() << "sending";
                    bzh_SerialPortService_pkg.write_panding = false;
                }

//                if(bzh_SerialPortService_pkg.serial_port_p->bytesAvailable() > 0)
//                {
//                    bzh_SerialPortService_pkg.serial_port_p->read((char*)bzh_SerialPortService_pkg.recv_data,1);
//                    emit signal_serialport_svc_data_ready();
//                }

                if(bzh_SerialPortService_pkg.serial_port_p->waitForReadyRead(10))
                {
                    bzh_SerialPortService_pkg.serial_port_p->read((char*)bzh_SerialPortService_pkg.recv_data,1);
                    emit signal_serialport_svc_data_ready();
                }
            }
        }
        qDebug() << "SerialPortService Stop" << this->currentThreadId();
    }

    void quit()
    {
        bzh_SerialPortService_pkg.loop = false;
    }


};

#endif // SERIAL_SERVICE_H
