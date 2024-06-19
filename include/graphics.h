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
 * Draw a vertical line from (x, y0) to (x, y1) in the pixel buffer, with luminance `lum`
 * and alpha `alpha`.
 * 
 * @param pixel_arr: The pixel buffer.
 * @param x: The x coordinate of the line.
 * @param y0: The starting endpoint of the line.
 * @param y1: The ending endpoint of the line.
 * @param lum: The luminance of the line.
 * @param alpha: The alpha of the line.
 */
void draw_vert(float *pixel_arr, int x, int y0, int y1, float lum, float alpha);

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
 * Determine if there is an intersection between a given wall and a viewing ray. Store the parameter `t` 
 * from the parametric representation of the ray such that the vector `ray.origin + t * ray.direction` 
 * gives the intersection point.
 * 
 * @param ray: The viewing ray.
 * @param wall: The wall which an intersection is to be checked with.
 * @param depth: The "depth" of the intersection, that is, the distance between the camera and the 
 *               wall, given in the basis of the focal length. 
 * @return Whether there was an intersection or not.
 */
bool intersection(struct ray *ray, struct wall *wall, double *depth);


/**
 * Determine if there was an intersection between a given viewing ray and a wall in the sector. If there
 * is an intersection, store the depth and wall id in `depth` and `hit_id` respectively.
 * 
 * @param ray: The viewing ray.
 * @param sector: The sector to check for intersections within.
 * @param depth: The distance from the camera to the colliding wall.
 * @param hit_id: The id of the intersecting wall.
 * 
 * @return Whether there was an intersection or not.
 */
bool first_hit(struct ray *ray, struct sector *sector, double *depth, size_t hit_id);