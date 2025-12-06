#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <glfw3.h>
#include"Shader.h"
#include"Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/trigonometric.hpp"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 1680;
const unsigned int SCR_HEIGHT = 944;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool ispaused = false;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

struct Cube{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float size;
};
struct Light{
    glm::vec3 Position;
    glm::vec3 Color;
    float diffuseFactor;
    float ambientFactor;
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;
};
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    //glfwMaximizeWindow(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }   

    glEnable(GL_DEPTH_TEST);

    Shader axisShader("Shader/axis.vs","Shader/axis.fs");

    float axisVertices[] = {
        0.0f,0.0f,0.0f,     1.0f,0.0f,0.0f, // x-axis start
        1.0f,0.0f,0.0f,     1.0f,0.0f,0.0f, 
        0.0f,0.0f,0.0f,     0.0f,1.0f,0.0f,
        0.0f,1.0f,0.0f,     0.0f,1.0f,0.0f,
        0.0f,0.0f,0.0f,     0.0f,0.0f,1.0f,
        0.0f,0.0f,1.0f,     0.0f,0.0f,1.0f
    };


    unsigned int axisVAO,axisVBO;
    glGenVertexArrays(1,&axisVAO);
    glGenBuffers(1,&axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER,axisVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(axisVertices),axisVertices,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0 );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    Shader CubeShader("Shader/vertex.vs","Shader/fragment.fs");

    float cube[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

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

    unsigned int cubeVAO, cubeVBO;
    glGenBuffers(1,&cubeVBO);
    glGenVertexArrays(1,&cubeVAO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(cube),cube,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    

    Shader lightShader("Shader/light.vs","Shader/light.fs");
    unsigned int lightVAO;
    glGenVertexArrays(1,&lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0) ;


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 15.0f;

    bool showCube = true;
    Cube myCube{
        glm::vec3(0.0f,0.0f,0.0f), // Position 
        glm::vec4(1.0f,1.0f,1.0f,1.0f), // Color but doesn't work now
        glm::vec3(1.0f,0.5f,0.31f),// ambient 
        glm::vec3(1.0f,0.5f,0.31f),// diffuse
        glm::vec3(0.5f,0.5f,0.5f),// specular
        32.0f, // shininess
        1.0f // size
    };

    Light myLight{
        glm::vec3(1.2f,1.0f,2.0f),
        glm::vec3(1.0f,1.0f,1.0f),
        0.5f,
        0.2f
    };
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        axisShader.use();
        axisShader.setmat4("model", model)  ;
        axisShader.setmat4("view", view)  ;
        axisShader.setmat4("projection", projection)  ;

        axisShader.use();
        glBindVertexArray(axisVAO);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);



        model = glm::translate(model,myCube.Position);
        CubeShader.use();
        CubeShader.setFloat("size", myCube.size);
        CubeShader.setmat4("model", model)  ;
        CubeShader.setmat4("view", view)  ;
        CubeShader.setmat4("projection", projection)  ;
        CubeShader.setVec3("lightColor",myLight.Color);
        CubeShader.setVec3("viewPos",camera.Position);
        CubeShader.setVec3("material.ambient",myCube.ambient);
        CubeShader.setVec3("material.diffuse",myCube.diffuse);
        CubeShader.setVec3("material.specular",myCube.specular);
        CubeShader.setFloat("material.shininess", myCube.shininess);
        myLight.ambientColor = myLight.Color*glm::vec3(myLight.ambientFactor);
        myLight.diffuseColor = myLight.Color*glm::vec3(myLight.diffuseFactor);
        CubeShader.setVec3("light.ambient",myLight.ambientColor);
        CubeShader.setVec3("light.diffuse",myLight.diffuseColor);
        CubeShader.setVec3("light.specular",1.0f,1.0f,1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, myLight.Position);
        model = glm::scale(model,glm::vec3(0.2f));
        CubeShader.use();
        CubeShader.setVec3("lightPos",myLight.Position);
        if(showCube)
        {
        CubeShader.use();
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES,0,36);
        glBindVertexArray(0);

        }


        

        lightShader.use();
        lightShader.setmat4("model",model);
        lightShader.setmat4("view",view);
        lightShader.setmat4("projection",projection);
        lightShader.setVec3("lightColor", myLight.Color);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES,0,36);
         
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();  
        ImGui::NewFrame();

        if(ImGui::IsKeyPressed(ImGuiKey_Tab))
        {
            ispaused = !ispaused;
            if(!ispaused)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        if(ispaused)
        {   
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
            ImGui::Begin("Camera Setttings");
    
            ImGui::SeparatorText("Positions");

            ImGui::Checkbox("SHOW CUBE", &showCube);
            ImGui::End();


            if(showCube)
            {ImGui::Begin("Cube Proporties");
            
            ImGui::SliderFloat("Cube Size", &myCube.size, 1.0f, 10.0f);
            ImGui::DragFloat3("Cube Position", &myCube.Position.x);
            ImGui::DragFloat3("Ambient",&myCube.ambient.x);
            ImGui::DragFloat3("Diffuse",&myCube.diffuse.x);
            ImGui::DragFloat3("Specular",&myCube.specular.x);
            ImGui::DragFloat("Shininess",&myCube.shininess);
            ImGui::End();
            }


            ImGui::Begin("Light Properties");

            ImGui::ColorEdit3("Light Color", &myLight.Color.x);
            ImGui::SeparatorText("Light Position");
            ImGui::DragFloat3("Light Position", &myLight.Position.x);
            ImGui::DragFloat3("Light Ambient", &myLight.ambientColor.x);
            ImGui::DragFloat3("Light Diffuse", &myLight.diffuseColor.x);

            ImGui::Text("FPS: %.1f",ImGui::GetIO().Framerate);
            ImGui::End();

        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&axisVAO);
    glDeleteBuffers(1,&axisVBO);
    glDeleteBuffers(1,&cubeVBO);
    glDeleteVertexArrays(1,&cubeVAO);
    glDeleteVertexArrays(1,&lightVAO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
 if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    
    
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if(!ispaused)
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

