// Source: https://github.com/JoeyDeVries/LearnOpenGL/tree/master/includes/learnopengl

#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


class Light
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec3 Intensity;
    
    //constructor
    Light(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)) // , glm::vec3 intensity = glm::vec3()
    {
        Position = position;
    }

private:
    glm::vec3 Color;
};
#endif