
#include <glad/glad.h>
#include <glfw/glfw3.h>
//#include <GL/freeglut.h>					// GLUT, includes glu.h and gl.h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>

#include "Headers/shader.h"
#include "Headers/filesystem.h"
#include "Headers/camera.h"
#include "Headers/model.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// global variables
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int activeShaderID = 0; // default - phong shader

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float currentFrame = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool paused = false;
float pausedFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main(int argc, char** argv)
{
	glfwInit();

	// tell GLFW to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// create window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shaders", NULL, NULL);
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

	// initialize glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);


	/// build and compile our shader program
	// ------------------------------------
	Shader phongShader("Shaders/Phong_lighting.vert", "Shaders/Phong_lighting.frag");
	Shader lightSourceShader("Shaders/Light_source.vert", "Shaders/Light_source.frag");
	Shader toonShader("Shaders/toon.vert", "Shaders/toon.frag");
	
	// load models
	// -----------
	//Model ourModel(FileSystem::getPath("Models/nanosuit/nanosuit.obj"));
	Model backpackModel(FileSystem::getPath("Models/Backpack/backpack.obj"));
	//Model ourModel(FileSystem::getPath("Models/Ravenors-Reading Corner/Ravenors-Reading Corner.obj"));

	float lightSourceVertices[] = { // cube
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	// configure light source cube VAO and VBO
	// ---------------------------------
	unsigned int VBO, lightSourceVAO;
	glGenVertexArrays(1, &lightSourceVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(lightSourceVAO);
	// load data into vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightSourceVertices), lightSourceVertices, GL_STATIC_DRAW);
	// set the vertex attribute pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	// configure uniform buffer objects for each shader program
	// ---------------------------------
	// first. We get the relevant block indices
	unsigned int uniformBlockIndexPhong = glGetUniformBlockIndex(phongShader.ID, "Matrices");
	unsigned int uniformBlockIndexToon = glGetUniformBlockIndex(toonShader.ID, "Matrices");
	unsigned int uniformBlockIndexLightSource = glGetUniformBlockIndex(lightSourceShader.ID, "Matrices");
	// then we link each shader's uniform block to this uniform binding point
	glUniformBlockBinding(phongShader.ID, uniformBlockIndexPhong, 0);
	glUniformBlockBinding(toonShader.ID, uniformBlockIndexToon, 0);
	glUniformBlockBinding(lightSourceShader.ID, uniformBlockIndexLightSource, 0);
	// Now actually create the buffer
	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	// store the projection matrix (we only do this once now)
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// render
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);	// Set background color to black and opaque
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set the view matrix in the uniform block - we only have to do this once per loop iteration.
		glm::mat4 view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f));
		//glm::mat3 normalMatrix = mat3(transpose(inverse(model)));

		// activate shader before setting uniforms
		switch (activeShaderID)
		{
		case 1: // toon shader
			toonShader.use();
			// set uniforms
			toonShader.setMat4("model", model);
			toonShader.setVec3("lightPos", lightPos);
			toonShader.setVec3("viewPos", camera.Position);
			backpackModel.Draw(toonShader);
			break;

		case 2: // gooch shader
		case 3: // hatching shader
		default: // phong shader
			phongShader.use();
			// set uniforms
			phongShader.setMat4("model", model);
			phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			phongShader.setVec3("lightPos", lightPos);
			phongShader.setVec3("viewPos", camera.Position);
			backpackModel.Draw(phongShader);
		}

		// render the light source
		lightSourceShader.use();
		/*lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setMat4("view", view);*/
		// rotate light around y axis of the displayed object at the origin
		const float radius = 3.0f;
		if (!paused)
		{
			lightPos.x = sin(currentFrame / 1.5f) * radius;
			lightPos.z = cos(currentFrame / 1.5f) * radius;
		}
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightSourceShader.setMat4("model", model);
		glBindVertexArray(lightSourceVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// deallocate all resources
	glDeleteVertexArrays(1, &lightSourceVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &uboMatrices);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}


// resize window callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
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
	{
		if (paused == false)
		{
			paused = true;
			pausedFrame = lastFrame;
		}
		else
		{
			paused = false;
			glfwSetTime(pausedFrame);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		activeShaderID = 1;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		activeShaderID = 4;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
