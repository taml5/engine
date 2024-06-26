#ifndef GAME
#define GAME
#include "game.h"
#endif
#ifndef GRAPHICS
#define GRAPHICS
#include "graphics.h"
#endif

/**
 * Return whether the wall intersects the camera's path.
 * 
 * @param camera: The camera.
 * @param wall: The wall to check for an intersection.
 * @param new: The new position of the camera.
 * @param t: The parameter for the wall parametric equation.
 */
bool collision(struct camera *camera, struct wall *wall, struct vec2 *new, double *t) {
    double walldir_x = wall->end->x - wall->start->x;
    double walldir_y = wall->end->y - wall->start->y;
    double posdir_x = new->x - camera->pos->x;
    double posdir_y = new->y - camera->pos->y;
    double c1 = wall->start->x - camera->pos->x;
    double c2 = wall->start->y - camera->pos->y;

    double denom = posdir_x * walldir_y - posdir_y * walldir_x;
    if (-FUDGE < denom && denom < FUDGE) {
        return false;
    }
    double s = (c1 * walldir_y - walldir_x * c2) / denom;
    if (s < 0 + FUDGE || s > 1) {
        return false;
    }
    double param = (c1 * posdir_y - posdir_x * c2) / denom;
    if (param < 0 || param > 1) {
        return false;
    }
    *t = param;
    return true;
}

bool update_location(struct camera *camera,
                     struct sector **sectors,
                     struct vec2 *new,
                     int depth) {
    if (camera->pos->x == new->x && camera->pos->y == new->y) {
        return false;
    }

    double t;
    for (int i = 0; i < sectors[camera->sector - 1]->n_walls; i++) {
        struct wall *wall = sectors[camera->sector - 1]->walls[i];
        if (collision(camera, wall, new, &t)) {
            if (wall->portal != 0 && (t <= 0 + 0.005 || t >= 1 - 0.005)) {
                return false;
            } else if (wall->portal != 0 && fabs(sectors[camera->sector - 1]->floor_z  - sectors[wall->portal - 1]->floor_z) < 1.0) {
                camera->height += sectors[wall->portal - 1]->floor_z - sectors[camera->sector - 1]->floor_z;
                camera->sector = wall->portal;
                return true;
            }

            // calculate the resulting direction component along wall
            struct vec2 direction = {
                new->x - camera->pos->x,
                new->y - camera->pos->y
            };
            struct vec2 walldir = {
                wall->end->x - wall->start->x,
                wall->end->y - wall->start->y
            };
            double len = min(dot(&walldir, &direction) * Q_rsqrt(pow(walldir.x, 2.0) + pow(walldir.y, 2.0)), 0.5);
            new->x = camera->pos->x + 0.5 * len * walldir.x;
            new->y = camera->pos->y + 0.5 * len * walldir.y;
            if (depth < 10) {
                return update_location(camera, sectors, new, depth + 1);
            } else {
                return false;
            }
        }
    }

    return true;
}