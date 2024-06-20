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
void process_input(GLFWwindow *window, struct camera *camera)
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
        camera->pos->x += MVTSPD * camera->anglecos;
        camera->pos->y += MVTSPD * camera->anglesin;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // go forward
        camera->pos->x -= MVTSPD * camera->anglecos;
        camera->pos->y -= MVTSPD * camera->anglesin;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // go left
        camera->pos->x -= MVTSPD * camera->anglesin;
        camera->pos->y += MVTSPD * camera->anglecos;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // go right
        camera->pos->x += MVTSPD * camera->anglesin;
        camera->pos->y -= MVTSPD * camera->anglecos;
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

    // TODO: load sectors and build sector and wall structs
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
    camera->angle = 0;
    camera->anglecos = cos(camera->angle);
    camera->anglesin = sin(camera->angle);
    camera->sector = 1;

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
        process_input(window, camera);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        memset(pixel_arr, 0, sizeof(float) * 2 * SCR_HEIGHT * SCR_WIDTH);  // reset pixel buffer

        double depth;
        int hit_id;
        bool is_vertex;
        for (int x = 0; x < SCR_WIDTH; x++) {
            struct ray *ray = viewing_ray(camera, x);

            int sector_index = camera->sector - 1;
            if (first_hit(ray, sectors[sector_index], sectors, 0, &is_vertex, &depth, &hit_id)) {
                // printf("hit %d\n", hit_id);
                struct wall *hit_wall = (sectors[sector_index]->walls)[hit_id];

                int line_height = (int) SCR_HEIGHT / depth;
                int y0 = max((SCR_HEIGHT / 2) - (line_height / 2), 0);
                int y1 = min((SCR_HEIGHT / 2) + (line_height / 2), SCR_HEIGHT - 1);

                if (is_vertex) {
                    draw_vert(pixel_arr, x, y0, y1, 1.0, 1.0);
                } else {
                    draw_vert(pixel_arr, x, y0, y1, 0.3, 1.0);
                }
                
            }

            destroy_ray(ray);
        }

        // TODO: apply dithering filter

        // draw pixels
        glPixelZoom(2, 2);
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
