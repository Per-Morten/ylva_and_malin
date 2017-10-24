#pragma once

#define YM_MEM_REG_REGION_HEADS_ID              0
#define YM_MEM_REG_REGION_HEADS_BLOCK_SIZE    256
#define YM_MEM_REG_REGION_HEADS_OFFSET          0

#define YM_MEM_REG_GFX_ID                       1
#define YM_MEM_REG_GFX_BLOCK_SIZE             256
#define YM_MEM_REG_GFX_OFFSET                   \
(YM_MEM_REG_REGION_HEADS_OFFSET +               \
 YM_MEM_REG_REGION_HEADS_BLOCK_SIZE)

// Telemetry should be last block, as it will mostly
// contain debug information,
// And won't run when the product is finished.
#define YM_MEM_REG_TELEMETRY_ID                 2
#define YM_MEM_REG_TELEMETRY_BLOCK_SIZE         0
#define YM_MEM_REG_TELEMETRY_OFFSET             \
(YM_MEM_REG_GFX_OFFSET +                        \
 YM_MEM_REG_GFX_BLOCK_SIZE)

#define YM_MEM_REG_COUNT                        3

typedef
enum
{
    ym_mem_reg_region_heads = YM_MEM_REG_REGION_HEADS_ID,
    ym_mem_reg_gfx = YM_MEM_REG_GFX_ID,
    ym_mem_reg_telemetry = YM_MEM_REG_TELEMETRY_ID,
    ym_mem_reg_count = YM_MEM_REG_COUNT,
} ym_mem_reg_id;

YM_INLINE
const char*
ym_mem_reg_id_str(ym_mem_reg_id id)
{
    switch (id)
    {
        case ym_mem_reg_region_heads:
            return "ym_mem_reg_region_heads";
        case ym_mem_reg_gfx:
            return "ym_mem_reg_gfx";
        case ym_mem_reg_telemetry:
            return "ym_mem_reg_telemetry";
        case ym_mem_reg_count:
            return "ym_mem_reg_count";
    }
    return "UNKNOWN";
}
