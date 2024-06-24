#ifndef GAME
#define GAME
#include "game.h"
#endif
#include "graphics.h"
#include "load.h"

/**
 * Process the inputs of the user and perform different actions based on them.
 * 
 * @param window: A pointer to the GLFW window struct.
 * @param camera: A pointer to the camera struct.
 */
void process_input(GLFWwindow *window, 
                   struct camera *camera, 
                   struct sector **sectors,
                   struct vec2 *new)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        // turn left
        camera->angle = fmod(camera->angle + ROTSPD, PI * 2);
        camera->anglecos = cos(camera->angle);
        camera->anglesin = sin(camera->angle);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        // turn right
        camera->angle = fmod(camera->angle - ROTSPD, PI * 2);
        camera->anglecos = cos(camera->angle);
        camera->anglesin = sin(camera->angle);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // go back
        new->x = camera->pos->x + MVTSPD * camera->anglecos;
        new->y = camera->pos->y + MVTSPD * camera->anglesin;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // go forward
        new->x = camera->pos->x - MVTSPD * camera->anglecos;
        new->y = camera->pos->y - MVTSPD * camera->anglesin;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // go left
        new->x = camera->pos->x - MVTSPD * camera->anglesin;
        new->y = camera->pos->y + MVTSPD * camera->anglecos;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // go right
        new->x = camera->pos->x + MVTSPD * camera->anglesin;
        new->y = camera->pos->y - MVTSPD * camera->anglecos;
    }
}

int main(int argc, char *argv[]) {
    if (!glfwInit()) {
        fprintf(stderr, "Error: GLFW failed to initialize\n");
        exit(1);
    }
    // print version to stdout
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    printf("Running against GLFW %i.%i.%i\n", major, minor, revision);

    // load sectors and build sector and wall structs
    int n_sectors;
    struct sector **sectors = load_sectors("./content/map.txt", &n_sectors);

    // initialise pixel buffer storing luminance and alpha
    float pixel_arr[SCR_WIDTH * SCR_HEIGHT * 2];
    memset(pixel_arr, 0, sizeof(float) * 2 * SCR_HEIGHT * SCR_WIDTH);

    // initialise camera
    struct camera *camera = malloc(sizeof(struct camera));
    camera->pos = malloc(sizeof(struct vec2));
    camera->pos->x = 2;
    camera->pos->y = 2;
    camera->angle = PI;
    camera->anglecos = cos(camera->angle);
    camera->anglesin = sin(camera->angle);
    camera->sector = 1;

    struct vec2 new;
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);

    // create the engine window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "engine", NULL, NULL);
    if (!window)
    {
        // Window or OpenGL context creation failed
        fprintf(stderr, "Error: GLFW failed to create a window\n");
        glfwTerminate();
        exit(1);
    }
    glfwSwapInterval(1);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        process_input(window, camera, sectors, &new);

        if (update_location(camera, sectors, &new)) {
            camera->pos->x = new.x;
            camera->pos->y = new.y;
        }

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        memset(pixel_arr, 0, sizeof(float) * 2 * SCR_HEIGHT * SCR_WIDTH);  // reset pixel buffer

        double depth;
        int hit_id, hit_sector;
        bool is_vertex;
        for (int x = 0; x < SCR_WIDTH; x++) {
            struct ray *ray = viewing_ray(camera, x);

            int sector_index = camera->sector - 1;
            if (first_hit(ray, sectors[sector_index], sectors, FUDGE, &is_vertex, &depth, &hit_id, &hit_sector)) {
                struct wall *hit_wall = (sectors[hit_sector - 1]->walls)[hit_id];

                // calculate depth effect
                int line_height = (int) SCR_HEIGHT / depth;
                int y0 = max((SCR_HEIGHT / 2) - (line_height / 2), 0);
                int y1 = min((SCR_HEIGHT / 2) + (line_height / 2), SCR_HEIGHT - 1);

                // calculate colour based on angle to x-axis
                struct vec2 light = {1.5, 1.5};
                float lambertian_coeff = 0;
                lambertian_coeff += lambertian(ray, &light, depth, hit_wall, 0.2);
                lambertian_coeff += lambertian(ray, camera->pos, depth, hit_wall, min(0.4 / powf(depth, 2.0), 0.3));
                
                draw_vert(pixel_arr, x, y1, SCR_HEIGHT, 0.15, 1.0);
                if (is_vertex) {
                    draw_vert(pixel_arr, x, y0, y1, 1.0, 1.0);
                } else {
                    draw_vert(pixel_arr, x, y0, y1, max(AMBIENT + lambertian_coeff, 0.0), 1.0);
                }
            }

            destroy_ray(ray);
        }

        // TODO: apply dithering filter

        // draw pixels
        glDrawPixels(SCR_WIDTH, SCR_HEIGHT, GL_LUMINANCE_ALPHA, GL_FLOAT, pixel_arr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    destroy_sectors(sectors, &n_sectors);
    glfwTerminate();
    free(camera->pos);
    free(camera);
    return 0;
}
