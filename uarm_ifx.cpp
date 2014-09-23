#include "uarm_ifx.h"
#include <math.h>
#include <QDebug>
#include <QThread>
#include <iostream>

using namespace std;

uArm_ifx::uArm_ifx(void){

}

uArm_ifx::~uArm_ifx() {

    port->close();

    for (servo_itr sv=servos.begin(); sv !=servos.end(); ++sv) {
        sv.value()->detach_servo();
    } /* endfor */
}

void uArm_ifx::main_proc() {
    qDebug() << __FUNCTION__ << "Current thread is " << QThread::currentThreadId();

    qRegisterMetaType<enum servo_ids>("servo_ids");
    qRegisterMetaType<uint16_t>("uint16_t");

    port = new QSerialPort("/dev/ttyUSB0", 0);
    if (!port->open(QIODevice::ReadWrite)) {
        abort();
    } /* endif */
    if (!port->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections) ||
        !port->setDataBits(QSerialPort::Data8) ||
        !port->setStopBits(QSerialPort::OneStop) ||
        !port->setFlowControl(QSerialPort::NoFlowControl) ||
        !port->setParity(QSerialPort::NoParity) ||
        !port->setDataTerminalReady(true)) {
        qFatal("Unable to configure serial port /dev/ttyUSB0");
        exit(1);
    } else {
        /* Drain the serial port of all inbound bytes */
        port->waitForReadyRead(1000);
        char c;
        while (port->bytesAvailable() > 0) {
            port->getChar(&c);
            cout << c;
        } /* endwhile */
        port->clear();
    } /* endif */

    servos[servo_ids::SERVO_R] = new servo(this, servo_ids::SERVO_R, port);
    servos[servo_ids::SERVO_L] = new servo(this, servo_ids::SERVO_L, port);
    servos[servo_ids::SERVO_ROT] = new servo(this, servo_ids::SERVO_ROT, port);
    servos[servo_ids::SERVO_HAND] = new servo(this, servo_ids::SERVO_HAND, port);
    servos[servo_ids::SERVO_HAND_ROT] = new servo(this, servo_ids::SERVO_HAND_ROT, port);

    status_timer = new QTimer;
    connect(status_timer, SIGNAL(timeout()), this, SLOT(request_update_position()));
    status_timer->start(500);

} /* main_proc */

void uArm_ifx::get_position(double &r, double &theta, double &rho) {

    r = 25;
    theta = M_PI/2.0;
    rho = M_PI/2.0;
    qDebug() << QThread::currentThreadId() << "< threadid " << __FUNCTION__ << "-- r, theta, rho = " << r << ", " << theta << ", " << rho;
} /* get_position */

void uArm_ifx::set_position(double r, double theta, double rho) {
    qDebug() << QThread::currentThreadId() << "< threadid " << __FUNCTION__ << "-- r, theta, rho = " << r << ", " << theta << ", " << rho;
} /* set_position */

void uArm_ifx::disable_motors() {
    qDebug()<< QThread::currentThreadId() << "< threadid " << __FUNCTION__;
} /* disable_motors */

void uArm_ifx::enable_motors() {
    qDebug()<< QThread::currentThreadId() << "< threadid " << __FUNCTION__;
} /* enable_motors */

void uArm_ifx::disable_servo(servo_ids sv) {
    qDebug()<< QThread::currentThreadId() << "< threadid " << __FUNCTION__;
    servos[sv]->detach_servo();
} /* disable_servo */

void uArm_ifx::enable_servo(servo_ids sv) {
    qDebug()<< QThread::currentThreadId() << "< threadid " << __FUNCTION__;
    servos[sv]->attach_servo();
} /* enable_servo */

void uArm_ifx::request_update_position() {
    uint16_t servo_pos;
    servo_ids servo_reporting;
    for (servo_itr sv=servos.begin(); sv !=servos.end(); ++sv) {
        servo_pos = sv.value()->get_pos();
        servo_reporting = sv.value()->get_servo_id();
//        qDebug() << "Qt:" << QThread::currentThreadId() << "< threadid " << __FUNCTION__ << " sv = " << static_cast<int>(servo_reporting) << " position = " << servo_pos;
        emit servo_pos_changed(servo_reporting, servo_pos);
    } /* endfor */
} /* request_update_position */


