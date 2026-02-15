#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "shader.h"
#include "camera.h"

#include <iostream>
#include <random>

// TODO:                                                                                                                                                  DONE:
// 1.  add transformations                                                                                                                                 x
// 2.  add background white wall                                                                                                                           x
// 3.  add camera, make it a fps camera                                                                                                                    x
// 4.  add a dot/crosshair in the center of our camera                                                                                                     x
// 5.  create the basic arena                                                                                                                              x
// 6.  on the wall, add randomly placed 10 targets                                                                                                         x
// 7.  after a mouse is clicked on the target, delete it and spawn a new one on the wall                                                                   x       first batch done

// 8.  Wall collision detection                                                                                                                            x
// 9.  Debug mode with faster movement speed, when in debug, add a second key that will display only vertices (q - debug, e - skeleton mode)               x
// 10. Scoring, lets say that for now in console                                                                                                           x                                                
//


const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int score = 0;

// starting mouse position
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool isFirstMouse = true;

// callback and input
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void ProcessInput(GLFWwindow* window);

// cube logic
glm::vec3 GenerateCubePosition(std::vector<glm::vec3>& cubePositions);
bool CubeTooClose(std::vector<glm::vec3>& cubePositions, glm::vec3 position);
bool TargetClickDetection(GLFWwindow* window, Camera& camera, std::vector<glm::vec3>& cubePositions);
unsigned int cubeToRemove;

// wall collision logic
void ManageWallCollision(Camera& camera);
bool isDebugMode = false;
bool isWireframe = false;

Camera camera;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Konraad's", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // disable mouse (make it cetered in the screen pos)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader cubeShader("resources/shaders/cubeShader.vs", "resources/shaders/cubeShader.fs");
    Shader crosshairShader("resources/shaders/crosshairShader.vs", "resources/shaders/crosshairShader.fs");
    Shader lightSourceShader("resources/shaders/lightCubeShader.vs", "resources/shaders/lightCubeShader.fs");

    // cubes
    float vertices[] {
        // 0-2 vertices, 3-5 normals (normals are unit vectors perpendicular to the surface of the vertex, can be also calculated using cross product)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    std::vector<glm::vec3> cubePositions {};

    for (int i = 0; i < 10; i++) {
        cubePositions.push_back(GenerateCubePosition(cubePositions));
    }

    std::vector<glm::vec3> wallPositions {
        glm::vec3(0.0f, 12.0f, -25.0f), // front
        glm::vec3(25.0f, 12.0f, 0.0f),  // right
        glm::vec3(0.0f, 12.0f, 25.0f),  // back
        glm::vec3(-25.0f, 12.0f, 0.0f), // left
        glm::vec3(0.0f, -3.0f, 0.0f)    // floor 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // light source
    glm::vec3 lightPos(0.0f, 10.0f, 0.0f);
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // crosshair
    float crosshairVertices[]{
        -0.5f, 0.5f,
        -0.5f,-0.5f,
         0.5f,-0.5f,
         0.5f,-0.5f,
        -0.5f, 0.5f,
         0.5f, 0.5f
    };

    unsigned int crosshairVBO, crosshairVAO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);

    glBindVertexArray(crosshairVAO);

    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    std::cout << "Score: " << score << std::endl;

	// render loop
	while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		ProcessInput(window);

        if (TargetClickDetection(window, camera, cubePositions)) {
            score += 10;
            std::cout << "Score: " << score << std::endl;
            cubePositions.erase(cubePositions.begin() + cubeToRemove);
            cubePositions.push_back(GenerateCubePosition(cubePositions));
        }

        if (isDebugMode) {
            if (isWireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            camera.MovementSpeed = 6.0f;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            ManageWallCollision(camera);
            camera.MovementSpeed = 3.5f;
        }
    

        // clear buffers
        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader program
        cubeShader.use();
        cubeShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);   
        cubeShader.setVec3("lightPos", lightPos);
        cubeShader.setVec3("viewPos", camera.Position);

        // boxes
        glBindVertexArray(VAO);

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(cubeShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection = glm::mat4(0.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(cubeShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        cubeShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
        cubeShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
        cubeShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        cubeShader.setFloat("material.shininess", 32.0f);
        cubeShader.setInt("flipNormal", 0);

        for (int i = 0; i < cubePositions.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);

            glUniformMatrix4fv(glGetUniformLocation(cubeShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // walls
        for (int i = 0; i < 4; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, wallPositions[i]);
            if (i % 2 != 0) {
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                cubeShader.setInt("flipNormal", 1);
            }
            else {
                cubeShader.setInt("flipNormal", 0);
            }

            model = glm::scale(model, glm::vec3(50.0f, 30.0f, 0.2f));

            cubeShader.setMat4("model", model);

            cubeShader.setVec3("material.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            cubeShader.setVec3("material.diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
            cubeShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            cubeShader.setFloat("material.shininess", 32.0f);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // floor
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, wallPositions[4]);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(50.0f, 50.0f, 0.2f));
        cubeShader.setMat4("model", model);

        cubeShader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        cubeShader.setFloat("material.shininess", 32.0f);

        cubeShader.setInt("flipNormal", 1);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // light source
        lightSourceShader.use();

        glBindVertexArray(lightVAO);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightSourceShader.setMat4("model", model);
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);

        glDrawArrays(GL_TRIANGLES, 0, 36);


        // crosshair
        glDisable(GL_DEPTH_TEST);
        crosshairShader.use();

        glBindVertexArray(crosshairVAO);
        projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, -1.0f, 1.0f); // range could be anything that includes 0.0f since the crosshair is placed at z=0 (eg. -0.1f, 0.1f)
        crosshairShader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(5.0f, 5.0f, 1.0f));
        crosshairShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
        glfwPollEvents();
	}

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &crosshairVAO);
    glDeleteBuffers(1, &crosshairVBO);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0 ,0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (isFirstMouse) {
        lastX = xpos;
        lastY = ypos;
        isFirstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos; 
    lastY = ypos;

    camera.MouseMovement(xoffset, yoffset);
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

    // movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.KeyboardMovement(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.KeyboardMovement(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.KeyboardMovement(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.KeyboardMovement(RIGHT, deltaTime);

    static bool qPressed = false;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !qPressed) {
        isDebugMode = !isDebugMode;
        qPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
        qPressed = false;

    static bool ePressed = false;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !ePressed) {
        isWireframe = !isWireframe;
        ePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
        ePressed = false;
}

