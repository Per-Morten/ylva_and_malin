#include <ym_resource.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

ym_errc
ym_resource_load_map(const char* filepath,
                     ym_tileset** out_tileset,
                     int* out_tileset_count,
                     ym_layer** out_layer,
                     int* out_layer_count)
{
    FILE* map = fopen(filepath, "r");
    if (!map || errno != 0)
    {
        YM_WARN("%s: Could not open file: %s, system error: %s",
                ym_errc_str(ym_errc_system_error),
                filepath,
                strerror(errno));
        return ym_errc_system_error;
    }

    char buffer[256];

    // Ignore first two lines
    (void)fgets(buffer, 256, map);
    (void)fgets(buffer, 256, map);

    // Ugly hack for testing
    static ym_tileset tilebuf[256];
    int tilebuf_count = 0;

    // Parsing all the tilesets
    bool section_over = false;
    fgets(buffer, sizeof(buffer), map);
    while (!section_over)
    {
        ym_tileset* tile = &tilebuf[tilebuf_count];

        sscanf(buffer, " <tileset firstgid=\"%d\" name=\"%*[^\"]\" tilewidth=\"%d\" tileheight=\"%d\" tilecount=\"%d\">",
               &tile->firstgid,
               &tile->tilewidth,
               &tile->tileheight,
               &tile->tilecount);

        int height = 0;
        int width = 0;
        fgets(buffer, sizeof(buffer), map);
        sscanf(buffer, " <image source=\"../sprites/%[^\"]\" width=\"%d\" height=\"%d\"",
               tile->filename, &height, &width);

        tile->rowcount = height / tile->tileheight;
        tile->colcount = width / tile->tilewidth;

        tilebuf_count++;

        while (true)
        {
            fgets(buffer, sizeof(buffer), map);
            if (strstr(buffer, "<tileset") != NULL)
            {
                break;
            }
            if (strstr(buffer, "<layer") != NULL)
            {
                section_over = true;
                break;
            }
        }
    }

    *out_tileset = tilebuf;
    *out_tileset_count = tilebuf_count;

    // Ugly hack for testing
    static ym_layer layerbuf[256];
    int layerbuf_count = 0;

    // Parsing layers
    section_over = false;
    while (!section_over)
    {
        // Ignore <data encoding=...
        fgets(buffer, sizeof(buffer), map);

        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                int id;
                fscanf(map, "%d%*c", &id);
                layerbuf[layerbuf_count].sprite_ids[i][j] = id;
                layerbuf[layerbuf_count].coordinates[i][j].x = j;
                layerbuf[layerbuf_count].coordinates[i][j].y = i;
            }
        }
        layerbuf_count++;

        while (true)
        {
            char* ptr = fgets(buffer, sizeof(buffer), map);
            if (strstr(buffer, "<layer") != NULL)
            {
                break;
            }
            if (ptr == NULL)
            {
                section_over = true;
                break;
            }
        }
    }

    *out_layer = layerbuf;
    *out_layer_count = layerbuf_count;

    fclose(map);

    return ym_errc_success;
}

ym_errc
ym_resource_parse_map(const ym_tileset* tileset,
                      int tileset_count,
                      ym_layer* layer,
                      int layer_count)
{
    // SUPER INNEFICIENT, FIND DIFFERENT WAY OF DOING THIS
    for (int i = 0; i < layer_count; i++)
    {
        for (int x = 0; x < 10; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                if (layer[i].sprite_ids[x][y] == 0)
                    continue;

                // Find the correct tileset
                int cnt = tileset_count - 1;
                while (layer[i].sprite_ids[x][y] < tileset[cnt].firstgid)
                    cnt--;

                layer[i].sprite_ids[x][y] -= tileset[cnt].firstgid;
                layer[i].sheet_ids[x][y] = tileset[cnt].sheet_id;
            }
        }
    }


    return ym_errc_success;
}
