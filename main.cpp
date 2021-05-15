
#include <glad/glad.h>
#include <glfw/glfw3.h>
//#include <GL/freeglut.h>					// GLUT, includes glu.h and gl.h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include "Headers/shader.h"
#include "Headers/filesystem.h"
#include "Headers/camera.h"
#include "Headers/model.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processMovement(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void configureFBO(GLuint* FBO, vector<GLuint*>* renderTargets, bool multiSample, bool depthOrStencil);


// global variables
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int numSamples = 4;
int activeShaderID = 0; // default - phong shader
bool displayNormals = false;
bool texturesToggle = true;
std::vector<Model*> modelsList;
int vectorIndex = 0;
Model* modelToRender;

// camera
Camera camera(glm::vec3(0.0f, 1.5f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float currentFrame = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool paused = false;
float pausedFrame = 0.0f;
float rotationTime = 0.0f;

// lighting
glm::vec3 lightPos(3.0f, 4.0f, 2.0f);


int main(int argc, char** argv)
{
	glfwInit();

	// tell GLFW to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, numSamples);

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
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

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
	glEnable(GL_MULTISAMPLE);


	/// build and compile our shader program
	// ------------------------------------
	Shader phongShader("Shaders/default.vert", "Shaders/PhongLighting.frag");
	Shader lightSourceShader("Shaders/LightSource.vert", "Shaders/LightSource.frag");
	Shader toonShader("Shaders/default.vert", "Shaders/ToonShading.frag");
	Shader goochShader("Shaders/default.vert", "Shaders/GoochShading.frag");
	//Shader freiChenShader("Shaders/FreiChenFilter.vert", "Shaders/FreiChenFilter.frag");
	Shader sobelShader("Shaders/RenderQuad.vert", "Shaders/SobelOutline.frag");
	Shader normalShader("Shaders/DisplayNormals.vert", "Shaders/DisplayNormals.frag", "Shaders/DisplayNormals.geom");
	
	// load models
	// -----------
	//Model ourModel(FileSystem::getPath("Models/nanosuit/nanosuit.obj"));
	Model teapotModel(FileSystem::getPath("Models/teapot.obj"));
	Model backpackModel(FileSystem::getPath("Models/Backpack/backpack.obj"));
	//Model ourModel(FileSystem::getPath("Models/Ravenors-Reading Corner/Ravenors-Reading Corner.obj"));
	modelsList.push_back(&teapotModel);
	modelsList.push_back(&backpackModel);

	modelToRender = modelsList[0];

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
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	// configure light source cube VAO and VBO
	// ---------------------------------
	GLuint VBO, lightSourceVAO;
	glGenVertexArrays(1, &lightSourceVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(lightSourceVAO);
	// load data into vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightSourceVertices), lightSourceVertices, GL_STATIC_DRAW);
	// set the vertex attribute pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	// screen quad VAO and VBO
	// ---------------------------------
	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// configure uniform buffer objects for each shader program
	// ---------------------------------
	// first. We get the relevant block indices
	GLuint uniformBlockIndexPhong = glGetUniformBlockIndex(phongShader.ID, "Matrices");
	GLuint uniformBlockIndexToon = glGetUniformBlockIndex(toonShader.ID, "Matrices");
	GLuint uniformBlockIndexLightSource = glGetUniformBlockIndex(lightSourceShader.ID, "Matrices");
	// then we link each shader's uniform block to this uniform binding point
	glUniformBlockBinding(phongShader.ID, uniformBlockIndexPhong, 0);
	glUniformBlockBinding(toonShader.ID, uniformBlockIndexToon, 0);
	glUniformBlockBinding(lightSourceShader.ID, uniformBlockIndexLightSource, 0);
	// Now actually create the buffer
	GLuint uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	// store the projection matrix (we only have to do this once)
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// configure frame buffer objects
	// ---------------------------------
	GLuint FBO, imageTextureMS, normalTextureMS, depthTextureMS;
	vector<GLuint*> renderTargets; // G buffers
	glGenTextures(1, &imageTextureMS);
	renderTargets.push_back(&imageTextureMS);
	glGenTextures(1, &normalTextureMS);
	renderTargets.push_back(&normalTextureMS);
	glGenTextures(1, &depthTextureMS);
	renderTargets.push_back(&depthTextureMS);
	//FBO = prepareFBO(renderTargets, true, true);
	configureFBO(&FBO, &renderTargets, true, true);

	GLuint intermediateFBO, imageTexture, normalTexture, depthTexture;
	vector<GLuint*> renderTargets2;
	glGenTextures(1, &imageTexture);
	renderTargets2.push_back(&imageTexture);
	glGenTextures(1, &normalTexture);
	renderTargets2.push_back(&normalTexture);
	glGenTextures(1, &depthTexture);
	renderTargets2.push_back(&depthTexture);
	//intermediateFBO = prepareFBO(renderTargets2, false, false);
	configureFBO(&intermediateFBO, &renderTargets2, false, false);


	// shader configuration
	// ---------------------------------
	/*freiChenShader.use();
	freiChenShader.setInt("textureImage", 0);
	freiChenShader.setInt("textureNormal", 1);
	freiChenShader.setInt("textureDepth", 2);*/
	sobelShader.use();
	sobelShader.setInt("imageTexture", 0);
	sobelShader.setInt("normalTexture", 1);
	sobelShader.setInt("depthTexture", 2);



	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//processInput(window, &modelToRender);
		processMovement(window);

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
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f));
		if (!paused)
			rotationTime = (float)glfwGetTime();
		modelMatrix = glm::rotate(modelMatrix, rotationTime, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat3 normalMatrix = glm::mat3(transpose(inverse(modelMatrix)));

		// assign render targets for FBO
		GLuint GBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

		// activate shader before setting uniforms
		switch (activeShaderID)
		{
		case 1: // toon shader
			toonShader.use();
			// set uniforms
			toonShader.setMat4("model", modelMatrix);
			toonShader.setMat3("normalMatrix", normalMatrix);
			toonShader.setVec3("lightPos", lightPos);
			toonShader.setVec3("viewPos", camera.Position);
			toonShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			toonShader.setBool("texturesToggle", texturesToggle);
			// render model
			modelToRender->Draw(toonShader);
			break;

		case 2: // Gooch shader
			// enable draw buffers for intermediateFBO
			// this is really important for the blit operation to copy into ALL color buffers
			glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
			glDrawBuffers(3, GBuffers);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, FBO);
				glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

				// Before rendering the first pass
				// clear the image's render target to white
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				// clear the normals' render target to a vector facing away from the camera
				glDrawBuffer(GL_COLOR_ATTACHMENT1);
				glm::vec3 clearVec(0.0f, 0.0f, -1.0f);
				// from normalized vector to rgb color; from [-1,1] to [0,1]
				clearVec = (clearVec + glm::vec3(1.0f, 1.0f, 1.0f)) * 0.5f;
				glClearColor(clearVec.x, clearVec.y, clearVec.z, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				// clear the depth's render target to black
				glDrawBuffer(GL_COLOR_ATTACHMENT2);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				// now enable all render targets for drawing
				glDrawBuffers(3, GBuffers);
				glClear(GL_DEPTH_BUFFER_BIT);

				// First pass: Render the model using Gooch shading, and render the camera-space normals and fragment depths to the other render targets
				goochShader.use();
				// set uniforms
				goochShader.setMat4("model", modelMatrix);
				goochShader.setMat3("normalMatrix", normalMatrix);
				goochShader.setVec3("lightPos", lightPos);
				goochShader.setVec3("viewPos", camera.Position);
				goochShader.setVec3("coolColor", 0.0f, 0.0f, 0.55f);
				goochShader.setVec3("warmColor", 0.3f, 0.3f, 0.0f);
				goochShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
				goochShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
				goochShader.setFloat("specularStrength", 0.5f);
				goochShader.setFloat("alpha", 0.25f);
				goochShader.setFloat("beta", 0.5f);
				// render model
				modelToRender->Draw(goochShader);

				// now blit multisampled buffer(s) to normal colorbuffer of intermediateFBO
				glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
				glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
				

			// Second pass: Do a full-screen edge detection filter over the normals from the first pass and draw feature edges
			// bind back to default framebuffer and draw a quad plane with the attached framebuffer color textures
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
				//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
				// clear all relevant buffers
				// set background to white to be able to see rendered outline
				//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				sobelShader.use();
				glBindVertexArray(quadVAO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, imageTexture);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, normalTexture);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
			
				// set uniforms
				//freiChenShader.setFloat("edgeThreshold", 0.03f);
				//sobelShader.setFloat("edgeThreshold", 0.03f);
				sobelShader.setFloat("_OutlineThickness", 1.0f);
				sobelShader.setFloat("_OutlineDepthMultiplier", 1.0f);
				sobelShader.setFloat("_OutlineDepthBias", 1.0f);
				sobelShader.setFloat("_OutlineNormalMultiplier", 1.0f);
				sobelShader.setFloat("_OutlineNormalBias", 10.0f);
				sobelShader.setVec4("_OutlineColor", 0.0f, 0.0f, 0.0f, 1.0f);

				// finally render quad
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				//teapotModel.Draw(sobelShader);
				glBindVertexArray(0);
			break;

		case 3: // hatching shader
		default: // Phong shader
			texturesToggle = false;
			phongShader.use();
			// set uniforms
			phongShader.setMat4("model", modelMatrix);
			phongShader.setMat3("normalMatrix", normalMatrix);
			phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			phongShader.setVec3("lightPos", lightPos);
			phongShader.setVec3("objectColor", 1.0f, 0.5f, 0.5f);
			phongShader.setVec3("viewPos", camera.Position);
			phongShader.setBool("texturesToggle", texturesToggle);
			// render model
			modelToRender->Draw(phongShader);
		}

		if (displayNormals)
		{
			// draw model with normal visualizing geometry shader
			normalShader.use();
			normalShader.setMat4("projection", projection);
			normalShader.setMat4("view", view);
			normalShader.setMat4("model", modelMatrix);
			// render model
			modelToRender->Draw(normalShader);
		}

		// render the light source
		lightSourceShader.use();
		// rotate light around y axis of the displayed object at the origin
		/*const float radius = 4.0f;
		if (!paused)
		{
			lightPos.x = sin(currentFrame / 1.5f) * radius;
			lightPos.z = cos(currentFrame / 1.5f) * radius;
		}*/
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, lightPos);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f)); // a smaller cube
		lightSourceShader.setMat4("model", modelMatrix);
		glBindVertexArray(lightSourceVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// deallocate all resources
	//glDeleteTextures(3, renderTargets);
	glDeleteVertexArrays(1, &lightSourceVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &uboMatrices);
	glDeleteFramebuffers(1, &FBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}


// resize window callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processMovement(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}


//void processInput(GLFWwindow* window, Model** modelToRender)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
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

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) // toon shading
	{
		activeShaderID = 1;
		//texturesToggle = true;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) // Gooch shading
	{
		activeShaderID = 2;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) // Phong shading
	{
		activeShaderID = 4;
		texturesToggle = true;
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS) // toggle textures
		texturesToggle = !texturesToggle;
	if (key == GLFW_KEY_6 && action == GLFW_PRESS) // toggle normals displayed
		displayNormals = !displayNormals;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		vectorIndex++;

		if (vectorIndex > modelsList.size() - 1)
			vectorIndex = 0;

		modelToRender = modelsList[vectorIndex];
	}
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

// Source: https://www.lighthouse3d.com/tutorials/opengl_framebuffer_objects/
// ----------------------------------------------------------------------
void configureFBO(GLuint* FBO, vector<GLuint*>* renderTargets, bool multiSample, bool depthOrStencil) {
	//GLuint FBO;
	glGenFramebuffers(1, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

	// get default textures
	int width, height, nrChannels;
	unsigned char* whiteTexture = stbi_load("Textures/white.png", &width, &height, &nrChannels, 0);
	unsigned char* blackTexture = stbi_load("Textures/black.png", &width, &height, &nrChannels, 0);

	// generate texture buffers
	for (int i = 0; i < (*renderTargets).size(); i++)
	{
		if (multiSample)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *(*renderTargets)[i]);

			//if (i == 0) // if image texture
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
			//else
			//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, blackTexture);

			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, *(*renderTargets)[i], 0);

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, *(*renderTargets)[i]);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, whiteTexture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, *(*renderTargets)[i], 0);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
	}

	if (depthOrStencil)
	{
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);

		// use a single renderbuffer object for both a depth AND stencil buffer
		if (multiSample)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	}
	
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//return FBO;
}