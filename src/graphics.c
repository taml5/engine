#include "graphics.h"
#include "bayer.h"

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
  i  = * ( int * ) &y;                       // evil floating point bit level hacking
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

void draw_vert(float *pixel_arr, int x, int y0, int y1, float r, float g, float b) {
    for (int i = y0; i < y1; i++) {
        if (!(fabs(r - g) < FUDGE || fabs(r - g) < FUDGE)) {
            pixel_arr[3 * (i * SCR_WIDTH + x) + 0] = r;
            pixel_arr[3 * (i * SCR_WIDTH + x) + 1] = g;
            pixel_arr[3 * (i * SCR_WIDTH + x) + 2] = b;
        } else {
            float lum_out = r + bayer_matrix[x % BAYER_NUM][i % BAYER_NUM];
            float lum = lum_out > 0.5 ? 1.0 : 0.0;

            pixel_arr[3 * (i * SCR_WIDTH + x) + 0] = lum;
            pixel_arr[3 * (i * SCR_WIDTH + x) + 1] = lum;
            pixel_arr[3 * (i * SCR_WIDTH + x) + 2] = lum;
        }
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

void render(
    float *pixel_arr,
    struct camera *camera,
    struct sector **sectors,
    struct ray *ray,
    int x,
    struct sector *sector,
    double min_t
) {
    // find the first hit wall
    bool hit = false, is_vertex = false;
    double depth = HUGE_VAL;
    int hit_sector = sector->id, hit_id;
    double curr_depth;

    for (int i = 0; i < sector->n_walls; i++) {
        if (intersection(ray, sector->walls[i], min_t, &curr_depth, &is_vertex) && curr_depth < depth) {
            hit = true;
            hit_id = i;
            depth = curr_depth;
        }
    }
    

    if (!hit) {
        ;
    } else {
        // calculate depth effect
        // XXX: THE VARIABLE NAMES ARE NOT INDICATIVE OF WHAT THEY ACTUALLY ARE!!!
        int floor_y = (int) (SCR_HEIGHT / 2) * ((sectors[hit_sector - 1]->ceil_z - camera->height) / (depth * RATIO));
        int ceil_y = (int) (SCR_HEIGHT / 2) * ((camera->height - sectors[hit_sector - 1]->floor_z) / (depth * RATIO));
        int y0 = max((SCR_HEIGHT / 2) - (ceil_y), 0);
        int y1 = min((SCR_HEIGHT / 2) + (floor_y), SCR_HEIGHT - 1);

        struct wall *hit_wall = (sectors[hit_sector - 1]->walls)[hit_id];
        // calculate colour based on angle to x-axis
        struct vec2 light = {1.5, 1.5};
        float lambertian_coeff = 0;
        lambertian_coeff += lambertian(ray, &light, depth, hit_wall, 0.1);
        lambertian_coeff += lambertian(ray, camera->pos, depth, hit_wall, min(0.4 / powf(depth, 2.0), 0.3));

        if (sector->walls[hit_id]->portal != 0) {
            render(
                pixel_arr,
                camera,
                sectors,
                ray,
                x,
                sectors[sector->walls[hit_id]->portal - 1],
                depth + FUDGE
            );
            // draw the lintel
            float new_sector_ceil = sectors[sector->walls[hit_id]->portal - 1]->ceil_z;
            int new_ceil_y = (int) (SCR_HEIGHT / 2) * ((new_sector_ceil - camera->height) / (depth * RATIO));
            int new_ceil_y0 =  min((SCR_HEIGHT / 2) + (new_ceil_y), SCR_HEIGHT - 1);
            draw_vert(pixel_arr, x, new_ceil_y0, y1, max(AMBIENT + lambertian_coeff, 0.0), max(AMBIENT + lambertian_coeff, 0.0), max(AMBIENT + lambertian_coeff, 0.0));
            
            // // draw the step
            float new_sector_floor = sectors[sector->walls[hit_id]->portal - 1]->floor_z;
            int floor_y = (int) (SCR_HEIGHT / 2) * ((camera->height - new_sector_floor) / (depth * RATIO));
            int floor_y0 = max((SCR_HEIGHT / 2) - floor_y, 0);
            draw_vert(pixel_arr, x, y0, floor_y0, max(AMBIENT + lambertian_coeff, 0.0), max(AMBIENT + lambertian_coeff, 0.0), max(AMBIENT + lambertian_coeff, 0.0));
        } else if (is_vertex) {
            draw_vert(pixel_arr, x, y0, y1, 1.0, 1.0, 1.0);
        } else {
            draw_vert(pixel_arr, x, y0, y1, max(AMBIENT + lambertian_coeff, 0.0), max(AMBIENT + lambertian_coeff, 0.0), max(AMBIENT + lambertian_coeff, 0.0));
        }
        // draw floor and ceiling
        draw_vert(pixel_arr, x, y1, SCR_HEIGHT, 0.0, 0.0, 0.1);
        draw_vert(pixel_arr, x, 0, y0, 0.1, 0.0, 0.0);
    }
}