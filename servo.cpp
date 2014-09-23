#include "servo.h"
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include "var_st_xport.h"
#include <iostream>

void servo::var_cmd_proc(cmd_hdr_t *hdr, void *var_part) {

    //  qDebug() << "Qt -- servo - Got a response ";
    switch (hdr->cmd) {
    case SET_SERVO_POS_RET:
        break;
    case GET_SERVO_POS_RET: {
        get_servo_pos_ret_t cmd_ret;
        cmd_ret.hdr = *hdr;
        cmd_ret.body = *(get_servo_pos_ret_ext_t *)var_part;
        //            qDebug() << "Qt servo is " << cmd_ret.body.servo_id << " position is " << cmd_ret.body.servo_pos;
        current_pos = cmd_ret.body.servo_pos;
    }
        break;
    default:
        break;
    } /* endswitch */
} /* var_cmd_proc */


void servo::base_init(servo_ids in_id) {
    servo_min = 9999;           // Large value to force initial set of minimum value
    servo_max = -9999;          // and ditto for the maximum value
    servo_calibrated = false;   // Till we do calibration don't trust min and max.
    current_pos = 0;
    switch (in_id) {
    case (servo_ids::SERVO_UNINIT):
    case (servo_ids::SERVO_HAND):
    case (servo_ids::SERVO_HAND_ROT):
    case (servo_ids::SERVO_ROT):
    case (servo_ids::SERVO_R):
    case (servo_ids::SERVO_L):
        servo_id = in_id;
        break;
    default:
        servo_id = servo_ids::SERVO_UNINIT;
    } /* endswitch */
}

void servo::send_cmd(QByteArray &cmd) {
    port->write(cmd);
    port->flush();
}

static QByteArray working_response;

void cmd_rsp_handler(cmd_hdr_t *hdr, void *body) {
    working_response = QByteArray((char *)hdr, sizeof(cmd_hdr_t));
    working_response += QByteArray((char *)body, hdr->len);
}

void dump_bytes_to_cout(unsigned char c[], uint16_t ch_len) {
    for (int i = 0; i < ch_len; ++i) {
        cout << c[i];
        if (c[i] == '\n') {
            cout.flush();
        } /* endif */
    }
}

bool servo::get_cmd_response(QByteArray &ret, uint16_t ret_size) {
    var_st_xport<cmd_hdr_t, MAX_CMD> cmd_transporter;
    cmd_transporter.set_var_st_handler(cmd_rsp_handler);
    cmd_transporter.set_var_chars_handler(dump_bytes_to_cout);
    working_response.clear();
    if ((port->bytesAvailable() == 0) && !port->waitForReadyRead(10000)) {
        qDebug() << "Timeout waiting for Ard. response";
        return false;
    } else {
        while ((port->bytesAvailable() > 0) || (port->waitForReadyRead(10) && (ret.size() < ret_size))) {
            char in_c;
            port->getChar(&in_c);
//            qDebug() << "Input byte is " << QString::number(in_c,16);
            cmd_transporter.add_byte(in_c);
        } /* endwhile */
        ret = working_response;
        working_response.clear();
//        qDebug() << "Qt - Got response";
        return true;
    } /* endif */
}

// Calibrate this servo against its limits. This sets min and max.
bool servo::calibrate() {
    // This works by using a servo read position command to the Arduino and a servo set position command.
    // The servo is assumed to be detached (not powered) initially.
    // The code reads the current position reading from the servo, and uses that as a safe base.
    // The servo is commanded to move to a smaller value and the position is read repeatedly until the
    // position number stabilizes.  If the stabile number is not the commanded position, the
    // code will place the server back at the last "safe" position, and will retry a slightly less extreme position.
    // This will be repeated for the maximum value also.
    // Once this process has completed, the servo will be marked as stabilized.

    uint16_t tmp_pos;
    uint16_t now_pos;
    uint16_t safe_pos;
    uint16_t last_good_pos;
    uint16_t delta_pos;
    bool done = false;
    QDateTime end_time;

    safe_pos = get_pos();
    last_good_pos = safe_pos;
    now_pos = safe_pos;

    // Work on min first, take now_pos and make it smaller.

    tmp_pos = now_pos / 2;          // Lets try a binary search to find the min.
    delta_pos = now_pos - tmp_pos;
    while ((!done && (tmp_pos > 0))) {
        set_pos(tmp_pos);
        end_time = QDateTime::currentDateTime().addSecs(2);
        // Now read the position for a couple of seconds to see  if we get there
        while (QDateTime::currentDateTime() < end_time) {
            now_pos = get_pos();
            if (now_pos == tmp_pos) {
                // We've gotten to the requested position
                last_good_pos = now_pos;
                break;
            } /* endif */
        } /* endwhile */
        if (now_pos != tmp_pos) {
            // Failed and timed out. Start by safing the servo
            set_pos(last_good_pos);
            // Now increase the value instead.
            tmp_pos += delta_pos / 2;
            delta_pos = delta_pos / 2;
        } else {
            // Positioning worked, the next loop will go further
            if (tmp_pos == 0) {         // Can't get smaller than 0
                done = true;
            } else {
                delta_pos = tmp_pos - (tmp_pos / 2);
                tmp_pos = tmp_pos / 2;
            } /* endif */
        } /* endif */
    } /* endwhile */

    return servo_calibrated;
}

