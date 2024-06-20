#include "graphics.h"

void draw_vert(float *pixel_arr, int x, int y0, int y1, float lum, float alpha) {
    for (int i = y0; i < y1; i++) {
        pixel_arr[2 * (i * SCR_WIDTH + x)] = lum;
        pixel_arr[2 * (i * SCR_WIDTH + x) + 1] = alpha;
    }
}

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

bool intersection(struct ray *ray, struct wall *wall, double *depth, double min_t) {
    // implementation of cramer's rule on the system of linear equations
    // given by equating the parametric equations of both lines
    double walldir_x = wall->end->x - wall->start->x;
    double walldir_y = wall->end->y - wall->start->y;
    double p_min_l_x = ray->origin->x - wall->start->x;
    double p_min_l_y = ray->origin->y - wall->start->y;

    double denom = (walldir_x * ray->direction->y) - (walldir_y * ray->direction->x);
    if (-0.000001f < denom && denom < 0.000001f) {
        // the lines are parallel and will not intersect
        // a little error is given for floating point errors
        return false;
    }
    double s = ((p_min_l_x * ray->direction->y) - (p_min_l_y * ray->direction->x)) / denom;
    if (s < 0 || s > 1) {
        // intersection lies outside of the point
        return false;
    }
    double t = ((walldir_x * p_min_l_y) - (walldir_y * p_min_l_x)) / denom;
    if (t < min_t) {
        // only need to check if intersection occurs behind camera (t is negative)
        return false;
    }
    
    *depth = t;
    return true;
}

bool first_hit(struct ray *ray, struct sector *sector, struct sector **sectors, double *depth, int *hit_id, double min_t) {
    bool hit = false;
    *depth = HUGE_VAL;
    double curr_depth;
    for (int i = 0; i < sector->n_walls; i++) {
        if (intersection(ray, sector->walls[i], &curr_depth, min_t)) {
            if (curr_depth < *depth) {
                hit = true;
                *hit_id = i;
                *depth = curr_depth;
            }
        }
    }

    if (hit && sector->walls[*hit_id]->portal != 0) {
        return first_hit(ray, sectors[sector->walls[*hit_id]->portal - 1], sectors, depth, hit_id, *depth + 0.000001);
    }
    return hit;
}