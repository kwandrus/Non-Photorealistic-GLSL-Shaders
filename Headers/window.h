
#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "render_text.h"
#include "camera.h"
#include "model.h"

class CameraInfo
{
private:
	Camera* camera;
	float lastX;
	float lastY;

public:
	CameraInfo()
	{
		camera = new Camera(0.0f, 5.0f, 10.0f, 0.0f, 1.0f, 0.0f, -90.0f, -20.0f);
		lastX = -1;
		lastY = -1;
	}
	~CameraInfo()
	{
		delete camera;
	}

	Camera* getCamera()
	{
		return camera;
	}
	float getLastX()
	{
		return lastX;
	}
	float getLastY()
	{
		return lastY;
	}

	void setLastX(float x)
	{
		lastX = x;
	}
	void setLastY(float y)
	{
		lastY = y;
	}
};


class TimingInfo
{
private:
	float currentFrame;
	float deltaTime;
	float lastFrame;
	bool rotationPaused;
	float rotationTime;

public:
	TimingInfo()
	{
		currentFrame = 0.0f;
		deltaTime = 0.0f;
		lastFrame = 0.0f;
		rotationPaused = true;
		rotationTime = 0.0f;
	}
	~TimingInfo() { }

	float getCurrentFrame()
	{
		return currentFrame;
	}
	float getDeltaTime()
	{
		return deltaTime;
	}
	float getLastFrame()
	{
		return lastFrame;
	}
	bool isRotationPaused()
	{
		return rotationPaused;
	}
	float getRotationTime()
	{
		return rotationTime;
	}

	void setCurrentFrame(float frame)
	{
		currentFrame = frame;
	}
	void setDeltaTime()
	{
		deltaTime = currentFrame - lastFrame;
	}
	void setLastFrame(float frame)
	{
		lastFrame = frame;
	}
	void setRotationPaused(bool pause)
	{
		rotationPaused = pause;
	}
	void setRotationTime(float time)
	{
		rotationTime = time;
	}
};


class LightingInfo
{
private:
	const glm::vec3 LIGHTPOS = glm::vec3(3.0f, 4.0f, 2.64575f);
	float currentLightSourcePosition; // along the light's path circumference
	float lightSourceVelocity;
	float lightPathRadius;
	float directionFlip;
	bool lightPaused;

public:
	LightingInfo()
	{
		currentLightSourcePosition = 1.69612f;
		lightSourceVelocity = 2.0f;
		lightPathRadius = 4.0f;
		directionFlip = 1.0f;
		lightPaused = true;
	}
	~LightingInfo() { }

	glm::vec3 getLIGHTPOS()
	{
		return LIGHTPOS;
	}
	float getCurrentLightSourcePosition() // gets position of light at the moment it was paused
	{
		return currentLightSourcePosition;
	}
	float getLightSourceVelocity()
	{
		return lightSourceVelocity;
	}
	float getLightPathRadius()
	{
		return lightPathRadius;
	}
	float getDirectionFlip()
	{
		return directionFlip;
	}
	bool isLightPaused()
	{
		return lightPaused;
	}

	void setCurrentLightSourcePosition(float pos)
	{
		currentLightSourcePosition = pos;
	}
	void setLightSourceVelocity(float velocity)
	{
		lightSourceVelocity = velocity;
	}
	void setLightPathRadius(float radius)
	{
		lightPathRadius = radius;
	}
	void setDirectionFlip(float flipValue)
	{
		directionFlip = flipValue;
	}
	void setLightPaused(bool paused)
	{
		lightPaused = paused;
	}
};

class Window
{
private:	
	GLFWwindow* glfwWindow;
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;
	int numSamples;
	int activeShaderID; // default - phong shader
	bool normalsDisplaySetting;
	std::vector<Model*> modelsList;
	int activeModelIndex;

	CameraInfo* cameraInfo;
	TimingInfo* timingInfo;
	LightingInfo* lightingInfo;

public:
	std::map<GLchar, Character> screenText;

	Window()
	{
		glfwWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shaders", NULL, NULL);
		glfwSetWindowUserPointer(glfwWindow, this);
		numSamples = 4;
		activeShaderID = 0;
		normalsDisplaySetting = false;
		activeModelIndex = 0;

		cameraInfo = new CameraInfo();
		timingInfo = new TimingInfo();
		lightingInfo = new LightingInfo();
	}
	~Window()
	{
		delete cameraInfo;
		delete timingInfo;
		delete lightingInfo;
	}

	GLFWwindow* getGLFWWindow()
	{
		return glfwWindow;
	}
	CameraInfo* getCameraInfo()
	{
		return cameraInfo;
	}
	TimingInfo* getTimingInfo()
	{
		return timingInfo;
	}
	LightingInfo* getLightingInfo()
	{
		return lightingInfo;
	}
	int getWidth()
	{
		return SCR_WIDTH;
	}
	int getHeight()
	{
		return SCR_HEIGHT;
	}
	int getNumSamples()
	{
		return numSamples;
	}
	int getActiveShaderID()
	{
		return activeShaderID;
	}
	bool getNormalsDisplaySetting()
	{
		return normalsDisplaySetting;
	}
	Model* getModelToRender()
	{
		return modelsList[activeModelIndex];
	}

	void setActiveShaderID(int ID)
	{
		activeShaderID = ID;
	}
	void flipNormalsDisplaySetting()
	{
		normalsDisplaySetting = !normalsDisplaySetting;
	}
	void addModel(Model* model)
	{
		modelsList.push_back(model);
	}
	void setActiveModelIndex(int index)
	{
		activeModelIndex = index;
	}
};

#endif

