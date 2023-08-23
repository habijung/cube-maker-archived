//
// Created by habi on 2023-08-21.
//

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "include/cube.h"
#include "include/shader.h"
#include "include/stb_image.h"
#include <iostream>
using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

float vertices[] = {
        // positions      // texture
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,  // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,// bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f  // top left
};
unsigned int indices[] = {
        0, 1, 3,// first triangle
        1, 2, 3 // second triangle
};
float texCoords[] = {
        0.0f, 0.0f,// lower-left corner
        1.0f, 0.0f,// lower-right corner
        0.5f, 1.0f // top-center corner
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Cube Maker", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Potision attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Shader
    const char *vert = "include/shader.vert";
    const char *frag = "include/shader.frag";
    Shader ourShader(vert, frag);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set the texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    const char *img = "include/wall.jpg";
    unsigned char *data = stbi_load(img, &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        cout << "Failed to load texture" << endl;
    }
    stbi_image_free(data);

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Projection matrix rarely changes there's no need to do this per frame
    ourShader.use();
    mat projection = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);

    // Rendering
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Rendering somethings
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create camera
        vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
        vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
        vec3 cameraDirection = normalize(cameraPos - cameraTarget);
        vec3 up = vec3(0.0f, 1.0f, 0.0f);
        vec3 cameraRight = normalize(cross(up, cameraDirection));
        vec3 cameraUp = cross(cameraDirection, cameraRight);

        // Render container
        ourShader.use();
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        // Camera, View transformation
        mat4 view = mat4(1.0f);
        const float radius = 10.0f;
        float camX = static_cast<float>(sin(glfwGetTime()) * radius);
        float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
        view = lookAt(glm::vec3(camX, 0.0, camZ),
                      glm::vec3(0.0, 0.0, 0.0),
                      glm::vec3(0.0, 1.0, 0.0));
        ourShader.setMat4("view", view);

        mat4 model = mat4(1.0f);
        float angle = 20.0f;
        model = translate(model, vec3(0.0f, 0.0f, 0.0f));
        model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        ourShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Optional
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
