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
 * Generate a ray from the camera through a particular y-slice of the image plane.
 * 
 * @param camera: A pointer to the camera.
 * @param x: The x coordinate (in the image plane) of the pixel to cast the ray through
 * @returns A heap allocated ray struct, with the origin being the camera, and the direction
 *          going through the y-slice. In the parametric representation, when t=1, the resulting
 *          vector lies on the image plane.
 */
struct ray *viewing_ray(struct camera *camera, int x);

/**
 * Deallocate the memory used by a ray.
 * 
 * @param ray: The ray to be deallocated.
 */
void destroy_ray(struct ray *ray);

/**
 * Determine if there is an intersection between a wall and a viewing ray. If there is an 
 * intersection, return the parameter `t` from the parametric representation of the ray 
 * such that the vector `ray.origin + t * ray.direction` results in the intersection. If 
 * there is no intersection, return -1.0.
 * 
 * @param ray: The viewing ray.
 * @param wall: The wall which an intersection is to be checked with.
 * @returns The "depth" of the intersection, that is, the distance between the camera and the 
 *          wall, given in the basis of the focal length. If there is no intersection, 
 *          -1.0 is returned.
 */
double intersection(struct ray *ray, struct wall *wall);