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
    int fps = 0;
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
    float *pixel_arr = malloc(sizeof(float) * SCR_WIDTH * SCR_HEIGHT * 3);

    // initialise camera
    struct camera *camera = malloc(sizeof(struct camera));
    camera->pos = malloc(sizeof(struct vec2));
    camera->pos->x = 2.0;
    camera->pos->y = 2.0;
    camera->angle = PI;
    camera->anglecos = cos(camera->angle);
    camera->anglesin = sin(camera->angle);
    camera->sector = 1;
    camera->height = CAM_Z + sectors[camera->sector -1]->floor_z;

    struct vec2 new = {camera->pos->x, camera->pos->y};
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

        // update the player's location
        if (update_location(camera, sectors, &new, 0)) {
            camera->pos->x = new.x;
            camera->pos->y = new.y;
        }

        /* Render here */
        for (int x = 0; x < SCR_WIDTH; x++) {
            struct ray *ray = viewing_ray(camera, x);
            render(pixel_arr, camera, sectors, ray, x, camera->sector, FUDGE, 0);
            destroy_ray(ray);
        }

        // draw pixels
        glDrawPixels(SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_FLOAT, pixel_arr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        fps++;
    }

    destroy_sectors(sectors, &n_sectors);
    glfwTerminate();
    free(camera->pos);
    free(camera);
    free(pixel_arr);

    printf("frames=%d\n", fps);
    return 0;
}
