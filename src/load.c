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

void destroy_sectors(struct sector **sectors, int *n_sectors) {
    for (int i = 1; i < *n_sectors + 1; i++) {
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