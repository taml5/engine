#include "graphics.h"

float dot(struct vec2 *a, struct vec2 *b) {
    return (a->x * b->x) + (a->y * b->y);
}

float doti(struct vec2i *a, struct vec2i *b) {
    return (a->x * b->x) + (a->y * b->y);
}

float Q_rsqrt(float number) {
  int i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

struct vec2 normalise(struct vec2 *v) {
    float invsqrt = Q_rsqrt(powf(v->x, 2.0) + powf(v->y, 2.0));

    return (struct vec2) { 
        invsqrt * v->x,
        invsqrt * v->y
    };
}

struct vec2 wall_norm(struct wall *wall) {
    float walldir_x = wall->end->x - wall->start->x;
    float walldir_y = wall->end->y - wall->start->y;

    struct vec2 walln = {walldir_y, -walldir_x};
    return normalise(&walln);
}

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
    ray_dir->x = FOCAL_LEN * camera->anglecos + (WORLD2CAM(x) * camera->anglesin);
    ray_dir->y = FOCAL_LEN * camera->anglesin + (WORLD2CAM(x) * -camera->anglecos);

    struct ray *ray = malloc(sizeof(struct ray));
    ray->direction = ray_dir;
    ray->origin = camera->pos;
    return ray;
}

bool intersection(struct ray *ray, struct wall *wall, double min_t, double *depth, bool *is_vertex) {
    // implementation of cramer's rule on the system of linear equations
    // given by equating the parametric equations of both lines
    double walldir_x = wall->end->x - wall->start->x;
    double walldir_y = wall->end->y - wall->start->y;
    double p_min_l_x = ray->origin->x - wall->start->x;
    double p_min_l_y = ray->origin->y - wall->start->y;

    double denom = (walldir_x * ray->direction->y) - (walldir_y * ray->direction->x);
    if (-FUDGE < denom && denom < FUDGE) {
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
    
    *is_vertex = (s < 0 + EDGE_LIM || s > 1 - EDGE_LIM) ? true : false;
    *depth = t;
    return true;
}

bool first_hit(struct ray *ray, 
               struct sector *sector, 
               struct sector **sectors, 
               double min_t, 
               bool *is_vertex, 
               double *depth, 
               int *hit_id,
               int *hit_sector) {
    bool hit = false;
    *depth = HUGE_VAL;
    *hit_sector = sector->id;
    double curr_depth;
    for (int i = 0; i < sector->n_walls; i++) {
        if (intersection(ray, sector->walls[i], min_t, &curr_depth, is_vertex) && curr_depth < *depth) {
            hit = true;
            *hit_id = i;
            *depth = curr_depth;
        }
    }

    if (hit && sector->walls[*hit_id]->portal != 0) {
        return first_hit(ray, 
                         sectors[sector->walls[*hit_id]->portal - 1], 
                         sectors, 
                         *depth + FUDGE, 
                         is_vertex, 
                         depth, 
                         hit_id,
                         hit_sector);
    }
    return hit;
}

float lambertian(struct ray *ray, struct vec2 *light_pt, float depth, struct wall *wall, float intensity) {
    struct vec2 n = wall_norm(wall);
    struct vec2 q = {
        -((ray->origin->x - depth * ray->direction->x) - light_pt->x), 
        -((ray->origin->y - depth * ray->direction->y) - light_pt->y)
    };
    struct vec2 light = normalise(&q);
    return intensity * max(dot(&light, &n), 0.0);
}