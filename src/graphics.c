#include "graphics.h"
#include "bayer.h"

float dot(const struct vec2 *a, const struct vec2 *b) {
    return (a->x * b->x) + (a->y * b->y);
}

float doti(const struct vec2i *a, const struct vec2i *b) {
    return (a->x * b->x) + (a->y * b->y);
}

float Q_rsqrt(const float number) {
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

struct vec2 normalise(const struct vec2 *v) {
    float invsqrt = Q_rsqrt(powf(v->x, 2.0) + powf(v->y, 2.0));

    return (struct vec2) { 
        invsqrt * v->x,
        invsqrt * v->y
    };
}

struct vec2 wall_norm(const struct wall *wall) {
    float walldir_x = wall->end->x - wall->start->x;
    float walldir_y = wall->end->y - wall->start->y;

    struct vec2 walln = {walldir_y, -walldir_x};
    return normalise(&walln);
}

void draw_vert(float *pixel_arr, int x, int y0, int y1, struct rgb *colour) {
    for (int i = y0; i < y1; i++) {
        if (fabs(colour->r - colour->g) > FUDGE 
        || fabs(colour->r - colour->b) > FUDGE 
        || fabs(colour->g - colour->b) > FUDGE) {
            // not greyscale - render in full colour
            pixel_arr[3 * (i * SCR_WIDTH + x) + 0] = colour->r;
            pixel_arr[3 * (i * SCR_WIDTH + x) + 1] = colour->g;
            pixel_arr[3 * (i * SCR_WIDTH + x) + 2] = colour->b;
        } else {
            // greyscale - apply dithering filter
            float lum_out = colour->r + bayer_matrix[x % BAYER_NUM][i % BAYER_NUM];
            float lum = lum_out > 0.5 ? 1.0 : 0.0;

            pixel_arr[3 * (i * SCR_WIDTH + x) + 0] = lum;
            pixel_arr[3 * (i * SCR_WIDTH + x) + 1] = lum;
            pixel_arr[3 * (i * SCR_WIDTH + x) + 2] = lum;
        }
    }
}

struct ray *viewing_ray(const struct camera *camera, const int x) {
    double u_coord = -1.0 + (2 * (x + 0.5)) / SCR_WIDTH;
    
    struct vec2 *ray_dir = malloc(sizeof(struct vec2));
    ray_dir->x = FOCAL_LEN * camera->anglecos + (WORLD2CAM(x) * camera->anglesin);
    ray_dir->y = FOCAL_LEN * camera->anglesin + (WORLD2CAM(x) * -camera->anglecos);

    struct ray *ray = malloc(sizeof(struct ray));
    ray->direction = ray_dir;
    ray->origin = camera->pos;
    return ray;
}

void destroy_ray(struct ray *ray) {
    free(ray->direction);
    free(ray);
}

bool intersection(
    const struct ray *ray, 
    const struct wall *wall, 
    const double min_t, 
    double *depth, 
    double *length, 
    bool *is_vertex
) {
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
        // intersection lies outside of the wall
        return false;
    }
    double t = ((walldir_x * p_min_l_y) - (walldir_y * p_min_l_x)) / denom;
    if (t < min_t) {
        // only need to check if intersection occurs behind camera (t is negative)
        return false;
    }
    
    *is_vertex = (s < 0 + EDGE_LIM || s > 1 - EDGE_LIM) ? true : false;
    *depth = t;
    *length = s;
    return true;
}

float lambertian(
    const struct ray *ray, 
    const struct vec2 *light_pt, 
    const float depth, 
    const struct wall *wall, 
    const float intensity
) {
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
    const struct camera *camera,
    struct sector **sectors,
    const struct ray *ray,
    const int x,
    const int sector_id,
    const double min_t,
    const int sector_dist
) {
    // find the first hit wall
    struct sector *sector = sectors[sector_id];
    bool hit = false, is_vertex = false;
    double depth = HUGE_VAL;
    int hit_sector = sector->id, hit_id;
    double curr_depth, curr_len;

    for (int i = 0; i < sector->n_walls; i++) {
        if (intersection(ray, sector->walls[i], min_t, &curr_depth, &curr_len, &is_vertex) && curr_depth < depth) {
            hit = true;
            hit_id = i;
            depth = curr_depth;
        }
    }

    if (!hit) {return;}  // no wall was found: don't draw anything

    // calculate depth effect
    int ceil_y = (int) (SCR_HEIGHT / 2) * ((sectors[hit_sector]->ceil_z - camera->height) / (depth * RATIO));
    int floor_y = (int) (SCR_HEIGHT / 2) * ((camera->height - sectors[hit_sector]->floor_z) / (depth * RATIO));
    int y0 = max((SCR_HEIGHT / 2) - (floor_y), 0);
    int y1 = min((SCR_HEIGHT / 2) + (ceil_y), SCR_HEIGHT - 1);

    struct wall *hit_wall = (sectors[hit_sector]->walls)[hit_id];
    // apply shading model to wall
    float lambertian_coeff = lambertian(ray, camera->pos, depth, hit_wall, min(0.4 / powf(depth, 2.0), 0.3));
    float intensity = min(max(AMBIENT + lambertian_coeff, 0.0), 1.0);  // clamp intensity coefficient
    struct rgb colour = {intensity, intensity, intensity};

    if (sector->walls[hit_id]->portal != 0) {
        // recursively render the other sector
        render(pixel_arr, camera, sectors, ray, x, sector->walls[hit_id]->portal, depth + FUDGE, sector_dist + 1);

        // calculate lintel height and convert to pixel coordinates
        float new_sector_ceil = sectors[sector->walls[hit_id]->portal]->ceil_z;
        int lintel_h = (int) (SCR_HEIGHT / 2) * ((new_sector_ceil - camera->height) / (depth * RATIO));
        int lintel_y =  min((SCR_HEIGHT / 2) + (lintel_h), SCR_HEIGHT - 1);
        // draw the lintel
        draw_vert(pixel_arr, x, lintel_y, y1, &colour);
        
        // calculate sill height and convert to pixel coordinates
        float new_sector_floor = sectors[sector->walls[hit_id]->portal]->floor_z;
        int sill_h = (int) (SCR_HEIGHT / 2) * ((camera->height - new_sector_floor) / (depth * RATIO));
        int sill_y = max((SCR_HEIGHT / 2) - sill_h, 0);
        // draw the sill
        draw_vert(pixel_arr, x, y0, sill_y, &colour);
    } else if (is_vertex) {
        struct rgb vertex_colour = {1.0, 1.0, 1.0};
        draw_vert(pixel_arr, x, y0, y1, &vertex_colour);
    } else {
        draw_vert(pixel_arr, x, y0, y1, &colour);
    }
    // draw floor and ceiling
    struct rgb shaded_floor_colour = {
        sector->floor_colour->r - (sector->floor_colour->r * SHADING_FAC * sector_dist),
        sector->floor_colour->g - (sector->floor_colour->g * SHADING_FAC * sector_dist),
        sector->floor_colour->b - (sector->floor_colour->b * SHADING_FAC * sector_dist)
    };
    struct rgb shaded_ceil_colour = {
        sector->ceil_colour->r - (sector->ceil_colour->r * SHADING_FAC * sector_dist),
        sector->ceil_colour->g - (sector->ceil_colour->g * SHADING_FAC * sector_dist),
        sector->ceil_colour->b - (sector->ceil_colour->b * SHADING_FAC * sector_dist)
    };
    draw_vert(pixel_arr, x, 0, y0, &shaded_floor_colour);
    draw_vert(pixel_arr, x, y1, SCR_HEIGHT, &shaded_ceil_colour);
}