// Based on templates from learnopengl.com
#include "helperFunctions.h"
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <math.h>
#include <iostream>
#include <fstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadShaders(string vertex_fname, string frag_fname);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // glew: load all OpenGL function pointers
    glewInit();

    // load data shader
    unsigned int dataShader = loadShaders("shaders/data.vs", "shaders/data.fs");

    // load water shader
    unsigned int waterShader = loadShaders("shaders/water.vs", "shaders/water.fs");

    // construct water
    float amplitude = 0.01;
    float Lx = 10.0;
    float Lz = 10.0;
    int M = 64;
    int N = 64;
    glm::vec2 wind_vector(2.0, 0.0);
    water_grid water(amplitude, Lx, Lz, M, N, wind_vector);
    float time = 0.0;
    float fps = 30.0;
    float delta_time = 1.0 / fps;
    vector<Triangle> triangles = water.gen_triangles();
    int numBytesData = triangles.size() * sizeof(triangles[0]);
    int vertexSizeData = sizeof(triangles[0].vertex1);

    // construct projected grid
    int nx = 64;
    int ny = 64;
    float xMin = -100.0;
    float xMax = 100.0;
    float zMin = -100.0; 
    float zMax = 100.0;
    ProjectedGrid proj_grid(nx, ny, xMin, xMax, zMin, zMax);
    int numBytesWater = proj_grid.triangles.size() * 3 * sizeof(float);
    int vertexSizeWater = 3*sizeof(float);

    // construct projector matrix
    float fov = 60.0f * M_PI / 180.0f;
    float aspect = (float) SCR_WIDTH / SCR_HEIGHT;
    float znear = 0.2;
    float zfar = 100.0;
    glm::vec3 eye = glm::vec3(-8.0, 3.0, 0.0);
    glm::mat4 lookAt = glm::lookAt(eye, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projMatrix = glm::perspective(fov, aspect, znear, zfar);
    glm::mat4 transform = projMatrix * lookAt;

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.6f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // update geometry
        water.eval_grids(time);
        triangles = water.gen_triangles();

        // create frame buffer object
        unsigned int fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // create texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // attach texture to frame buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        // make sure frame buffer object completed
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        // create data buffer and attribute array
        unsigned int dataVBO, dataVAO;
        glGenVertexArrays(1, &dataVAO);
        glGenBuffers(1, &dataVBO);
        glBindBuffer(GL_ARRAY_BUFFER, dataVBO);
        glBufferData(GL_ARRAY_BUFFER, numBytesData, triangles.data(), GL_STATIC_DRAW);
        glBindVertexArray(dataVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSizeData, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSizeData, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // initialize shader program params
        glUseProgram(dataShader);
        glm::mat4 projMatrix = glm::ortho(-Lx/2, Lx/2, -Lz/2, Lz/2, -10.0f, 10.0f);
        glm::mat4 transformMatrix = projMatrix;
        GLint pMatID = glGetUniformLocation(dataShader, "transformMatrix");
        GLint minID = glGetUniformLocation(dataShader, "miny");
        GLint maxID = glGetUniformLocation(dataShader, "maxy");
        GLint minSlopeXID = glGetUniformLocation(dataShader, "minSlopeX");
        GLint maxSlopeXID = glGetUniformLocation(dataShader, "maxSlopeX");
        GLint minSlopeZID = glGetUniformLocation(dataShader, "minSlopeZ");
        GLint maxSlopeZID = glGetUniformLocation(dataShader, "maxSlopeZ");
        glUniformMatrix4fv(pMatID, 1, GL_FALSE, glm::value_ptr(transformMatrix));
        glUniform1f(minID, water.min);
        glUniform1f(maxID, water.max);
        glUniform1f(maxSlopeXID, water.max_slope_x);
        glUniform1f(minSlopeXID, water.min_slope_x);
        glUniform1f(maxSlopeZID, water.max_slope_z);
        glUniform1f(minSlopeZID, water.min_slope_z);

        // write data to texture
        glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // draw water on projected grid
        unsigned int waterVBO, waterVAO;
        glGenVertexArrays(1, &waterVAO);
        glGenBuffers(1, &waterVBO);
        glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
        glBufferData(GL_ARRAY_BUFFER, numBytesWater, proj_grid.triangles.data(), GL_STATIC_DRAW);
        glBindVertexArray(waterVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSizeWater, (void*)0);
        glEnableVertexAttribArray(0);

        // initialize shader program params
        glUseProgram(waterShader);
        GLint LxID = glGetUniformLocation(waterShader, "Lx");
        GLint LzID = glGetUniformLocation(waterShader, "Lz");
        GLint transformID = glGetUniformLocation(waterShader, "transform");
        GLint eyeID = glGetUniformLocation(waterShader, "eye");
        minID = glGetUniformLocation(waterShader, "minY");
        maxID = glGetUniformLocation(waterShader, "maxY");
        minSlopeXID = glGetUniformLocation(waterShader, "minSlopeX");
        maxSlopeXID = glGetUniformLocation(waterShader, "maxSlopeX");
        minSlopeZID = glGetUniformLocation(waterShader, "minSlopeZ");
        maxSlopeZID = glGetUniformLocation(waterShader, "maxSlopeZ");
        glUniform3f(eyeID, eye.x, eye.y, eye.z);
        glUniformMatrix4fv(transformID, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform1f(LxID, water.Lx);
        glUniform1f(LzID, water.Lz);
        glUniform1f(minID, water.min);
        glUniform1f(maxID, water.max);
        glUniform1f(maxSlopeXID, water.max_slope_x);
        glUniform1f(minSlopeXID, water.min_slope_x);
        glUniform1f(maxSlopeZID, water.max_slope_z);
        glUniform1f(minSlopeZID, water.min_slope_z);
        
        // switch shader and bind texture
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, proj_grid.triangles.size());
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        time += delta_time;
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

unsigned int loadShaders(string vertex_fname, string frag_fname) {
    string vertexShaderSourceString = readFile(vertex_fname);
    string fragmentShaderSourceString = readFile(frag_fname);
    char* vertexShaderSource = &vertexShaderSourceString[0];
    char* fragmentShaderSource = &fragmentShaderSourceString[0];

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}