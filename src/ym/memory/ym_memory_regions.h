#pragma once

#define YM_MEM_REG_REGION_HEADS_ID              0
#define YM_MEM_REG_REGION_HEADS_BLOCK_SIZE   4096
#define YM_MEM_REG_REGION_HEADS_OFFSET          0

#define YM_MEM_REG_GFX_ID                       1
#define YM_MEM_REG_GFX_BLOCK_SIZE            4096 // orig: 256
#define YM_MEM_REG_GFX_OFFSET                   \
(YM_MEM_REG_REGION_HEADS_OFFSET +               \
 YM_MEM_REG_REGION_HEADS_BLOCK_SIZE)

#define YM_MEM_REG_GL_ID                        2
#define YM_MEM_REG_GL_BLOCK_SIZE             4096 // orig: 2048
#define YM_MEM_REG_GL_OFFSET                    \
(YM_MEM_REG_GFX_OFFSET +                        \
 YM_MEM_REG_GFX_BLOCK_SIZE)

// Telemetry should be last block, as it will mostly
// contain debug information,
// And won't run when the product is finished.
#define YM_MEM_REG_TELEMETRY_ID                 3
#define YM_MEM_REG_TELEMETRY_BLOCK_SIZE      4096 // orig: 256
#define YM_MEM_REG_TELEMETRY_OFFSET             \
(YM_MEM_REG_GL_OFFSET +                         \
 YM_MEM_REG_GL_BLOCK_SIZE)

#define YM_MEM_REG_COUNT                        4

typedef
enum
{
    ym_mem_reg_region_heads = YM_MEM_REG_REGION_HEADS_ID,
    ym_mem_reg_gfx = YM_MEM_REG_GFX_ID,
    ym_mem_reg_gl = YM_MEM_REG_GL_ID,
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
        case ym_mem_reg_gl:
            return "ym_mem_reg_gl";
        case ym_mem_reg_telemetry:
            return "ym_mem_reg_telemetry";
        case ym_mem_reg_count:
            return "ym_mem_reg_count";
    }
    return "UNKNOWN";
}