set_servo_pos_cmd_t servo::build_setpos(const uint16_t new_pos) {
    set_servo_pos_cmd_t set_p = { { 0xFEFF, 0xee, SET_SERVO_POS, sizeof(set_p)-sizeof(set_p.hdr)}, { static_cast<int>(servo_id), new_pos } };
    return set_p;
}

get_servo_pos_cmd_t servo::build_getpos(void) {
    if (static_cast<int>(servo_id) > 16) {
        abort();
    }
    get_servo_pos_cmd_t get_p = { { 0xFEFF, 0xee, GET_SERVO_POS, sizeof(get_p)-sizeof(get_p.hdr)}, { static_cast<int>(servo_id) } };
    return get_p;
}

attach_servo_cmd_t servo::build_attach_servo(void) {
    attach_servo_cmd_t att_s = { { 0xFEFF, 0xee, ATTACH_SERVO, sizeof(att_s)-sizeof(att_s.hdr)}, { static_cast<int>(servo_id) } };
    return att_s;
}

detach_servo_cmd_t servo::build_detach_servo(void) {
    detach_servo_cmd_t det_s = { { 0xFEFF, 0xee, DETACH_SERVO, sizeof(det_s)-sizeof(det_s.hdr)}, { static_cast<int>(servo_id) } };
    return det_s;
}

servo::servo(QObject *parent, servo_ids in_id, QSerialPort *comm_port) : QObject(parent) {
    base_init(in_id);
    port = comm_port;
}

bool servo::set_pos(const uint16_t new_pos) {
    emit sync_set_pos(new_pos);
    return true;
}

uint16_t servo::get_pos(void) {
//    qDebug() << __FUNCTION__ << " Current thread is " << QThread::currentThreadId();
//    qDebug() << "Qt -- servo - sending get_pos to Ard.";
    emit sync_get_pos();
    return current_pos;
}

void servo::attach_servo() {
    emit sync_attach_servo();
}

void servo::detach_servo() {
    emit sync_detach_servo();
}

void servo::sync_set_pos(const uint16_t new_pos) {
    //        qDebug() << "Qt -- servo - sending set_pos to Ard.";
    set_servo_pos_cmd_t cmd = build_setpos(new_pos);
    set_servo_pos_ret_t ret_response;
    QByteArray cmd_bytes = QByteArray((const char *)&cmd, sizeof(cmd));
    QByteArray ret_bytes;
    send_cmd(cmd_bytes);
    if (!get_cmd_response(ret_bytes, sizeof(ret_response))) {
        qDebug() << "Qt -- servo - set_pos to Ard failed.";
    } else {
        ret_response = *reinterpret_cast<const set_servo_pos_ret_t *>(ret_bytes.constData());
        current_pos = ret_response.body.servo_pos;
    } /* endif */
}

void servo::sync_get_pos() {
    //        qDebug() << "Qt -- servo - sending get_pos to Ard. ";
    get_servo_pos_cmd_t cmd = build_getpos();
    QByteArray cmd_bytes = QByteArray((const char *)&cmd, sizeof(cmd));
    get_servo_pos_ret_t ret_response;
    QByteArray ret_bytes;
    send_cmd(cmd_bytes);
    if (!get_cmd_response(ret_bytes, sizeof(ret_response))) {
        qDebug() << "Qt -- servo - get_pos from Ard failed.";
    } else {
        ret_response = *reinterpret_cast<const get_servo_pos_ret_t *>(ret_bytes.constData());
        current_pos = ret_response.body.servo_pos;
    } /* endif */
}

void servo::sync_attach_servo() {
    //        qDebug() << "Qt -- servo - sending attach servo to Ard. ";
    attach_servo_cmd_t cmd = build_attach_servo();
    QByteArray cmd_bytes = QByteArray((const char *)&cmd, sizeof(cmd));
    send_cmd(cmd_bytes);
}

void servo::sync_detach_servo() {
    //        qDebug() << "Qt -- servo - sending detach servo to Ard. ";
    detach_servo_cmd_t cmd = build_detach_servo();
    QByteArray cmd_bytes = QByteArray((const char *)&cmd, sizeof(cmd));
    send_cmd(cmd_bytes);
}

