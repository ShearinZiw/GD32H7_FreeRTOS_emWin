/**
 *********************************************************************
 * @file    syscall.c
 * @brief   FatFS re-entrancy (multi-task safety) via FreeRTOS mutex
 *
 * When _FS_REENTRANT=1, FatFS calls these functions to protect
 * file system operations from concurrent access by multiple tasks.
 * _SYNC_t is typedef'd to void* in ffconf.h; we cast it to
 * SemaphoreHandle_t internally.
 *********************************************************************
 */

#include "ff.h"
#include "FreeRTOS.h"
#include "semphr.h"

int ff_cre_syncobj(BYTE vol, _SYNC_t *sobj) {
    (void)vol;
    *sobj = (void *)xSemaphoreCreateMutex();
    return (*sobj != NULL) ? 1 : 0;
}

int ff_req_grant(_SYNC_t sobj) {
    if (sobj != NULL &&
        xSemaphoreTake((SemaphoreHandle_t)sobj, (TickType_t)_FS_TIMEOUT) == pdTRUE) {
        return 1;
    }
    return 0;
}

void ff_rel_grant(_SYNC_t sobj) {
    if (sobj != NULL) {
        xSemaphoreGive((SemaphoreHandle_t)sobj);
    }
}

int ff_del_syncobj(_SYNC_t sobj) {
    if (sobj != NULL) {
        vSemaphoreDelete((SemaphoreHandle_t)sobj);
    }
    return 1;
}
