
#include <glad/glad.h>
#include <glfw/glfw3.h>
//#include <GL/freeglut.h>					// GLUT, includes glu.h and gl.h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#include "Headers/window.h"
#include "Headers/render_text.h"
#include "Headers/shader.h"
#include "Headers/filesystem.h"
#include "Headers/camera.h"
#include "Headers/model.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processMovement(GLFWwindow* window, CameraInfo* cameraInfo, TimingInfo* timingInfo);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void configureFBO(Window* window, GLuint* FBO, vector<GLuint*>* textures, bool multisample, bool mipmap, bool depthOrStencil);



int main(int argc, char** argv)
{
	glfwInit();

	Window* window = new Window();

	// tell GLFW to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, window->getNumSamples());

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// create window
	//GLFWwindow* window = glfwCreateWindow(instance->getWidth(), instance->getHeight(), "Shaders", NULL, NULL);
	if (window->getGLFWWindow() == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window->getGLFWWindow());
	glfwSetFramebufferSizeCallback(window->getGLFWWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(window->getGLFWWindow(), mouse_callback);
	glfwSetScrollCallback(window->getGLFWWindow(), scroll_callback);
	glfwSetKeyCallback(window->getGLFWWindow(), key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window->getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetInputMode(window->getGLFWWindow(), GLFW_STICKY_KEYS, GLFW_TRUE);

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	/// build and compile our shader program
	// ------------------------------------
	Shader phongShader("Shaders/default.vert", "Shaders/PhongLighting.frag");
	Shader lightSourceShader("Shaders/LightSource.vert", "Shaders/LightSource.frag");
	Shader toonShader("Shaders/default.vert", "Shaders/ToonShading.frag");
	Shader goochShader("Shaders/default.vert", "Shaders/GoochShading.frag");
	//Shader freiChenShader("Shaders/FreiChenFilter.vert", "Shaders/FreiChenFilter.frag");
	Shader sobelShader("Shaders/RenderQuad.vert", "Shaders/SobelOutline.frag");
	Shader normalShader("Shaders/DisplayNormals.vert", "Shaders/DisplayNormals.frag", "Shaders/DisplayNormals.geom");
	Shader hatchingShader("Shaders/default.vert", "Shaders/Hatching.frag");
	Shader renderTextShader("Shaders/RenderText.vert", "Shaders/RenderText.frag");
	
	// load models
	// -----------
	//Model ourModel(FileSystem::getPath("Models/nanosuit/nanosuit.obj"));
	Model teapotModel(FileSystem::getPath("Models/teapot3.obj"));
	Model backpackModel(FileSystem::getPath("Models/Backpack/backpack.obj"));
	Model bunnyModel(FileSystem::getPath("Models/StanfordBunny.obj"));
	//Model ourModel(FileSystem::getPath("Models/Ravenors-Reading Corner/Ravenors-Reading Corner.obj"));

	window->addModel(&teapotModel);
	window->addModel(&backpackModel);
	window->addModel(&bunnyModel);

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
	GLuint lightSourceVBO, lightSourceVAO;
	glGenVertexArrays(1, &lightSourceVAO);
	glGenBuffers(1, &lightSourceVBO);
	glBindVertexArray(lightSourceVAO);
	// load data into vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, lightSourceVBO);
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


	// configure frame buffer objects
	// ---------------------------------
	GLuint goochFBO, imageTextureMSAA, normalTextureMSAA, depthTextureMSAA;
	vector<GLuint*> renderTargetsMSAA; // G buffers
	glGenTextures(1, &imageTextureMSAA);
	renderTargetsMSAA.push_back(&imageTextureMSAA);
	glGenTextures(1, &normalTextureMSAA);
	renderTargetsMSAA.push_back(&normalTextureMSAA);
	glGenTextures(1, &depthTextureMSAA);
	renderTargetsMSAA.push_back(&depthTextureMSAA);
	configureFBO(window, &goochFBO, &renderTargetsMSAA, true, false, true);

	GLuint intermediateFBO, imageTexture, normalTexture, depthTexture;
	vector<GLuint*> intermediateRenderTargets;
	glGenTextures(1, &imageTexture);
	intermediateRenderTargets.push_back(&imageTexture);
	glGenTextures(1, &normalTexture);
	intermediateRenderTargets.push_back(&normalTexture);
	glGenTextures(1, &depthTexture);
	intermediateRenderTargets.push_back(&depthTexture);
	configureFBO(window, &intermediateFBO, &intermediateRenderTargets, false, false, false);

	GLuint hatchingFBO, hatching0, hatching1, hatching2, hatching3, hatching4, hatching5;
	vector<GLuint*> hatchingRenderTargets; // G buffers
	glGenTextures(1, &hatching0);
	hatchingRenderTargets.push_back(&hatching0);
	glGenTextures(1, &hatching1);
	hatchingRenderTargets.push_back(&hatching1);
	glGenTextures(1, &hatching2);
	hatchingRenderTargets.push_back(&hatching2);
	glGenTextures(1, &hatching3);
	hatchingRenderTargets.push_back(&hatching3);
	glGenTextures(1, &hatching4);
	hatchingRenderTargets.push_back(&hatching4);
	glGenTextures(1, &hatching5);
	hatchingRenderTargets.push_back(&hatching5);
	configureFBO(window, &hatchingFBO, &hatchingRenderTargets, false, true, false);


	// assign render targets for goochFBO and intermediateFBO
	// -----------------------------------
	GLuint GBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glBindFramebuffer(GL_FRAMEBUFFER, goochFBO);
	glDrawBuffers(3, GBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
	glDrawBuffers(3, GBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// set up FreeType
	// -----------------------------------
	int returnVal = SetUpFreeType(&(window->screenText));
	if (returnVal != 0)
	{
		return returnVal;
	}
	// configure VAO/VBO for texture quads
	GLuint textVAO, textVBO;
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(window->getWidth()), 0.0f, static_cast<float>(window->getHeight()));


	// shader configuration
	// ---------------------------------
	sobelShader.use();
	sobelShader.setInt("imageTexture", 0);
	sobelShader.setInt("normalTexture", 1);
	sobelShader.setInt("depthTexture", 2);

	hatchingShader.use();
	hatchingShader.setInt("hatching0", 0);
	hatchingShader.setInt("hatching1", 1);
	hatchingShader.setInt("hatching2", 2);
	hatchingShader.setInt("hatching3", 3);
	hatchingShader.setInt("hatching4", 4);
	hatchingShader.setInt("hatching5", 5);


	float fpsTimer = glfwGetTime();
	int numFrames = 0, numFramesToDisplay = 0;

	glm::vec3 lightPos = window->getLightingInfo()->getLIGHTPOS();
	float lightSourceFrame = window->getLightingInfo()->getCurrentLightSourcePosition();

	// render loop
	// -----------------------------------
	while (!glfwWindowShouldClose(window->getGLFWWindow()))
	{
		// per-frame time logic
		// --------------------
		window->getTimingInfo()->setCurrentFrame(glfwGetTime());
		window->getTimingInfo()->setDeltaTime();
		window->getTimingInfo()->setLastFrame(window->getTimingInfo()->getCurrentFrame());

		//processInput(window, &modelToRender);
		processMovement(window->getGLFWWindow(), window->getCameraInfo(), window->getTimingInfo());

		// render
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);	// Set background color to black and opaque
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set the view matrix in the uniform block - we only have to do this once per loop iteration.
		// -----------------------------------
		glm::mat4 view = (window->getCameraInfo()->getCamera())->GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// world transformation
		// scale then rotation then translation -> T * R * S
		// -----------------------------------
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 2.0f, 0.0f));
		
		if (!window->getTimingInfo()->isRotationPaused())
		{
			window->getTimingInfo()->setRotationTime(window->getTimingInfo()->getRotationTime() + window->getTimingInfo()->getDeltaTime() * window->getLightingInfo()->getDirectionFlip());
		}

		if (window->getActiveShaderID() == 2)
		{
			modelMatrix = glm::rotate(modelMatrix, window->getTimingInfo()->getRotationTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
		{
			window->getTimingInfo()->setRotationTime(0.0f);
		}

		glm::mat3 normalMatrix = glm::mat3(transpose(inverse(modelMatrix)));

		// store the projection matrix
		// -----------------------------------
		glm::mat4 projection = glm::perspective(glm::radians((window->getCameraInfo()->getCamera())->Zoom), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// activate shader before setting uniforms
		// -----------------------------------
		switch (window->getActiveShaderID())
		{
		case 1: // toon shader
			window->getTimingInfo()->setRotationPaused(true);

			toonShader.use();

			// set uniforms
			toonShader.setMat4("model", modelMatrix);
			toonShader.setMat3("normalMatrix", normalMatrix);
			toonShader.setVec3("lightPos", lightPos);
			toonShader.setVec3("viewPos", (window->getCameraInfo()->getCamera())->Position);
			toonShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			toonShader.setVec3("objectColor", 1.0f, 0.5f, 0.5f);

			// render model
			window->getModelToRender()->Draw(toonShader);
			break;

		case 2: // Gooch shader
			window->getLightingInfo()->setLightPaused(true);
			lightPos = window->getLightingInfo()->getLIGHTPOS();
			lightSourceFrame = window->getLightingInfo()->getCurrentLightSourcePosition();

			glBindFramebuffer(GL_FRAMEBUFFER, goochFBO);
				glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

				// Before rendering the first pass
				// clear the image's render target to white
				glDrawBuffer(GBuffers[0]);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				// clear the normals' render target to a vector facing away from the camera
				glDrawBuffer(GBuffers[1]);
				glm::vec3 clearVec(0.0f, 0.0f, -1.0f);
				// from normalized vector to rgb color; from [-1,1] to [0,1]
				clearVec = (clearVec + glm::vec3(1.0f, 1.0f, 1.0f)) * 0.5f;
				glClearColor(clearVec.x, clearVec.y, clearVec.z, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				// clear the depth's render target to black
				glDrawBuffer(GBuffers[2]);
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
				goochShader.setVec3("viewPos", (window->getCameraInfo()->getCamera())->Position);
				goochShader.setVec3("coolColor", 0.0f, 0.0f, 0.8f);
				goochShader.setVec3("warmColor", 0.4f, 0.4f, 0.0f);
				goochShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
				goochShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
				goochShader.setFloat("specularStrength", 0.5f);
				goochShader.setFloat("alpha", 0.25f);
				goochShader.setFloat("beta", 0.5f);

				// render model
				window->getModelToRender()->Draw(goochShader);

			// now blit multisampled buffer(s) to G intermediateFBO's G buffers
			glBindFramebuffer(GL_READ_FRAMEBUFFER, goochFBO);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);

				// must copy each color attachment one at a time
				// blitting a multisampled source texture into a singlesampled destination takes care of MSAA resolve
				// the resulting texture is anti-aliased
				glReadBuffer(GBuffers[0]); // image
				glDrawBuffer(GBuffers[0]);
				glBlitFramebuffer(0, 0, window->getWidth(), window->getHeight(), 0, 0, window->getWidth(), window->getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
				glReadBuffer(GBuffers[1]); // normal
				glDrawBuffer(GBuffers[1]);
				glBlitFramebuffer(0, 0, window->getWidth(), window->getHeight(), 0, 0, window->getWidth(), window->getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
				glReadBuffer(GBuffers[2]); // depth
				glDrawBuffer(GBuffers[2]);
				glBlitFramebuffer(0, 0, window->getWidth(), window->getHeight(), 0, 0, window->getWidth(), window->getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
				
			// Second pass: Do a full-screen edge detection filter over the normals from the first pass and draw feature edges
			// bind back to default framebuffer and draw a quad plane with the attached framebuffer color textures
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
				//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
				// clear all relevant buffers
				// set background to white to be able to see rendered outline
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				sobelShader.use();

				// activate textures
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, *intermediateRenderTargets[0]); // image
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, *intermediateRenderTargets[1]); // normal
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, *intermediateRenderTargets[2]); // depth

				glBindVertexArray(quadVAO);
			
				// set uniforms
				sobelShader.setVec4("_OutlineColor", 0.0f, 0.0f, 0.0f, 1.0f);
				sobelShader.setFloat("depthThreshold", 0.04f);
				sobelShader.setFloat("normalThreshold", 0.085f);
				/*sobelShader.setFloat("depthThreshold", 0.075f);
				sobelShader.setFloat("normalThreshold", 0.075f);*/

				// finally render quad
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glBindVertexArray(0);
			break;

		case 3: // hatching shader
			window->getTimingInfo()->setRotationPaused(true);

			hatchingShader.use();

			// activate textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *hatchingRenderTargets[0]); // light
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, *hatchingRenderTargets[1]);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, *hatchingRenderTargets[2]);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, *hatchingRenderTargets[3]);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, *hatchingRenderTargets[4]);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, *hatchingRenderTargets[5]); // dark

			// set uniforms
			hatchingShader.setMat4("model", modelMatrix);
			hatchingShader.setMat3("normalMatrix", normalMatrix);
			hatchingShader.setVec3("lightPos", lightPos);
			hatchingShader.setVec3("viewPos", (window->getCameraInfo()->getCamera())->Position);
			hatchingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

			// render model
			window->getModelToRender()->Draw(hatchingShader);
			break;

		default: // Phong shader
			window->getTimingInfo()->setRotationPaused(true);

			phongShader.use();

			// set uniforms
			phongShader.setMat4("model", modelMatrix);
			phongShader.setMat3("normalMatrix", normalMatrix);
			phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			phongShader.setVec3("lightPos", lightPos);
			phongShader.setVec3("objectColor", 1.0f, 0.5f, 0.5f);
			phongShader.setVec3("viewPos", (window->getCameraInfo()->getCamera())->Position);

			// render model
			window->getModelToRender()->Draw(phongShader);
		}


		if (window->getNormalsDisplaySetting())
		{
			// draw model with normal visualizing geometry shader
			normalShader.use();
			normalShader.setMat4("projection", projection);
			normalShader.setMat4("view", view);
			normalShader.setMat4("model", modelMatrix);

			// render model
			window->getModelToRender()->Draw(normalShader);
		}

		// render the light source
		// -----------------------------------
		lightSourceShader.use();
		// rotate light around y axis of the displayed object at the origin
		if (!window->getLightingInfo()->isLightPaused())
		{
			lightSourceFrame = lightSourceFrame + window->getTimingInfo()->getDeltaTime() * window->getLightingInfo()->getDirectionFlip();
			lightPos.x = sin(lightSourceFrame / window->getLightingInfo()->getLightSourceVelocity()) * window->getLightingInfo()->getLightPathRadius();
			lightPos.z = cos(lightSourceFrame / window->getLightingInfo()->getLightSourceVelocity()) * window->getLightingInfo()->getLightPathRadius();
		}
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, lightPos);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f)); // a smaller cube
		lightSourceShader.setMat4("model", modelMatrix);
		glBindVertexArray(lightSourceVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render the FPS
		// -----------------------------------
		renderTextShader.use();
		renderTextShader.setMat4("projection", textProjection);
		numFrames++;
		if (window->getTimingInfo()->getCurrentFrame() - fpsTimer >= 1.0f)
		{
			numFramesToDisplay = numFrames;
			numFrames = 0;
			fpsTimer += 1.0f;
		}
		// print FPS on screen
		std::string fpsText = std::string("FPS: ") + std::to_string(numFramesToDisplay);
		RenderText(renderTextShader, fpsText.c_str(), window->screenText, textVAO, textVBO, 25.0f, 25.0f, 1.0f, glm::vec3(1.0, 0.0f, 0.0f));

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwPollEvents();
		glfwSwapBuffers(window->getGLFWWindow());
	}

	// deallocate all resources
	// -----------------------------------
	glDeleteVertexArrays(1, &lightSourceVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &textVAO);

	glDeleteBuffers(1, &lightSourceVBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &textVBO);
	glDeleteBuffers(1, &uboMatrices);

	glDeleteFramebuffers(1, &goochFBO);
	glDeleteFramebuffers(1, &intermediateFBO);
	glDeleteFramebuffers(1, &hatchingFBO);

	glDeleteTextures(1, &imageTextureMSAA);
	glDeleteTextures(1, &normalTextureMSAA);
	glDeleteTextures(1, &depthTextureMSAA);
	glDeleteTextures(1, &imageTexture);
	glDeleteTextures(1, &normalTexture);
	glDeleteTextures(1, &depthTexture);
	glDeleteTextures(1, &hatching0);
	glDeleteTextures(1, &hatching1);
	glDeleteTextures(1, &hatching2);
	glDeleteTextures(1, &hatching3);
	glDeleteTextures(1, &hatching4);
	glDeleteTextures(1, &hatching5);

	delete window;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}


