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
 */
bool collision(struct camera *camera, struct wall *wall, struct vec2 *new) {
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
    if (s < 0 || s > 1) {
        return false;
    }
    double t = (posdir_x * c2 - c1 * posdir_y) / denom;
    if (t < -1 || t > 2) {
        return false;
    }
    return true;
}

bool update_location(struct camera *camera,
                     struct sector **sectors,
                     struct vec2 *new) {
    if (camera->pos->x == new->x && camera->pos->y == new->y) {
        return false;
    }

    for (int i = 0; i < sectors[camera->sector - 1]->n_walls; i++) {
        struct wall *wall = sectors[camera->sector - 1]->walls[i];
        if (collision(camera, wall, new)) {
            if (wall->portal != 0) {
                camera->sector = wall->portal;
                return true;
            } else {
                // calculate the resulting direction component along wall
                struct vec2 direction = {
                    new->x - camera->pos->x,
                    new->y - camera->pos->y
                };
                struct vec2 walldir = {
                    wall->end->x - wall->start->x,
                    wall->end->y - wall->start->y
                };
                float len = dot(&walldir, &direction) * Q_rsqrt(pow(walldir.x, 2.0) + pow(walldir.y, 2.0));
                new->x = camera->pos->x + 0.5 * len * walldir.x;
                new->y = camera->pos->y + 0.5 * len * walldir.y;
                return update_location(camera, sectors, new);
            }
        }
    }

    return true;
}