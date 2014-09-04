#ifndef UARM_CONTROLLER_H
#define UARM_CONTROLLER_H

#include <QMainWindow>
#include "uarm_ifx.h"

namespace Ui {
class uArm_controller;
}

class uArm_controller : public QMainWindow
{
    Q_OBJECT

public:
    explicit uArm_controller(QWidget *parent = 0);
    ~uArm_controller();

private slots:
    void on_actionExit_triggered();

private:
    Ui::uArm_controller *ui;
    QThread *worker_thread;
    uArm_ifx *worker_obj;
};

#endif // UARM_CONTROLLER_H
