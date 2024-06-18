#include "graphics.h"

void destroy_ray(struct ray *ray) {
    free(ray->direction);
    free(ray);
}

struct ray *viewing_ray(struct camera *camera, int x) {
    double u_coord = -1.0 + (2 * (x + 0.5)) / SCR_WIDTH;
    
    struct vec2 *ray_dir = malloc(sizeof(struct vec2));
    ray_dir->x = FOCAL_LEN * camera->anglecos + ((-1 + (2 * (x + 0.5)) / SCR_WIDTH) * camera->anglesin);
    ray_dir->y = FOCAL_LEN * camera->anglesin + ((-1 + (2 * (x + 0.5)) / SCR_WIDTH) * -camera->anglecos);

    struct ray *ray = malloc(sizeof(struct ray));
    ray->direction = ray_dir;
    ray->origin = camera->pos;
    return ray;
}

double intersection(struct ray *ray, struct wall *wall) {
    double walldir_x = wall->right.x - wall->left.x;
    double walldir_y = wall->right.y - wall->right.y;
    double p_min_l_x = ray->origin->x - wall->left.x;
    double p_min_l_y = ray->origin->y - wall->left.y;

    double denom = (walldir_x * ray->direction->y) - (walldir_y * ray->direction->x);
    if (-0.000001f < denom && denom < 0.000001f) {
        return -1;
    }
    double s = ((p_min_l_x * ray->direction->y) - (p_min_l_y * ray->direction->x)) / denom;
    if (s < 0 || s > 1) {
        return -1;
    }
    double t = ((walldir_x * p_min_l_y) - (walldir_y * p_min_l_x)) / denom;
    if (t < 0) {
        return -1;
    }
    
    return t;
}