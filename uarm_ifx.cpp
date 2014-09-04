#include "uarm_ifx.h"
#include <math.h>
#include <QDebug>
#include <QThread>

uArm_ifx::uArm_ifx(void){

}

uArm_ifx::~uArm_ifx() {

}

void uArm_ifx::main_proc() {
    qDebug() << __FUNCTION__ << "Current thread is " << QThread::currentThreadId();
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

