#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdio.h>
#include <stdlib.h>

void process_input(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

int main(int argc, char *argv[]) {
    if (!glfwInit()) {
        fprintf(stderr, "Error: GLFW failed to initialize\n");
        exit(1);
    }

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    printf("Running against GLFW %i.%i.%i\n", major, minor, revision);

    GLFWwindow* window = glfwCreateWindow(800, 600, "engine", NULL, NULL);
    if (!window)
    {
        // Window or OpenGL context creation failed
        fprintf(stderr, "Error: GLFW failed to create a window\n");
        glfwTerminate();
        exit(1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
