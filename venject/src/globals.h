#ifndef VENJECT__GLOBALS_H
#define VENJECT__GLOBALS_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
	const uint8_t *data;
	size_t		   data_len;
} payload_t;

extern payload_t g_payload_asar;

#endif // VENJECT__GLOBALS_H