glm::vec3 GenerateCubePosition(std::vector<glm::vec3>& cubePositions) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(-21.0f, 21.0f);
    std::uniform_real_distribution<float> disY(-11.0f, 11.0f);

    glm::vec3 position;
    do {
        float randomX = disX(gen);
        float randomY = disY(gen);
        position = glm::vec3(0.0f + randomX, 12.0f + randomY, -24.5f);
    } while (CubeTooClose(cubePositions, position));

    return position;
}

bool CubeTooClose(std::vector<glm::vec3>& cubePositions, glm::vec3 position) {
    for (glm::vec3& cubePos : cubePositions) {
        float distance = glm::distance(cubePos, position);
        // pitagoras:
        //float dx = cubePos.x - position.x;
        //float dy = cubePos.y - position.y;
        //float distance = sqrt(dx * dx + dy * dy);
        if (distance < 2.0f)
            return true;
    }
    return false;
}

bool TargetClickDetection(GLFWwindow* window, Camera& camera, std::vector<glm::vec3>& cubePositions) {
    for (int i = 0; i < cubePositions.size(); i++) {
        glm::vec3 cameraToCubeVector = cubePositions[i] - camera.Position;
        glm::vec3 cameraToCubeDirection = glm::normalize(cameraToCubeVector);

        bool mouseClicked = false;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            mouseClicked = true;
            cubeToRemove = i;

        if (glm::dot(cameraToCubeDirection, camera.Front) > 0.9999f && mouseClicked)
            return true;
    }

    return false;
}


std::vector<glm::vec3> wallPositions{
    glm::vec3(0.0f, 12.0f, -25.0f), // front
    glm::vec3(25.0f, 12.0f, 0.0f),  // right
    glm::vec3(0.0f, 12.0f, 25.0f),  // back
    glm::vec3(-25.0f, 12.0f, 0.0f), // left
    glm::vec3(0.0f, -3.0f, 0.0f)    // floor 
};

void ManageWallCollision(Camera& camera) {
    if (camera.Position.z < -24.0f)
        camera.Position.z = -24.0f;

    if (camera.Position.x > 24.0f)
        camera.Position.x = 24.0f;
    
    if (camera.Position.z > 24.0f)
        camera.Position.z = 24.0f;
    
    if (camera.Position.x < -24.0f)
        camera.Position.x = -24.0f;
}