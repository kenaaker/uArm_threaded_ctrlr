#include <QThread>
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

    ui->setupUi(this);
}

uArm_controller::~uArm_controller() {
    delete ui;
}

void uArm_controller::on_actionExit_triggered() {
    QApplication::quit();
}
