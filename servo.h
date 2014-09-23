#ifndef SERVO_H
#define SERVO_H

/*====================================================================================*/
/*====================================================================================*/
/*   This class handles a servo motor attached via an Arduino controller.             */
/*                                                                                    */
/*   All positioning is in handled as angular positions (in radians)                  */
/*                                                                                    */
/*   The calibration process will command a single servo to the extremes of its       */
/*   range, monitoring the resulting position to detect stalling and the limits       */
/*   of motion.                                                                       */
/*                                                                                    */
/*   This is not a generalized object yet. It is specific to the uFactory servos.     */
/*                                                                                    */
/*                                                                                    */
/*====================================================================================*/
#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <fixedendian.h>
#include "cmd_types.h"

enum class servo_ids { SERVO_UNINIT=0, SERVO_HAND=9, SERVO_HAND_ROT=10, SERVO_ROT=11, SERVO_R=12, SERVO_L=13 };

class servo : public QObject {
    Q_OBJECT
public:
    explicit servo(QObject *parent = 0, servo_ids in_id = servo_ids::SERVO_UNINIT, QSerialPort *comm_port = NULL);

    bool calibrate();       // Calibrate the servo against its limits, sets min and max
    void attach_servo();    // Request a servo be attached to its control line
    void detach_servo();    // Request a servo be detached from its control line
    bool set_pos(const uint16_t new_pos); // Sets the servo position via the Arduino
    uint16_t get_pos(void); // Gets the current servo position from the Arduino (from wiper, converted to degrees).
    void set_min(const uint16_t new_min) {
        servo_min = new_min;
    }
    void set_max(const uint16_t new_max) {
        servo_max = new_max;
    }
    uint16_t get_min(void) {
        return servo_min;
    }
    uint16_t get_max(void) {
        return servo_max;
    }
    servo_ids get_servo_id(void) {
        return servo_id;
    }
    void var_cmd_proc(cmd_hdr_t *hdr, void *var_part); /* Process command returns */
signals:

public slots:

private:
    bool servo_calibrated;  // Don't trust range until the servo is calibrated.
    servo_ids servo_id;     // The ID number used to identify the servo to the Arduino sketch
    int servo_min;          // Minimum value reported from the servo hardware
    int servo_max;          // Maximum value reported from the servo hardware
    int current_pos;        // Local tracking copy of position
    QSerialPort *port;      // The serial port used to talk to the Arduino sketch
    QTimer arrival_timer;   // Timer that goes off to check that the servo moved properly
    void base_init(servo_ids in_id);

    void send_cmd(QByteArray &cmd);
    bool get_cmd_response(QByteArray &ret, uint16_t ret_size);
    set_servo_pos_cmd_t build_setpos(const uint16_t new_pos);
    get_servo_pos_cmd_t build_getpos(void);
    attach_servo_cmd_t build_attach_servo();
    detach_servo_cmd_t build_detach_servo();
private slots:
    void sync_set_pos(const uint16_t new_pos);
    void sync_get_pos();
    void sync_attach_servo();
    void sync_detach_servo();
};

#endif // SERVO_H
