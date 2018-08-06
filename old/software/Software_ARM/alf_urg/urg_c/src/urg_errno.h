#ifndef URG_ERRNO_H
#define URG_ERRNO_H

/*!
 * @file
*/

enum {
    URG_NO_ERROR = 0,
    URG_UNKNOWN_ERROR = -1,
    URG_NOT_CONNECTED = -2,
    URG_NOT_IMPLEMENTED = -3,
    URG_INVALID_RESPONSE = -4,
    URG_NO_RESPONSE = -5,

    URG_SEND_ERROR = -6,
    URG_RECEIVE_ERROR = -7,
    URG_CHECKSUM_ERROR = -8,
    URG_INVALID_PARAMETER = -9,
    URG_MEASUREMENT_TYPE_MISMATCH = -10,
    URG_COMMON_ERROR_LAST,

    URG_SERIAL_OPEN_ERROR = (URG_COMMON_ERROR_LAST -1) -1,
    URG_NOT_DETECT_BAUDRATE_ERROR = (URG_COMMON_ERROR_LAST -1) -2,
    URG_ETHERNET_OPEN_ERROR = (URG_COMMON_ERROR_LAST -1) -3,
    URG_SCANNING_PARAMETER_ERROR = (URG_COMMON_ERROR_LAST -1) -4,
    URG_DATA_SIZE_PARAMETER_ERROR = (URG_COMMON_ERROR_LAST -1) -5,
};

#endif /* !URG_ERRNO_H */
