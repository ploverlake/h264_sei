/**
 * Copyright (c) 2021 Russell, All rights reserved.
 */

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "h264_sei.h"
#include "rtc_base/buffer.h"

// clang-format off
static const uint8_t TEST_UUID[] = {
    0x84, 0x31, 0x88, 0x8e, 0xc2, 0x46, 0x49, 0x63,
    0x81, 0x1a, 0x68, 0x28, 0xf2, 0x4d, 0xc1, 0x4f
};
// clang-format on

int main(void)
{
    std::vector<uint8_t> sei_data = {0, 0, 1};
    printf("SEI data: ");
    for (int i = 0; i < sei_data.size(); ++i) {
        printf("0x%02X, ", sei_data[i]);
    }
    printf("\n");

    std::unique_ptr<rtc::Buffer> sei_nalu = std::make_unique<rtc::Buffer>();
    WriteSeiNalu((const uint8_t *)(sei_data.data()),
                 sei_data.size(),
                 TEST_UUID,
                 sei_nalu.get());

    const uint8_t *nalu_data = sei_nalu->data();
    const size_t nalu_size = sei_nalu->size();
    printf("NALU size: %lu\n", nalu_size);

    printf("NALU data:\n");
    for (int i = 0; i < nalu_size; ++i) {
        printf("0x%02X, ", nalu_data[i]);
    }
    printf("\n");

    uint32_t payload_type = -1;
    uint8_t uuid[16] = {0};
    std::unique_ptr<rtc::Buffer> buffer = std::make_unique<rtc::Buffer>();
    ParseSeiNalu(nalu_data, nalu_size, &payload_type, uuid, buffer.get());
    printf("SEI data size: %lu\n", buffer->size());
    if (payload_type == 5) {
        printf("UUID: ");
        for (int i = 0; i < 16; ++i) {
            printf("0x%02X, ", uuid[i]);
        }
        printf("\n");
    }

    printf("SEI data: ");
    for (int i = 0; i < buffer->size(); ++i) {
        printf("0x%02X, ", buffer->data()[i]);
    }
    printf("\n");

    return 0;
}
