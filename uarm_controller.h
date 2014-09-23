#ifndef UARM_CONTROLLER_H
#define UARM_CONTROLLER_H

#include <QMainWindow>
#include <QGroupBox>
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

    void on_servo_9_valueChanged(int value);

    void update_position_display(servo_ids sv, uint16_t sv_pos);

    void on_connect_9_clicked(bool checked);

private:
    Ui::uArm_controller *ui;
    QThread *worker_thread;
    uArm_ifx *worker_obj;
    void update_servo_minmax(servo_ids sv, uint16_t sv_pos);
};

#endif // UARM_CONTROLLER_H
