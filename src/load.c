#include "load.h"

struct sector **load_sectors(const char *filepath, int *n_sectors) {
    FILE *file;
    if ((file = fopen(filepath, "r")) == NULL) {
        perror("load_sectors: ");
        return NULL;
    }
    fscanf(file, "%d", n_sectors);
    struct sector **sectors = malloc((*n_sectors + 1) * sizeof(struct sector));
    sectors[0] = NULL;

    int id, n_walls;
    float floor_z, ceil_z, floor_r, floor_g, floor_b, ceil_r, ceil_g, ceil_b;
    for (int i = 1; i < *n_sectors + 1; i++) {
        fscanf(file, "%d %d %f %f", &id, &n_walls, &floor_z, &ceil_z);
        fscanf(file, "%f %f %f %f %f %f", &floor_r, &floor_g, &floor_b, &ceil_r, &ceil_g, &ceil_b);
        printf("loading sector %d with %d walls\n", id, n_walls);
        struct sector *sector = malloc(sizeof(struct sector));
        struct wall **walls = malloc(sizeof(struct wall*) * n_walls);
        sector->floor_colour = malloc(sizeof(struct rgb));
        sector->ceil_colour = malloc(sizeof(struct rgb));
        
        int start_x, start_y, end_x, end_y, portal;
        for (int j = 0; j < n_walls; j++) {
            struct wall *wall = malloc(sizeof(struct wall));
            struct vec2i *start = malloc(sizeof(struct vec2i));
            struct vec2i *end = malloc(sizeof(struct vec2i));
            fscanf(file, "%d %d %d %d %d", &start_x, &start_y, &end_x, &end_y, &portal);
            
            printf("WALL %d: (%d, %d) to (%d, %d), portal: %d\n", j, start_x, start_y, end_x, end_y, portal);
            start->x = start_x;
            start->y = start_y;
            end->x = end_x;
            end->y = end_y;
            wall->start = start;
            wall->end = end;
            wall->portal = portal;
            wall->texture_id = 0;

            walls[j] = wall;
        }

        sector->id = id;
        sector->n_walls = n_walls;
        sector->walls = walls;
        sector->floor_z = floor_z;
        sector->ceil_z = ceil_z;
        
        sector->floor_colour->r = floor_r;
        sector->floor_colour->g = floor_g;
        sector->floor_colour->b = floor_b;
        sector->ceil_colour->r = ceil_r;
        sector->ceil_colour->g = ceil_g;
        sector->ceil_colour->b = ceil_b;

        sectors[i] = sector;
    }

    return sectors;
}

texture *load_textures(const char *filepath, const int n_textures) {
    FILE *file;
    if ((file = fopen(filepath, "r")) == NULL) {
        perror("load_sectors: ");
        return NULL;
    }
    // TODO: complete this function
    texture *textures = malloc(n_textures * sizeof(texture));
    for (int i = 0; i < n_textures; i++) {
        texture texture = malloc(TEX_HEIGHT * TEX_WIDTH * sizeof(struct rgb *));
        for (int y = 0; y < TEX_HEIGHT; y++) {
            for (int x = 0; x < TEX_WIDTH; x++) {
                struct rgb *texel = malloc(sizeof(struct rgb));
                if (x == 0 || y == 0 || x == TEX_WIDTH - 1 || y == TEX_HEIGHT - 1) {
                    texel->r = 0.0;
                    texel->g = 0.0;
                    texel->b = 0.0;
                } else if (x % 2 == y % 2) {
                    texel->r = 0.0;
                    texel->g = 0.0;
                    texel->b = 0.0;
                } else {
                    texel->r = 1.0;
                    texel->g = 0.0;
                    texel->b = 1.0;
                }
                texture[y * TEX_WIDTH + x] = texel;
            }
        }
        textures[i] = texture;
    }

    return textures;
}

void destroy_sectors(struct sector **sectors, const int n_sectors) {
    for (int i = 1; i < n_sectors + 1; i++) {
        for (int j = 0; j < sectors[i]->n_walls; j++) {
            struct wall *wall = sectors[i]->walls[j];
            free(wall->start);
            free(wall->end);
            free(wall);
        }
        free(sectors[i]->walls);
        free(sectors[i]->floor_colour);
        free(sectors[i]->ceil_colour);
        free(sectors[i]);
    }
    free(sectors);
    return;
}

void destroy_textures(texture *textures, const int n_textures) {
    for (int i = 0; i < n_textures; i++) {
        free(textures[i]);
    }
    free(textures);
    return;
}