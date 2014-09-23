#ifndef VAR_ST_XPORT_H
#define VAR_ST_XPORT_H
#include <QDebug>

using namespace std;

/* This templated class will reassemble variable length structures from a byte stream */
template <typename mv_st_hdr, size_t biggest> class var_st_xport {

    typedef void (*struct_handler)(mv_st_hdr *hdr, void *var_part);
    typedef void (*chars_handler)(unsigned char ch[], uint16_t ch_len);
    char var_part[biggest];
    unsigned int bytes_in_var_part;
    mv_st_hdr whole_header;
    unsigned int bytes_in_header;
    unsigned short byte_order_marker;
    unsigned char header_marker;
    struct_handler sh;
    chars_handler chars_h;

    enum accum_state_e { BUF_EMPTY, GOT_BOM_1, GOT_BOM_2,  WAITING_FOR_HDR_COMPLETE, WAITING_FOR_BUFFER_COMPLETE };
    accum_state_e in_accum;

public:

    var_st_xport() {
        in_accum = BUF_EMPTY;
        bytes_in_header = 0;
        bytes_in_var_part = 0;
        sh = (struct_handler)NULL;
        chars_h = (chars_handler)NULL;
        byte_order_marker = 0xfeff;
        header_marker = 0xee;
    }

    void set_var_st_handler(struct_handler in_sh) {
        sh = in_sh;
    }

    void set_var_chars_handler(chars_handler in_chars_h) {
        chars_h = in_chars_h;
    }

    void set_bom(unsigned short in_bom) {
        byte_order_marker = in_bom;
    }

    void set_hdr_marker(chars_handler in_h_marker) {
        header_marker = in_h_marker;
    }

    bool buffer_in_progress() {
        return in_accum != BUF_EMPTY;
    }

    void add_byte(unsigned char b) {
        unsigned char bytes[1];
        bytes[0] = b;
        add_bytes(bytes, 1);
    }

    void add_bytes(unsigned char bytes[], int bytes_length) {
        unsigned int bytes_available = bytes_length;
        unsigned char uc;
        int current_byte;
        static unsigned char bom_byte_1;
        static unsigned char bom_byte_2;

        current_byte = 0;
        while (bytes_available > 0) {
            uc = bytes[current_byte++];
            --bytes_available;
#ifdef BLAH_DEBUG
#ifndef QT_VERSION
            Serial.print("Ard -- add_bytes state="); Serial.println(in_accum); Serial.flush(); delay(500);
#else
            qDebug() << "Qt -- add_bytes state=" << in_accum;
#endif
#endif
            switch (in_accum) {
                case BUF_EMPTY:
                    bom_byte_1 = 0;
                    bom_byte_2 = 0;
                    if ((uc == BOM_BYTE_1_BE) || (uc == BOM_BYTE_1_LE)) {
                        bom_byte_1 = uc;
                        in_accum = GOT_BOM_1;
                    } else {
                        if (chars_h) {
                            chars_h(&uc, sizeof(uc));
                        } /* endif */
                    } /* endif */
                    break;
                case GOT_BOM_1:
                    if ((bom_byte_1 == BOM_BYTE_1_BE) && (uc == BOM_BYTE_2_BE)) {
                        bom_byte_2 = uc;
                        in_accum = GOT_BOM_2;
                    } else if ((bom_byte_1 == BOM_BYTE_1_LE) && (uc == BOM_BYTE_2_LE)) {
                        bom_byte_2 = uc;
                        in_accum = GOT_BOM_2;
                    } else {
                        in_accum = BUF_EMPTY;
                        if (chars_h) {
                            chars_h(&bom_byte_1, sizeof(bom_byte_1));
                            chars_h(&uc, sizeof(uc));
                        } /* endif */
                    } /* endif */
                    break;
                case GOT_BOM_2:
                    if (uc == HEADER_MARKER) {
                        /* Start assuming this is a valid header. Accumulate header */
                        whole_header.endian_ind = bom_byte_1 << 8 | bom_byte_2;
                        whole_header.header_marker = uc;
                        bytes_in_header = 3;
                        in_accum = WAITING_FOR_HDR_COMPLETE;
                    } else {
                        /* Doesn't match the header, dump it all and start over */
                        in_accum = BUF_EMPTY;
                        if (chars_h) {
                            chars_h(&bom_byte_1, sizeof(bom_byte_1));
                            chars_h(&bom_byte_2, sizeof(bom_byte_2));
                            chars_h(&uc, sizeof(uc));
                        } /* endif */
                    } /* endif */
                    break;
                case WAITING_FOR_HDR_COMPLETE:
                    /* Copy into header */
                    ((char *)&whole_header)[bytes_in_header++] = uc;
                    if (bytes_in_header >= sizeof(whole_header)) {
                        bytes_in_header = 0;
                        bytes_in_var_part = 0;
                        whole_header.len = min((unsigned long)whole_header.len, biggest); // Forceably prevent buffer overrun
                        in_accum = WAITING_FOR_BUFFER_COMPLETE;
                    } /* endif */
                    break;
                case WAITING_FOR_BUFFER_COMPLETE:
#ifdef BLAH_DEBUG
#ifndef QT_VERSION
                    Serial.print("Ard -- add_bytes header len="); Serial.println(whole_header.len); Serial.flush(); delay(500);
#else
            qDebug() << "Qt -- add_bytes length in header" << whole_header.len;
#endif
#endif
                    /* Copy bytes into record area */
                    var_part[bytes_in_var_part++] = uc;
                    if (bytes_in_var_part >= whole_header.len) {
#ifdef BLAH_DEBUG
#ifndef QT_VERSION
                        Serial.println("Ard -- received complete record calling "); Serial.flush(); delay(500);
#else
            qDebug() << "Qt -- add_bytes record complete calling";
#endif
#endif
                        if (sh) {
                            sh(&whole_header, var_part);
                        } /* endif */
                        bytes_in_header = 0;
                        bytes_in_var_part = 0;
                        in_accum = BUF_EMPTY;
                    } /* endif */
                    break;
                default:
                    in_accum = BUF_EMPTY;
                    break;
            } /* endswitch */
        } /* endwhile */
    } /* add_bytes */
};

template<class T>
const T& constrain(const T& x, const T& a, const T& b) {
    if(x < a) {
        return a;
    }
    else if(b < x) {
        return b;
    }
    else
        return x;
}

#endif // VAR_ST_XPORT_H
