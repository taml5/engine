#include "load.h"

struct sector **load_sectors(const char *filepath, int *n_sectors) {
    #ifdef DEBUG
    printf("Loading sectors from %s\n", filepath);
    #endif
    FILE *file;
    if ((file = fopen(filepath, "r")) == NULL) {
        perror("load_sectors");
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
        #ifdef DEBUG
        printf("loading sector %d with %d walls\n", id, n_walls);
        #endif
        struct sector *sector = malloc(sizeof(struct sector));
        struct wall **walls = malloc(sizeof(struct wall*) * n_walls);
        sector->floor_colour = malloc(sizeof(struct rgb));
        sector->ceil_colour = malloc(sizeof(struct rgb));
        
        int start_x, start_y, end_x, end_y, portal, tex_id;
        for (int j = 0; j < n_walls; j++) {
            struct wall *wall = malloc(sizeof(struct wall));
            struct vec2i *start = malloc(sizeof(struct vec2i));
            struct vec2i *end = malloc(sizeof(struct vec2i));
            fscanf(file, "%d %d %d %d %d %d", &start_x, &start_y, &end_x, &end_y, &portal, &tex_id);
            
            #ifdef DEBUG
            printf("WALL %d: (%d, %d) to (%d, %d), portal: %d\n", j, start_x, start_y, end_x, end_y, portal);
            #endif
            start->x = start_x;
            start->y = start_y;
            end->x = end_x;
            end->y = end_y;
            wall->start = start;
            wall->end = end;
            wall->portal = portal;
            wall->texture_id = tex_id;

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

texture load_texture(const char *filepath) {
    #ifdef DEBUG
    printf("Loading textures from %s\n", filepath);
    #endif
    FILE *file;
    if ((file = fopen(filepath, "rb")) == NULL) {
        perror("load_texture");
        return NULL;
    }

    int width, height, max_colour;
    fscanf(file, "P6\n %d %d %d", &width, &height, &max_colour);
    if (width != TEX_WIDTH || height != TEX_HEIGHT || max_colour != 255) {
        fprintf(stderr, "Invalid image format.\n");
        return NULL;
    }

    fseek(file, 1, SEEK_CUR);

    texture texture = malloc(TEX_HEIGHT * TEX_WIDTH * sizeof(struct rgb *));
    for (int y = TEX_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            unsigned char colour[3];
            fread(colour, 1, 3, file);
            struct rgb *texel = malloc(sizeof(struct rgb));
            texel->r = colour[0] / 255.0;
            texel->g = colour[1] / 255.0;
            texel->b = colour[2] / 255.0;
            texture[y * TEX_WIDTH + x] = texel;
        }
    }

    return texture;
}

struct light **load_lights(const char *filepath, int *n_lights) {
    #ifdef DEBUG
    printf("Loading lights from %s\n", filepath);
    #endif
    FILE *file;
    if ((file = fopen(filepath, "r")) == NULL) {
        perror("load_lights");
        return NULL;
    }
    fscanf(file, "%d", n_lights);
    struct light **lights = malloc(*n_lights * sizeof(struct light *));

    float x, y, intensity;
    for (int i = 0; i < *n_lights; i++) {
        fscanf(file, "%f %f %f", &x, &y, &intensity);
        struct light *light = malloc(sizeof(struct light));
        struct vec2 *pos = malloc(sizeof(struct vec2));
        pos->x = x;
        pos->y = y;
        
        light->pos = pos;
        light->intensity = intensity;
        lights[i] = light;
    }
    return lights;
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
        for (int j = 0; j < TEX_HEIGHT * TEX_WIDTH; j++){
            free(textures[i][j]);
        }        
        free(textures[i]);
    }
    free(textures);
    return;
}

void destroy_lights(struct light **lights, const int n_lights) {
    for (int i = 0; i < n_lights; i++) {
        struct light *light = lights[i];
        free(light->pos);
        free(light);
    }
    free(lights);
}