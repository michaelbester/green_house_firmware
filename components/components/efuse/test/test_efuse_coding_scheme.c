/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <strings.h>
#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "unity.h"
#include "bootloader_random.h"

#ifdef CONFIG_IDF_TARGET_ESP32
typedef struct {
    uint8_t unencoded[24];
    uint32_t encoded[8];
} coding_scheme_test_t;

/* Randomly generated byte strings, encoded and written to ESP32
   using espefuse algorithm, then verified to have no encoding errors
   and correct readback.
*/
static const coding_scheme_test_t coding_scheme_data[] = {
    {
        .unencoded = { 0x96, 0xa9, 0xab, 0xb2, 0xda, 0xdd, 0x21, 0xd2, 0x35, 0x22, 0xd3, 0x30, 0x3b, 0xf8, 0xcb, 0x77, 0x8d, 0x8d, 0xf4, 0x96, 0x25, 0xc4, 0xb9, 0x94 },
        .encoded = { 0xb2aba996, 0x6821ddda, 0x2235d221, 0x430730d3, 0x77cbf83b, 0x627f8d8d, 0xc42596f4, 0x4dae94b9 },
    },
    {
        .unencoded = { 0x0e, 0x6b, 0x1a, 0x1d, 0xa5, 0x9f, 0x24, 0xcf, 0x91, 0x5b, 0xe7, 0xe1, 0x7c, 0x0a, 0x6e, 0xdc, 0x5e, 0x8e, 0xb1, 0xec, 0xd1, 0xf3, 0x75, 0x48 },
        .encoded = { 0x1d1a6b0e, 0x5e589fa5, 0x5b91cf24, 0x6127e1e7, 0xdc6e0a7c, 0x5d148e5e, 0xf3d1ecb1, 0x57424875 },
    },
    {
        .unencoded = { 0x0a, 0x79, 0x5a, 0x1c, 0xb1, 0x45, 0x71, 0x2c, 0xb3, 0xda, 0x9e, 0xdc, 0x76, 0x27, 0xf5, 0xca, 0xe7, 0x00, 0x39, 0x95, 0x6c, 0x53, 0xc2, 0x07 },
        .encoded = { 0x1c5a790a, 0x4ac145b1, 0xdab32c71, 0x6476dc9e, 0xcaf52776, 0x4d8900e7, 0x536c9539, 0x495607c2 },
    },
    {
        .unencoded = { 0x76, 0x46, 0x88, 0x2d, 0x4c, 0xe1, 0x50, 0x5d, 0xd6, 0x7c, 0x41, 0x15, 0xc6, 0x1f, 0xd4, 0x60, 0x10, 0x15, 0x2a, 0x72, 0x2d, 0x89, 0x93, 0x13 },
        .encoded = { 0x2d884676, 0x4838e14c, 0x7cd65d50, 0x4bf31541, 0x60d41fc6, 0x39681510, 0x892d722a, 0x497c1393 },
    },
    {
        .unencoded = { 0x32, 0xbc, 0x40, 0x92, 0x13, 0x37, 0x1a, 0xae, 0xb6, 0x00, 0xed, 0x30, 0xb8, 0x82, 0xee, 0xfc, 0xcf, 0x6d, 0x7f, 0xc5, 0xfa, 0x0e, 0xdd, 0x84 },
        .encoded = { 0x9240bc32, 0x49783713, 0x00b6ae1a, 0x46df30ed, 0xfcee82b8, 0x6e8a6dcf, 0x0efac57f, 0x571784dd },
    },
    {
        .unencoded = { 0x29, 0xb3, 0x04, 0x95, 0xf2, 0x3c, 0x81, 0xe6, 0x5a, 0xf3, 0x42, 0x82, 0xd1, 0x79, 0xe2, 0x12, 0xbe, 0xc3, 0xd4, 0x10, 0x63, 0x66, 0x9f, 0xe3 },
        .encoded = { 0x9504b329, 0x51c53cf2, 0xf35ae681, 0x460e8242, 0x12e279d1, 0x5825c3be, 0x666310d4, 0x5ebde39f },
    },
    {
        .unencoded = { 0xda, 0xda, 0x71, 0x4a, 0x62, 0x33, 0xdd, 0x31, 0x87, 0xf3, 0x70, 0x12, 0x33, 0x3b, 0x3b, 0xe9, 0xed, 0xc4, 0x6e, 0x6a, 0xc7, 0xd5, 0x85, 0xfc },
        .encoded = { 0x4a71dada, 0x4e6a3362, 0xf38731dd, 0x4bfa1270, 0xe93b3b33, 0x61f3c4ed, 0xd5c76a6e, 0x636ffc85 },
    },
    {
        .unencoded = { 0x45, 0x64, 0x51, 0x34, 0x1c, 0x82, 0x81, 0x77, 0xf8, 0x89, 0xb1, 0x15, 0x82, 0x94, 0xdd, 0x64, 0xa2, 0x46, 0x0e, 0xfb, 0x1a, 0x70, 0x4b, 0x9f },
        .encoded = { 0x34516445, 0x39da821c, 0x89f87781, 0x4f2315b1, 0x64dd9482, 0x474b46a2, 0x701afb0e, 0x5e4b9f4b },
    },
    {
        .unencoded = { 0x89, 0x87, 0x15, 0xb6, 0x66, 0x34, 0x49, 0x18, 0x8b, 0x7b, 0xb2, 0xf6, 0x96, 0x1e, 0x2e, 0xf1, 0x03, 0x9d, 0x4e, 0x16, 0x32, 0xd6, 0x23, 0x22 },
        .encoded = { 0xb6158789, 0x4eff3466, 0x7b8b1849, 0x63e5f6b2, 0xf12e1e96, 0x54c99d03, 0xd632164e, 0x42bd2223 },
    },
    {
        .unencoded = { 0xa7, 0xa0, 0xb5, 0x21, 0xd2, 0xa3, 0x9f, 0x65, 0xa9, 0xeb, 0x72, 0xa2, 0x2e, 0xa6, 0xfb, 0x9c, 0x48, 0x7e, 0x68, 0x08, 0x7a, 0xb1, 0x4f, 0xbc },
        .encoded = { 0x21b5a0a7, 0x4ce2a3d2, 0xeba9659f, 0x5868a272, 0x9cfba62e, 0x5fd97e48, 0xb17a0868, 0x5b58bc4f },
    },
    {
        .unencoded = { 0xf7, 0x05, 0xe3, 0x6c, 0xb1, 0x55, 0xcb, 0x2f, 0x8d, 0x3e, 0x0b, 0x2e, 0x3e, 0xb7, 0x02, 0xf5, 0x91, 0xb1, 0xfe, 0x8b, 0x58, 0x50, 0xb2, 0x40 },
        .encoded = { 0x6ce305f7, 0x569955b1, 0x3e8d2fcb, 0x56722e0b, 0xf502b73e, 0x535eb191, 0x50588bfe, 0x3a8f40b2 },
    },
    {
        .unencoded = { 0x0f, 0x93, 0xb0, 0xd5, 0x60, 0xba, 0x40, 0x2a, 0x62, 0xa6, 0x92, 0x82, 0xb8, 0x91, 0x2c, 0xd7, 0x23, 0xdc, 0x6f, 0x7f, 0x2f, 0xbe, 0x41, 0xf5 },
        .encoded = { 0xd5b0930f, 0x5123ba60, 0xa6622a40, 0x3bbe8292, 0xd72c91b8, 0x582ddc23, 0xbe2f7f6f, 0x6935f541 },
    },
    {
        .unencoded = { 0x7f, 0x0c, 0x99, 0xde, 0xff, 0x2e, 0xd2, 0x1c, 0x48, 0x98, 0x70, 0x85, 0x15, 0x01, 0x2a, 0xfb, 0xcd, 0xf2, 0xa0, 0xf9, 0x0e, 0xbc, 0x9f, 0x0c },
        .encoded = { 0xde990c7f, 0x6fe52eff, 0x98481cd2, 0x3deb8570, 0xfb2a0115, 0x61faf2cd, 0xbc0ef9a0, 0x55780c9f },
    },
    {
        .unencoded = { 0x9a, 0x10, 0x92, 0x03, 0x81, 0xfe, 0x41, 0x57, 0x77, 0x02, 0xcb, 0x20, 0x67, 0xa4, 0x97, 0xf3, 0xf8, 0xc7, 0x0d, 0x65, 0xcd, 0xfc, 0x15, 0xef },
        .encoded = { 0x0392109a, 0x4b64fe81, 0x02775741, 0x418820cb, 0xf397a467, 0x6998c7f8, 0xfccd650d, 0x6ba3ef15 },
    },
};

