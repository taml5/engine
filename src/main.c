#ifndef GAME
#define GAME
#include "game.h"
#endif
#include "graphics.h"

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

    // TODO: initialise game struct

    // TODO: load sectors and build sector and wall structs
    struct wall test_wall;
    test_wall.left = (struct vec2i) {0, 3};
    test_wall.right = (struct vec2i) {3, 0};

    // initialise camera
    struct camera *camera = malloc(sizeof(struct camera));
    camera->pos = malloc(sizeof(struct vec2));
    camera->pos->x = 0;
    camera->pos->y = 0;
    camera->angle = 1.5 * PI;
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

        double depth;
        for (int x = 0; x < SCR_WIDTH; x++) {
            glBegin(GL_LINES);
                glColor3f(0.3, 0.3, 0.3);
                glVertex2f(2.0f * (x + 0.5f) / SCR_WIDTH - 1.0f, 1.0);
                glVertex2f(2.0f * (x + 0.5f) / SCR_WIDTH - 1.0f, 0.0);
                glColor3f(0.1, 0.1, 0.1);
                glVertex2f(2.0f * (x + 0.5f) / SCR_WIDTH - 1.0f, 0.0);
                glVertex2f(2.0f * (x + 0.5f) / SCR_WIDTH - 1.0f, -1.0);
            glEnd();
            struct ray *ray = viewing_ray(camera, x);
            
            if ((depth = intersection(ray, &test_wall)) > 0) { 
                // DRAW THE VERTICAL LINE
                int line_height = (int) SCR_HEIGHT / depth;
                int y0 = max((SCR_HEIGHT / 2) - (line_height / 2), 0);
                int y1 = min((SCR_HEIGHT / 2) + (line_height / 2), SCR_HEIGHT - 1);

                glBegin(GL_LINES);
                    glColor3f(0, 0.4, 0.8);
                    glVertex2f(2.0f * (x + 0.5f) / SCR_WIDTH - 1.0f, 2.0f * (y0 + 0.5f) / SCR_HEIGHT - 1.0f);
                    glVertex2f(2.0f * (x + 0.5f) / SCR_WIDTH - 1.0f, 2.0f * (y1 + 0.5f) / SCR_HEIGHT - 1.0f);
                glEnd();
            }

            destroy_ray(ray);
        }

        // TODO: apply dithering filter

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    free(camera->pos);
    free(camera);
    return 0;
}
