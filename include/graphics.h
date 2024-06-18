#ifndef GAME
#define GAME
#include "game.h"
#endif

#define PI 3.1415627f
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

#define FOCAL_LEN 1

#define SCR_WIDTH 640  // screen width
#define SCR_HEIGHT 480  // screen height

/**
 * A struct representing a ray described parametrically.
 * 
 * @param origin: The origin of the ray.
 * @param direction: The direction at which it is going.
 */
struct ray {
    struct vec2 *origin, *direction;
};

/**
 * TODO: fill this in
 * 
 * @param camera: A pointer to the camera.
 * @param x: The x coordinate (in the image plane) of the pixel to cast the ray through
 * @returns A heap allocated ray struct.
 */
struct ray *viewing_ray(struct camera *camera, int x);

/**
 * Deallocate the memory used by a ray.
 */
void destroy_ray(struct ray *ray);

/**
 * TODO: fill this in
 * 
 * @param ray:
 * @param wall:
 * @returns ...
 */
double intersection(struct ray *ray, struct wall *wall);