TEST_CASE("Test 3/4 Coding Scheme Algorithm", "[efuse]")
{
    const int num_tests = sizeof(coding_scheme_data)/sizeof(coding_scheme_test_t);
    for (int i = 0; i < num_tests; i++) {
        uint32_t result[8];
        const coding_scheme_test_t *t = &coding_scheme_data[i];

        printf("Test case %d...\n", i);
        esp_err_t r = esp_efuse_utility_apply_34_encoding(t->unencoded, result, sizeof(t->unencoded));
        TEST_ASSERT_EQUAL_HEX(ESP_OK, r);
        TEST_ASSERT_EQUAL_HEX32_ARRAY(t->encoded, result, 8);

        // Do the same, 6 bytes at a time
        for (int offs = 0; offs < sizeof(t->unencoded); offs += 6) {
            bzero(result, sizeof(result));
            r = esp_efuse_utility_apply_34_encoding(t->unencoded + offs, result, 6);
            TEST_ASSERT_EQUAL_HEX(ESP_OK, r);
            TEST_ASSERT_EQUAL_HEX32_ARRAY(t->encoded + (offs / 6 * 2), result, 2);
        }
    }
}

TEST_CASE("Test Coding Scheme for efuse manager", "[efuse]")
{
    int count_useful_reg = 0;
    int useful_data_in_byte;
    uint8_t buf[32];
    uint32_t encoded[8];
    bootloader_random_enable();
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(EFUSE_BLK2);
    if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
        printf("EFUSE_CODING_SCHEME_NONE\n");
        count_useful_reg = 8;
    } else if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
        printf("EFUSE_CODING_SCHEME_3_4\n");
        count_useful_reg = 6;
    } else if (coding_scheme == EFUSE_CODING_SCHEME_REPEAT) {
        printf("EFUSE_CODING_SCHEME_REPEAT\n");
        count_useful_reg = 4;
    }
    useful_data_in_byte = count_useful_reg * 4;

    for (int i = 0; i < 10; ++i) {
        esp_efuse_utility_erase_virt_blocks();
        printf("Test case %d...\n", i);
        memset(buf, 0, sizeof(buf));
        memset(encoded, 0, sizeof(encoded));
        // get test data
        bootloader_fill_random(buf, useful_data_in_byte);
        memset(buf, 0, i);

        esp_efuse_utility_reset();

        for (int j = 0; j < count_useful_reg; ++j) {
            REG_WRITE(EFUSE_BLK2_WDATA0_REG + j * 4, *((uint32_t*)buf + j));
        }

        TEST_ESP_OK(esp_efuse_utility_apply_new_coding_scheme());

        uint32_t w_data_after_coding[8] = { 0 };
        for (int j = 0; j < 8; ++j) {
            w_data_after_coding[j] = REG_READ(EFUSE_BLK2_WDATA0_REG + j * 4);
        }

        if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
            memcpy((uint8_t*)encoded, buf, sizeof(buf));
        } else if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
            TEST_ESP_OK(esp_efuse_utility_apply_34_encoding(buf, encoded, useful_data_in_byte));
        } else if (coding_scheme == EFUSE_CODING_SCHEME_REPEAT) {
            for (int j = 0; j < count_useful_reg; ++j) {
                encoded[j]     = *((uint32_t*)buf + j);
                encoded[j + 4] = encoded[j];
            }
        }
