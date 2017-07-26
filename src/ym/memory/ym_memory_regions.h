#pragma once

#define YM_MEM_REG_REGION_HEADS_ID              0
#define YM_MEM_REG_REGION_HEADS_BLOCK_SIZE    256
#define YM_MEM_REG_REGION_HEADS_BLOCK_COUNT     1
#define YM_MEM_REG_REGION_HEADS_OFFSET          0

#define YM_MEM_REG_DBG_STR_ID                   1
#define YM_MEM_REG_DBG_STR_BLOCK_SIZE         256
#define YM_MEM_REG_DBG_STR_BLOCK_COUNT          4
#define YM_MEM_REG_DBG_STR_OFFSET               \
(YM_MEM_REG_REGION_HEADS_OFFSET +               \
(YM_MEM_REG_REGION_HEADS_BLOCK_SIZE *           \
 YM_MEM_REG_REGION_HEADS_BLOCK_COUNT))

#define YM_MEM_REG_GFX_ID                       2

#define YM_MEM_REG_COUNT                        3


typedef
enum
{
    ym_mem_reg_region_heads = YM_MEM_REG_REGION_HEADS_ID,
    ym_mem_reg_dbg_str = YM_MEM_REG_DBG_STR_ID,
    ym_mem_reg_gfx = YM_MEM_REG_GFX_ID,
    ym_mem_reg_count = YM_MEM_REG_COUNT,
} ym_mem_reg_id;
