/**
 * Copyright (c) 2021 Russell, All rights reserved.
 */

#ifndef H264_SEI_H_
#define H264_SEI_H_

#include <stdint.h>

#include "rtc_base/buffer.h"

#define SEI_UUID_SIZE 16

/**
 * AnnexB NALU syntax:
 * Start sequence(3 or 4 bytes)
 * NALU type header(1 byte)
 * RBSP:
 *   SEI payload type
 *   SEI payload size
 *   SEI payload uuid(16 bytes), if payload type is 0x5
 *   SEI payload content
 * Rbsp trailing bits (0x80)
 */

/**
 * Get SEI data from a NALU of AnnexB format
 *
 * @param data, NALU data with start sequence
 * @param length, Length of data in bytes
 * @param payload_type, On output, the payload type of SEI
 * @param uuid, On output, the uuid if the payload type is 0x5
 * @param destination, On output, the SEI data
 * return 0 on success, otherwise -1
 */
int ParseSeiNalu(const uint8_t* data,
                 size_t length,
                 uint32_t* payload_type,
                 uint8_t uuid[SEI_UUID_SIZE],
                 rtc::Buffer* destination);

/**
 * Generate a NALU with custom SEI data
 *
 * @param data, SEI data
 * @param length, Length of data in bytes
 * @param payload_type, Custom payload type
 * @param uuid, UUID, Write uuid to the NALU if payload type is 0x5
 * @param destination, On output, the NALU data
 * return 0 on success, otherwise -1
 */
int WriteSeiNalu(const uint8_t* data,
                 size_t length,
                 uint32_t payload_type,
                 const uint8_t uuid[SEI_UUID_SIZE],
                 rtc::Buffer* destination);

#endif /* H264_SEI_H_ */
