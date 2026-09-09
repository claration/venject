#include "globals.h"

#include "gen/embedded/app.h"

payload_t g_payload_asar = {
	.data	  = embedded_app_asar,
	.data_len = embedded_app_asar_len,
};