// resize window callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processMovement(GLFWwindow* window, CameraInfo* cameraInfo, TimingInfo* timingInfo)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		(cameraInfo->getCamera())->ProcessKeyboard(FORWARD, timingInfo->getDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		(cameraInfo->getCamera())->ProcessKeyboard(BACKWARD, timingInfo->getDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		(cameraInfo->getCamera())->ProcessKeyboard(LEFT, timingInfo->getDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		(cameraInfo->getCamera())->ProcessKeyboard(RIGHT, timingInfo->getDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		(cameraInfo->getCamera())->ProcessKeyboard(UP, timingInfo->getDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		(cameraInfo->getCamera())->ProcessKeyboard(DOWN, timingInfo->getDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		(cameraInfo->getCamera())->ResetCamera();
	}
}


//void processInput(GLFWwindow* window, Model** modelToRender)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* myWindow = (Window*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// light movement
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (myWindow->getActiveShaderID() == 2) // Gooch shading
		{
			myWindow->getTimingInfo()->setRotationPaused(!myWindow->getTimingInfo()->isRotationPaused());
			myWindow->getLightingInfo()->setLightPaused(true);
		}
		else
		{
			myWindow->getLightingInfo()->setLightPaused(!myWindow->getLightingInfo()->isLightPaused());
			myWindow->getTimingInfo()->setRotationPaused(true);
		}
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) // rotate light counter-clockwise
	{
		if (myWindow->getActiveShaderID() == 2) // Gooch shading
		{
			myWindow->getTimingInfo()->setRotationPaused(false);
		}
		else
		{
			myWindow->getLightingInfo()->setLightPaused(false);
		}
		myWindow->getLightingInfo()->setDirectionFlip(1.0f);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) // rotate light clockwise
	{
		if (myWindow->getActiveShaderID() == 2) // Gooch shading
		{
			myWindow->getTimingInfo()->setRotationPaused(false);
		}
		else
		{
			myWindow->getLightingInfo()->setLightPaused(false);
		}
		myWindow->getLightingInfo()->setDirectionFlip(-1.0f);
	}
	if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT) && action == GLFW_RELEASE)
	{
		if (myWindow->getActiveShaderID() == 2) // Gooch shading
		{
			myWindow->getTimingInfo()->setRotationPaused(true);
		}
		else
		{
			myWindow->getLightingInfo()->setLightPaused(true);
		}
	}

	// shaders
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) // toon shading
	{
		myWindow->setActiveShaderID(1);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) // Gooch shading
	{
		myWindow->setActiveShaderID(2);
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) // hatching
	{
		myWindow->setActiveShaderID(3);
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) // Phong shading
	{
		myWindow->setActiveShaderID(4);
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) // toggle normal vectors
	{
		myWindow->flipNormalsDisplaySetting();
	}

	/*
	use linked list instead of vector

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		vectorIndex++;

		if (vectorIndex > modelsList.size() - 1)
			vectorIndex = 0;

		modelToRender = modelsList[vectorIndex];
	}*/
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Window* myWindow = (Window*)glfwGetWindowUserPointer(window);

	if (myWindow->getCameraInfo()->getLastX() < 0 || myWindow->getCameraInfo()->getLastY() < 0)
	{
		myWindow->getCameraInfo()->setLastX(xpos);
		myWindow->getCameraInfo()->setLastY(ypos);
	}

	float xoffset = xpos - myWindow->getCameraInfo()->getLastX();
	float yoffset = myWindow->getCameraInfo()->getLastY() - ypos; // reversed since y-coordinates go from bottom to top

	myWindow->getCameraInfo()->setLastX(xpos);
	myWindow->getCameraInfo()->setLastY(ypos);

	myWindow->getCameraInfo()->getCamera()->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Window* myWindow = (Window*)glfwGetWindowUserPointer(window);
	myWindow->getCameraInfo()->getCamera()->ProcessMouseScroll(yoffset);
}

// Bind textures and buffers to frame buffer object
// ----------------------------------------------------------------------
void configureFBO(Window* window, GLuint* FBO, vector<GLuint*>* textures, bool multisample, bool mipmap, bool depthOrStencil) {

	glGenFramebuffers(1, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

	// get default textures
	int width, height, nrChannels;

	// generate texture buffers
	for (int i = 0; i < (*textures).size(); i++)
	{
		if (multisample)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *(*textures)[i]);

			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, window->getNumSamples(), GL_RGB, window->getWidth(), window->getHeight(), GL_TRUE);

			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, *(*textures)[i], 0);

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, *(*textures)[i]);

			if (mipmap) // create mipmaps for hatching textures
			{
				std::string filename = std::string("Textures/Hatch/hatch_") + std::to_string(i) + std::string(".jpg");
				unsigned char* tex = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // trilinear filtering
				stbi_image_free(tex);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window->getWidth(), window->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			}
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, *(*textures)[i], 0);
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
		if (multisample)
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, window->getNumSamples(), GL_DEPTH24_STENCIL8, window->getWidth(), window->getHeight());
		}
		else
		{
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->getWidth(), window->getHeight());
		}
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	}
	
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}