/**
 * Copyright (c) 2021 Russell, All rights reserved.
 */

#include "h264_sei.h"

#include <vector>

#include "common_video/h264/h264_common.h"

static const char kAnnexBThreeBytesHeader[3] = {0x00, 0x00, 0x01};
static const char kAnnexBFourBytesHeader[4] = {0x00, 0x00, 0x00, 0x01};
static const uint8_t kNaluTypeSei = 0x06u;
static const uint8_t kSeiPayloadTypeUnregistered = 0x05u;
static const uint8_t kRbspTrailingBits = 0x80u;
static const uint8_t kNaluTypeMask = 0x1F;

int ParseSeiNalu(const uint8_t *data,
                 size_t length,
                 uint32_t *payload_type,
                 uint8_t uuid[SEI_UUID_SIZE],
                 rtc::Buffer *destination)
{
    if (data == nullptr || length == 0 || payload_type == nullptr ||
        uuid == nullptr || destination == nullptr) {
        return -1;
    }

    size_t payload_start_offset = 0;
    if (length > 4 && memcmp(data, kAnnexBFourBytesHeader, 4) == 0) {
        payload_start_offset = 4;
    } else if (length > 3 && memcmp(data, kAnnexBThreeBytesHeader, 3) == 0) {
        payload_start_offset = 3;
    } else {
        return -1;
    }
    data += payload_start_offset;
    length -= payload_start_offset;

    if ((*data & kNaluTypeMask) != kNaluTypeSei) {
        return -1;
    }
    ++data;
    --length;

    uint32_t sei_payload_type = 0;
    do {
        sei_payload_type += *data;
        --length;
    } while (*data++ == 0xFF);

    uint32_t sei_payload_size = 0;
    do {
        sei_payload_size += *data;
        --length;
    } while (*data++ == 0xFF);

    if (sei_payload_size > length)
        return -1;

    if (sei_payload_type == kSeiPayloadTypeUnregistered) {
        if (sei_payload_size < SEI_UUID_SIZE) {
            return -1;
        }
        memcpy(uuid, data, SEI_UUID_SIZE);
        data += SEI_UUID_SIZE;
        length -= SEI_UUID_SIZE;
        sei_payload_size -= SEI_UUID_SIZE;
    }

    *payload_type = sei_payload_type;

    std::vector<uint8_t> sei_payload_data =
        webrtc::H264::ParseRbsp(data, sei_payload_size);
    destination->SetData(sei_payload_data.data(), sei_payload_data.size());

    return 0;
}

int WriteSeiNalu(const uint8_t *data,
                 size_t length,
                 const uint8_t uuid[SEI_UUID_SIZE],
                 rtc::Buffer *destination)
{
    if (data == nullptr || length == 0 || uuid == nullptr ||
        destination == nullptr) {
        return -1;
    }

    std::unique_ptr<rtc::Buffer> sei_rbsp_buffer =
        std::make_unique<rtc::Buffer>();
    webrtc::H264::WriteRbsp(data, length, sei_rbsp_buffer.get());

    destination->SetSize(0);

    destination->AppendData(kAnnexBFourBytesHeader,
                            sizeof(kAnnexBFourBytesHeader));
    destination->AppendData(kNaluTypeSei);
    destination->AppendData(kSeiPayloadTypeUnregistered);

    uint32_t sei_payload_size = SEI_UUID_SIZE + sei_rbsp_buffer->size();
    while (sei_payload_size >= 0xFF) {
        destination->AppendData(static_cast<uint8_t>(0xFF));
        sei_payload_size -= 0xFF;
    }
    destination->AppendData(static_cast<uint8_t>(sei_payload_size));

    destination->AppendData(uuid, SEI_UUID_SIZE);
    destination->AppendData(sei_rbsp_buffer->data(), sei_rbsp_buffer->size());
    destination->AppendData(kRbspTrailingBits);

    return 0;
}
