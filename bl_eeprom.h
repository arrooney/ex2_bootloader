/*
 * bl_eeprom.h
 *
 *  Created on: May 17, 2021
 *      Author: Robert Taylor
 */

#ifndef BL_EEPROM_H_
#define BL_EEPROM_H_

#include <stdbool.h>
#include <stdint.h>

#define EXISTS_FLAG 0xA5A5A5A5

#define APP_STATUS_BLOCKNUMBER 2 // status byte reports 1 if program present
#define APP_STATUS_OFFSET 0
#define APP_STATUS_LEN sizeof(image_info)

#define GOLD_STATUS_BLOCKNUMBER 3
#define GOLD_STATUS_OFFSET 0
#define GOLD_STATUS_LEN sizeof(image_info)

#define CSP_GOLDEN_KEY_BLOCKNUMBER 4

#define CSP_SECONDARY_KEY_BLOCKNUMBER 5

#define CSP_KEY_STATUS_OFFSET 0

#define CSP_KEY_ITEM_LEN sizeof(csp_key_info)

#define GOLD_MINIMUM_ADDR 0x00018000
#define GOLD_DEFAULT_ADDR 0x00018000
#define GOLD_START_BANK 0

#define APP_MINIMUM_ADDR 0x00200000
#define APP_DEFAULT_ADDR 0x00200000
#define APP_START_BANK 1

#define BOOTLOADER_MAX_ADDR 0x0017FFF

#define BOOT_TYPE_BLOCK 1
#define BOOT_TYPE_OFFSET 0
#define BOOT_TYPE_LEN 1

#define BOOT_INFO_BLOCKNUMBER 4
#define BOOT_INFO_OFFSET 0
#define BOOT_INFO_LEN 8

// Representation of data which will be stored in FEE flash
typedef struct __attribute__((packed)) {
    uint32_t exists; // EXISTS_FLAG for exists, else does not exist
    uint32_t size;
    uint32_t addr;
    uint16_t crc;
} image_info;

typedef enum {
    ACTIVE      = 'A',
    INACTIVE    = 'I',
    RDERR       = 1,
    DNE         = 2,
    CRCERR      = 4
} csp_key_status;

typedef struct __attribute__((packed)) {
    uint32_t exists;
    char status;
    uint32_t key;
    uint16_t crc;
} csp_key_info;
// Representation of data which will be stored in FEE flash
typedef struct __attribute__((packed)) {
    uint32_t count; // total number of boot attempts
    uint32_t attempts; // total attempts since last failure
} boot_info;

bool eeprom_init();

void eeprom_shutdown();

char eeprom_get_boot_type();

void eeprom_set_boot_type(char boot);

void eeprom_set_app_info(image_info i);

image_info eeprom_get_app_info();

void eeprom_set_golden_info(image_info i);

image_info eeprom_get_golden_info();

boot_info eeprom_get_boot_info();

void eeprom_set_boot_info(boot_info b);

bool verify_application();

bool verify_golden();

#endif /* BL_EEPROM_H_ */
