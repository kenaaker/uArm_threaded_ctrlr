#ifndef UARM_IFX_H
#define UARM_IFX_H

#include <QObject>
#include <QMap>
#include "servo.h"
#include "cmd_types.h"
#include "var_st_xport.h"

class uArm_ifx : public QObject {
    Q_OBJECT

public:
    uArm_ifx(void);
    ~uArm_ifx();

public slots:
    void main_proc();
    void get_position(double &r, double &theta, double &rho);
    void set_position(double r, double theta, double rho);
    void disable_motors();
    void enable_motors();
    void disable_servo(servo_ids sv);
    void enable_servo(servo_ids sv);
    void request_update_position();

signals:
    void uarm_ready();
    void uarm_arrived(double r, double theta, double rho);
    void uarm_stalled(double r, double theta, double rho);
    void servo_pos_changed(servo_ids sv, uint16_t sv_pos);

private:
    QMap<servo_ids, servo *> servos;
    typedef QMap<servo_ids, servo *>::iterator servo_itr;
    QSerialPort *port;
    QTimer *status_timer;

};

#endif // UARM_IFX_H
