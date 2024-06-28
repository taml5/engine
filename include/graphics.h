#ifndef GAME
#define GAME
#include "game.h"
#endif

#define PI 3.1415627f
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

#define FOCAL_LEN 1  // the distance from the camera to the image plane, in game units

#define SCR_WIDTH 640  // screen width
#define SCR_HEIGHT 480  // screen height
#define RATIO ((float) SCR_HEIGHT / (float) SCR_WIDTH)

#define EDGE_LIM 0.005  // limit for edge detection
#define WORLD2CAM(x) (-1 + (2 * (x + 0.5)) / SCR_WIDTH)   // transformation from world plane to image plane

#define AMBIENT 0.0  // the ambient light intensity value
#define SHADING_FAC 0.2  // determines floor/ceiling intensity per sector distance

/**
 * A struct representing a light in the world.
 * 
 * @param pos: The position of the light in world coordinates.
 * @param intensity: The intensity of the light, between 0.0 and 1.0.
 */
struct light {
    struct vec2 pos;
    float intensity;
};

/**
 * Return the dot product between the vectors a and b.
 */
float dot(struct vec2 *a, struct vec2 *b);

/**
 * Return the dot product between the integer vectors a and b.
 */
float doti(struct vec2i *a, struct vec2i *b);

/**
 * https://en.wikipedia.org/wiki/Fast_inverse_square_root
 * 
 * The fast inverse square root algorithm from Quake III. Directly
 * copied from wikipedia except the type of i is changed from `long` to `int`.
 * 
 * @param number: A number.
 * @return The inverse square root of the number.
 */
float Q_rsqrt(float number);

/**
 * Return a normalised version of the given vector v.
 */
struct vec2 normalise(struct vec2 *v);

/**
 * Return the clockwise normal to the wall.
 */
struct vec2 wall_norm(struct wall *wall);

/**
 * Draw a vertical line from (x, y0) to (x, y1) in the pixel buffer.
 * 
 * @param pixel_arr: The pixel buffer.
 * @param x: The x coordinate of the line.
 * @param y0: The starting endpoint of the line.
 * @param y1: The ending endpoint of the line.
 * @param colour: The colour of the line.
 */
void draw_vert(float *pixel_arr, int x, int y0, int y1, struct rgb *colour);

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
 * @param min_t: The minimum depth considered.
 * @param depth: The "depth" of the intersection, that is, the distance between the camera and the 
 *               wall, given in the basis of the focal length.
 * @param length: How far along the wall with respect to the start endpoint the intersection occurs at.
 * @param is_vertex: Whether the hit point is on the endpoints or not.
 * @return Whether there was an intersection or not.
 */
bool intersection(struct ray *ray, struct wall *wall, double min_t, double *depth, double *length, bool *is_vertex);

/**
 * Calculate the luminosity given by the wall from the light at `light_pt` with intensity `intensity` using
 * the Lambertian model.
 * 
 * @param ray: The viewing ray that hits the wall.
 * @param light_pt: The position of the light.
 * @param depth: The distance from the camera to the wall.
 * @param wall: The wall.
 * @param intensity: The intensity of light.
 */
float lambertian(struct ray *ray, struct vec2 *light_pt, float depth, struct wall *wall, float intensity);

/**
 * Render the world scene on the given x coordinate.
 * 
 * @param pixel_arr: The pixel buffer.
 * @param camera: The camera.
 * @param sectors: The array of sectors.
 * @param ray: The light ray from the camera through the x coordinate on the image plane.
 * @param x: The x coordinate of the image plane.
 * @param sector_id: The id of the sector to be rendered.
 * @param min_t: The minimum distance of objects to be rendered.
 */
void render(float *pixel_arr,
    struct camera *camera,
    struct sector **sectors,
    struct ray *ray,
    int x,
    int sector_id,
    double min_t,
    int sector_dist
);