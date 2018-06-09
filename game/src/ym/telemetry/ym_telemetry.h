#pragma once
#include <ym_core.h>
#include <ym_memory.h>

YM_NO_DISCARD
ym_errc
ym_telemetry_init(ym_mem_region* region);

YM_NO_DISCARD
ym_errc
ym_telemetry_shutdown();
