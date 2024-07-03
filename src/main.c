#ifndef GAME
#define GAME
#include "game.h"
#endif
#include "graphics.h"
#include "load.h"

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

    // load textures
    int n_textures = 1;
    texture *textures = load_textures("./", n_textures);  // TODO: add correct filepath

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
    camera->height = CAM_Z + sectors[camera->sector]->floor_z;

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
            render(pixel_arr, camera, sectors, textures, ray, x, camera->sector, FUDGE, 0);
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

    glfwTerminate();
    destroy_sectors(sectors, n_sectors);
    destroy_textures(textures, n_textures);
    free(camera->pos);
    free(camera);
    free(pixel_arr);

    printf("frames=%d\n", fps);
    return 0;
}