#ifdef CONFIG_EFUSE_VIRTUAL
        printf("Data from W reg\n");
        esp_efuse_utility_erase_virt_blocks();
        esp_efuse_utility_burn_efuses();
        esp_efuse_utility_debug_dump_blocks();
        printf("Data from encoded\n");
        for (int j = 0; j < 8; ++j) {
            printf("0x%08x ", encoded[j]);
        }
        printf("\nData from w_data_after_coding\n");
        for (int j = 0; j < 8; ++j) {
            printf("0x%08x ", w_data_after_coding[j]);
        }

        printf("\nData from buf\n");
        for (int j = 0; j < 8; ++j) {
            printf("0x%08x ", *((uint32_t*)buf + j));
        }
        printf("\n");
#endif
        TEST_ASSERT_EQUAL_HEX32_ARRAY(buf, w_data_after_coding, 8);
    }
    esp_efuse_utility_reset();
    bootloader_random_disable();
}

TEST_CASE("Test data does not match the coding scheme", "[efuse]")
{
    int count_useful_reg = 0;
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(EFUSE_BLK2);
    if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
        printf("EFUSE_CODING_SCHEME_NONE\n");
        count_useful_reg = 8;
    } else if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
        printf("EFUSE_CODING_SCHEME_3_4\n");
        count_useful_reg = 6 + 1;
    } else if (coding_scheme == EFUSE_CODING_SCHEME_REPEAT) {
        printf("EFUSE_CODING_SCHEME_REPEAT\n");
        count_useful_reg = 4 + 1;
    }

    esp_efuse_utility_reset();

    for (int i = 0; i < count_useful_reg; ++i) {
        TEST_ESP_OK(esp_efuse_utility_write_reg(2, i, 0xABCDEF01 + i));
    }

    if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
        TEST_ESP_OK(esp_efuse_utility_apply_new_coding_scheme());
    } else {
        TEST_ESP_ERR(ESP_ERR_CODING, esp_efuse_utility_apply_new_coding_scheme());
    }

    esp_efuse_utility_reset();
}
#endif // CONFIG_IDF_TARGET_ESP32
