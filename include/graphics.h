#ifndef GAME
#define GAME
#include "game.h"
#endif

#define PI 3.1415627f
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

// constants for the alpha max plus beta min algorithm
// https://en.wikipedia.org/wiki/Alpha_max_plus_beta_min_algorithm
#define ALPHA 0.960433870103
#define BETA 0.397824734759

#define FOCAL_LEN 1  // the distance from the camera to the image plane, in game units
#define WORLD2CAM(x) (-1 + (2 * (x + 0.5)) / SCR_WIDTH)  // transformation from world plane to image plane

#define EDGE_LIM 0.01  // limit for edge detection
#define AMBIENT 0.0  // the ambient light intensity value
#define SHADING_FAC 0.25  // determines floor/ceiling intensity per sector distance

// #define BAYER  // whether to apply the dithering filter
#define BAYER_NUM 8  // the size of the bayer matrix

/**
 * Return the dot product between the vectors a and b.
 */
float dot(const struct vec2 *a, const struct vec2 *b);

/**
 * Return the dot product between the integer vectors a and b.
 */
float doti(const struct vec2i *a, const struct vec2i *b);

/**
 * https://en.wikipedia.org/wiki/Fast_inverse_square_root
 * 
 * The fast inverse square root algorithm from Quake III. Directly
 * copied from wikipedia except the type of i is changed from `long` to `int`.
 * 
 * @param number: A number.
 * @return The inverse square root of the number.
 */
float Q_rsqrt(const float number);

/**
 * Return a normalised version of the given vector v.
 */
struct vec2 normalise(const struct vec2 *v);

/**
 * Return the clockwise normal to the wall.
 */
struct vec2 wall_norm(const struct wall *wall);


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
struct ray *viewing_ray(const struct camera *camera, const int x);

/**
 * Deallocate the memory used by a ray.
 * 
 * @param ray: The ray to be deallocated.
 */
void destroy_ray(struct ray *ray);

/**
 * Render the world scene on the given x coordinate.
 * 
 * @param pixel_arr: The pixel buffer.
 * @param camera: The camera.
 * @param sectors: The array of sectors.
 * @param textures: The array of textures.
 * @param lights: The array of lights.
 * @param n_lights: The number of lights in the map.
 * @param ray: The light ray from the camera through the x coordinate on the image plane.
 * @param x: The x coordinate of the image plane.
 * @param sector_id: The id of the sector to be rendered.
 * @param min_t: The minimum distance of objects to be rendered.
 */
void render(float *pixel_arr,
    const struct camera *camera,
    struct sector * const * const sectors,
    texture *textures,
    struct light **lights,
    const int n_lights,
    const struct ray *ray,
    const int x,
    const int sector_id,
    const double min_t,
    const int sector_dist
);