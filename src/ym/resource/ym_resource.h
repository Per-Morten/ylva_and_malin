#pragma once
#include <ym_core.h>
#include <ym_gfx_sprite.h>

typedef struct
{
    char filename[256];
    int tilewidth;
    int tileheight;
    int firstgid;
    int tilecount;
    int rowcount;
    int colcount;
    ym_sheet_id sheet_id;
} ym_tileset;

typedef struct
{
    ym_vec2 coordinates[10][10];
    ym_sheet_id sheet_ids[10][10];
    ym_sprite_id sprite_ids[10][10];
} ym_layer;

// Returns several arrays,
// * one containing information about all the tilesets
//      * Tilewidth & Tileheight, use this with width and height of image to determine columns and rows
// * one containing a collection of coordinates and id's that needs to be tied to tiles


ym_errc
ym_resource_load_map(const char* filepath,
                     ym_tileset** out_tileset,
                     int* out_tileset_count,
                     ym_layer** out_layer,
                     int* out_layer_count);

ym_errc
ym_resource_parse_map(const ym_tileset* tileset,
                      int tileset_count,
                      ym_layer* layer,
                      int layer_count);
