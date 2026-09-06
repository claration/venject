#ifndef VENJECT__GLOBALS_H
#define VENJECT__GLOBALS_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    const uint8_t *data;
    size_t data_len;
} payload_t;

extern payload_t g_payload_asar;

#define VENJECT_ASAR_PATH "/tmp/vencord.asar"

#endif // VENJECT__GLOBALS_H
