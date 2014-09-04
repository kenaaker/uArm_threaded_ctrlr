#ifndef UARM_IFX_H
#define UARM_IFX_H

#include <QObject>

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

signals:
    void uarm_ready();
    void uarm_arrived(double r, double theta, double rho);
    void uarm_stalled(double r, double theta, double rho);

private:

};

#endif // UARM_IFX_H
