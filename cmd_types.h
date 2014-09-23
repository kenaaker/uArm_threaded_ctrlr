#ifndef CMD_TYPES_H
#define CMD_TYPES_H

static const int MAX_CMD = 64;
static const uint16_t BOM = 0xFEFF;

static const uint8_t BOM_BYTE_1_BE = 0xFE;
static const uint8_t BOM_BYTE_2_BE = 0xFF;
static const uint8_t BOM_BYTE_1_LE = 0xFF;
static const uint8_t BOM_BYTE_2_LE = 0xFE;

static const uint8_t HEADER_MARKER = 0xEE;

#define SET_SERVO_POS 0xab
#define SET_SERVO_POS_RET 0xac
#define GET_SERVO_POS 0xbc
#define GET_SERVO_POS_RET 0xbd

#define DETACH_SERVO 0xad
#define DETACH_SERVO_RET 0xae
#define ATTACH_SERVO 0xa9
#define ATTACH_SERVO_RET 0xaa

#pragma pack(1)
    typedef struct cmd_hdr_s {
        BigEndian<uint16_t> endian_ind;     /* Endian indicator word, FEFF == big, FFEF == little */
        uint8_t header_marker;              /* 0xee */
        uint8_t cmd;                        /* Command identifier. */
        BigEndian<uint16_t> len;            /* Length of following cmd */
    } cmd_hdr_t;

    typedef struct set_servo_pos_ext {
        BigEndian<uint16_t> servo_id;       /* servo id to position */
        BigEndian<uint16_t> servo_pos;       /* servo position to set */
    } set_servo_pos_ext_t;

    typedef struct set_servo_pos_cmd {
        cmd_hdr_t hdr;                      /* header_1 = 0xee header = 0xab */
        set_servo_pos_ext_t body;           /* specific part of set pos cmd */
    } set_servo_pos_cmd_t;

    typedef struct set_servo_pos_ret_ext {
        BigEndian<uint16_t> servo_id;       /* servo id of set position */
        BigEndian<uint16_t> servo_pos;      /* servo position set */
    } set_servo_pos_ret_ext_t;

    typedef struct set_servo_pos_ret {
        cmd_hdr_t hdr;                      /* header_1 = 0xee header = 0xac */
        set_servo_pos_ret_ext_t body;       /* specific part of set pos cmd */
    } set_servo_pos_ret_t;

    // The servo get position used here uses the analog value from the servo
    // sweep component, not the servo PWM microseconds value or the servo degrees value.
    // This must be calibrated against the servo positioning before it can be useful.
    // This raw value can be used for calibration or stall detection.
    typedef struct get_servo_pos_ext {
        BigEndian<uint16_t> servo_id;       /* servo id to retrieve position for */
    } get_servo_pos_ext_t;

    typedef struct get_servo_pos_cmd {
        cmd_hdr_t hdr;                      /* header_1 = 0xee header = 0xbc */
        get_servo_pos_ext_t body;           /* specific part of get pos cmd */
    } get_servo_pos_cmd_t;

    typedef struct get_servo_pos_ret_ext {
        BigEndian<uint16_t> servo_id;       /* servo id returning position */
        BigEndian<uint16_t> servo_pos;      /* servo position */
    } get_servo_pos_ret_ext_t;

    typedef struct get_servo_pos_ret {
        cmd_hdr_t hdr;                      /* header_1 = 0xee header = 0xbd */
        get_servo_pos_ret_ext_t body;       /* specific part of get pos cmd */
    } get_servo_pos_ret_t;

    // These commands are used to connect and disconnect the servo motors to protect them
    typedef struct detach_servo_ext {
        BigEndian<uint16_t> servo_id;       /* servo id to detach */
    } detach_servo_ext_t;

    typedef struct detach_servo_cmd {
        cmd_hdr_t hdr;                      /* header_1 = 0xee header = 0xad */
        detach_servo_ext_t body;            /* specific detach servo cmd */
    } detach_servo_cmd_t;

    typedef struct detach_servo_ret_ext {
        BigEndian<uint16_t> servo_id;       /* servo id of detached servo */
    } detach_servo_ret_ext_t;

    typedef struct detach_servo_ret {
        cmd_hdr_t hdr;                      /* header_1 = 0xee header = 0xac */
        detach_servo_ret_ext_t body;        /* specific part of detach servo cmd reply */
    } detach_servo_ret_t;

    typedef struct attach_servo_ext {
        BigEndian<uint16_t> servo_id;       /* servo id to attach */
    } attach_servo_ext_t;

    typedef struct attach_servo_cmd {
        cmd_hdr_t hdr;                      /* header_1 = 0xee header = 0xa9 */
        attach_servo_ext_t body;            /* specific detach servo cmd */
    } attach_servo_cmd_t;

    typedef struct attach_servo_ret_ext {
        BigEndian<uint16_t> servo_id;       /* servo id of attached servo */
    } attach_servo_ret_ext_t;

    typedef struct attach_servo_ret {
        cmd_hdr_t hdr;                      /* header_1 = 0xee header = 0xa9 */
        attach_servo_ret_ext_t body;        /* specific part of attach servo cmd */
    } attach_servo_ret_t;

#pragma pack()

#endif // CMD_TYPES_H
