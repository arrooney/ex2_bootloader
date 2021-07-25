/*
 * bl_eeprom.c
 *
 *  Created on: May 17, 2021
 *      Author: Robert Taylor
 */

#include "bl_eeprom.h"
#include "ti_fee.h"

unsigned short crc16();

// Returns false on failure
bool eeprom_init() {
    int delayCount = 0;
    TI_Fee_Init();
    while (TI_Fee_GetStatus(0) != IDLE) {
        delayCount++;
        if (delayCount > 10000) { // timeout after trying this many times
            return false;
        }
    }
    return true;
}

void eeprom_shutdown() {
    TI_Fee_Shutdown();
}

char eeprom_get_boot_type() {
    char bootType;
    TI_Fee_ReadSync(BOOT_TYPE_BLOCK, BOOT_TYPE_OFFSET, (uint8_t *)(&bootType), BOOT_TYPE_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    return bootType;
}

void eeprom_set_boot_type(char boot) {
    TI_Fee_WriteSync(BOOT_TYPE_BLOCK, (uint8_t *)&boot);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
}

void eeprom_set_app_info(image_info i) {
    TI_Fee_WriteSync(APP_STATUS_BLOCKNUMBER, (uint8_t *)(&i));
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
}

image_info eeprom_get_app_info() {
    image_info out;
    TI_Fee_ReadSync(APP_STATUS_BLOCKNUMBER, APP_STATUS_OFFSET, (uint8_t *)(&out), APP_STATUS_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    return out;
}

void eeprom_set_golden_info(image_info i) {
    TI_Fee_WriteSync(GOLD_STATUS_BLOCKNUMBER, (uint8_t *)&i);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
}

image_info eeprom_get_golden_info() {
    image_info out = {0};
    TI_Fee_ReadSync(GOLD_STATUS_BLOCKNUMBER, GOLD_STATUS_OFFSET, (uint8_t *)(&out), GOLD_STATUS_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    return out;
}

int eeprom_set_csp_golden_key(uint32_t key) {
    csp_key_info set = {
        .exists = EXISTS_FLAG,
        .status = (csp_key_status) ACTIVE,
        .key = key,
        .crc = crc16((char *) &key, sizeof(key))
    };

    TI_Fee_WriteSync(CSP_GOLDEN_KEY_BLOCKNUMBER, (uint8_t *)&set);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    return 0;
}

int eeprom_set_csp_golden_key_active() {
    csp_key_info set = {0};
    TI_Fee_ReadSync(CSP_GOLDEN_KEY_BLOCKNUMBER, CSP_KEY_STATUS_OFFSET, (uint8_t *)(&set), CSP_KEY_ITEM_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    if (set.status == ACTIVE) {
        return 0;
    }
    set.status = ACTIVE;
    TI_Fee_WriteSync(CSP_GOLDEN_KEY_BLOCKNUMBER, (uint8_t *)&set);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    return eeprom_set_csp_secondary_key_inactive();
}

int eeprom_set_csp_golden_key_inactive() {
    csp_key_info set = {0};
    TI_Fee_ReadSync(CSP_GOLDEN_KEY_BLOCKNUMBER, CSP_KEY_STATUS_OFFSET, (uint8_t *)(&set), CSP_KEY_ITEM_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    if (set.status == INACTIVE) {
        return 0;
    }
    set.status = INACTIVE;
    TI_Fee_WriteSync(CSP_GOLDEN_KEY_BLOCKNUMBER, (uint8_t *)&set);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    return eeprom_set_csp_secondary_key_active();
}

int eeprom_set_csp_secondary_key(uint32_t key) {
    csp_key_info set = {
        .exists = EXISTS_FLAG,
        .status = (csp_key_status) INACTIVE,
        .key = key,
        .crc = crc16((char *) &key, sizeof(key))
    };

    TI_Fee_WriteSync(CSP_SECONDARY_KEY_BLOCKNUMBER, (uint8_t *)&set);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    return 0;
}

int eeprom_set_csp_secondary_key_active() {
    csp_key_info set = {0};
    TI_Fee_ReadSync(CSP_SECONDARY_KEY_BLOCKNUMBER, CSP_KEY_STATUS_OFFSET, (uint8_t *)(&set), CSP_KEY_ITEM_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    if (set.status == ACTIVE) {
        return 0;
    }
    set.status = ACTIVE;
    TI_Fee_WriteSync(CSP_SECONDARY_KEY_BLOCKNUMBER, (uint8_t *)&set);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    return eeprom_set_csp_golden_key_inactive();
}

int eeprom_set_csp_secondary_key_inactive() {
    csp_key_info set = {0};
    TI_Fee_ReadSync(CSP_SECONDARY_KEY_BLOCKNUMBER, CSP_KEY_STATUS_OFFSET, (uint8_t *)(&set), CSP_KEY_ITEM_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    if (set.status == INACTIVE) {
        return 0;
    }
    set.status = INACTIVE;
    TI_Fee_WriteSync(CSP_SECONDARY_KEY_BLOCKNUMBER, (uint8_t *)&set);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        return -1;
    }
    return eeprom_set_csp_golden_key_active();
}

/**
 * @brief The the golden CSP key
 * 
 * @return csp_key_info 
 */
csp_key_info eeprom_get_csp_golden_key() {
    csp_key_info out = {0};
    TI_Fee_ReadSync(CSP_GOLDEN_KEY_BLOCKNUMBER, CSP_KEY_STATUS_OFFSET, (uint8_t *)(&out), CSP_KEY_ITEM_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        out.status = RDERR;
    } else if (out.exists != EXISTS_FLAG) {
        out.status = DNE;
    } else if (out.crc != crc16((char *)&out.key, sizeof(out.key))) {
        out.status = CRCERR;
    }
    return out;
}

/**
 * @brief Get the secondary CSP key
 * 
 * @return csp_key_info 
 */
csp_key_info eeprom_get_csp_secondary_key() {
    csp_key_info out = {0};
    TI_Fee_ReadSync(CSP_SECONDARY_KEY_BLOCKNUMBER, CSP_KEY_STATUS_OFFSET, (uint8_t *)(&out), CSP_KEY_ITEM_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    if (res != JOB_OK) {
        out.status = RDERR;
    } else if (out.exists != EXISTS_FLAG) {
        out.status = DNE;
    } else if (out.crc != crc16((char *)&out.key, sizeof(out.key))) {
        out.status = CRCERR;
    }
    return out;
}

/**
 * @brief Get the active csp key object
 * If there is a secondary key that is active, it will always be returned. Otherwise, active or not, the information at the
 * golden key will be returned.
 * 
 * @return uint32_t the key
 */
uint32_t get_active_csp_key() {
    csp_key_info key = eeprom_get_csp_secondary_key();
    if (key.status == ACTIVE) {
        return key.key;
    }
    key = eeprom_get_csp_golden_key();
    return key.key; // return regardless of status so that the golden key is always default.
boot_info eeprom_get_boot_info() {
    boot_info out = {0};
    TI_Fee_ReadSync(BOOT_INFO_BLOCKNUMBER, BOOT_INFO_OFFSET, (uint8_t *)(&out), BOOT_INFO_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    return out;
}

void eeprom_set_boot_info(boot_info b) {
    TI_Fee_WriteSync(BOOT_INFO_BLOCKNUMBER, (uint8_t *)&b);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
}

bool verify_application() {
    image_info app_info = {0};
    app_info = eeprom_get_app_info();
    if (app_info.exists == EXISTS_FLAG) {
        if (crc16((char *)app_info.addr, app_info.size) == app_info.crc) {
            return true;
        } else return false;
    } else return false;
}

bool verify_golden() {
    image_info app_info = eeprom_get_golden_info();
    if (app_info.exists == EXISTS_FLAG) {
        if (crc16((char *)app_info.addr, app_info.size) == app_info.crc) {
            return true;
        } else return false;
    } else return false;
}

unsigned short crc16( char *ptr, int count)
{
   uint16_t crc;
   char i;
   crc = 0;
   while (--count >= 0)
   {
      crc = crc ^  ( ((int)*ptr)  << 8  ) ;
      ptr=ptr+1;
      i = 8;
      do
      {
         if (crc & 0x8000)
            crc = (crc << 1) ^ 0x1021;
         else
            crc = crc << 1;
      } while(--i);
   }
   return (crc);
}



