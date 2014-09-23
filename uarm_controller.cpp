#include <QThread>
#include <QString>
#include <QDebug>
#include "uarm_controller.h"
#include "ui_uarm_controller.h"

uArm_controller::uArm_controller(QWidget *parent) : QMainWindow(parent),
                                ui(new Ui::uArm_controller) {
    worker_thread = new QThread;
    worker_obj = new uArm_ifx();

    qDebug() << __FUNCTION__ << " Current thread is " << QThread::currentThreadId();

    worker_obj->moveToThread(worker_thread);
    connect(worker_thread, SIGNAL(started()), worker_obj, SLOT(main_proc()));
    connect(worker_obj, SIGNAL(destroyed()), worker_thread, SLOT(quit()));
    worker_thread->start();
    connect(worker_obj, SIGNAL(servo_pos_changed(servo_ids, uint16_t)),
            this, SLOT(update_position_display(servo_ids, uint16_t)));

    ui->setupUi(this);
}

uArm_controller::~uArm_controller() {
    delete ui;
}

void uArm_controller::on_actionExit_triggered() {
    QApplication::quit();
}

void uArm_controller::update_position_display(servo_ids sv, uint16_t sv_pos) {

//    qDebug() << "Qt:" << QThread::currentThreadId() << "< threadid " << __FUNCTION__ << " sv = " << static_cast<int>(sv) << " position = " << sv_pos;

    update_servo_minmax(sv, sv_pos);
    QString sv_label_name = "sensor_value_";
    sv_label_name += QString::number(static_cast<int>(sv));
    QLabel *sv_label = ui->centralwidget->findChild<QLabel *>(sv_label_name);
    sv_label->setText(QString::number(sv_pos));
}

void uArm_controller::update_servo_minmax(servo_ids sv, uint16_t sv_pos) {

}

void uArm_controller::on_servo_9_valueChanged(int value) {

}

void uArm_controller::on_connect_9_clicked(bool checked) {
    if (checked) {
        worker_obj->enable_servo(servo_ids::SERVO_HAND);
    } else {
        worker_obj->disable_servo(servo_ids::SERVO_HAND);
    }
}
