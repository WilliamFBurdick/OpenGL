#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// timing
float deltaTime = 0.0f;	// time to render current frame
float lastFrame = 0.0f;	// time of last frame

// camera
Camera camera;

// mosue input
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = (float)SCREEN_WIDTH / 2.0;
float lastY = (float)SCREEN_HEIGHT / 2.0;
float fov = 45.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// Initialize and configure GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// GLFW create window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
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

	// tell GLFW to capture mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD  load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// create and compile shader program
	Shader shader("../Shaders/main.vert", "../Shaders/main.frag");
	// shader program for light source
	Shader lightShader("../Shaders/lightsource.vert", "../Shaders/lightsource.frag");

	// set up vertex attributes
	float vertices[] = {
		// POSITION			// TEX COORDS	// NORMALS
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	0.0f, -1.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f, 1.0f, 0.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f, 0.6f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0, 0.0),
		glm::vec3(0.2f, 0.2f, 1.0f)
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffers, and then configure vertex attributes
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set up attributes
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// create VAO for light source object
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// load and create texture
	// ----------------------
	unsigned int diffuse = loadTexture("../Imgs/container2.png");
	unsigned int specular = loadTexture("../Imgs/container2_specular.png");
	unsigned int emission = loadTexture("../Imgs/matrix.jpg");
	glBindTexture(GL_TEXTURE_2D, diffuse);
	glBindTexture(GL_TEXTURE_2D, specular);
	glBindTexture(GL_TEXTURE_2D, emission);
	


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	shader.use();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);

	string path = "../Models/backpack/backpack.obj";
	Model backpack(path.c_str());

	while (!glfwWindowShouldClose(window))
	{
		// update deltatime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		// render model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		shader.setMat4("model", model);
		backpack.Draw(shader);


		// process events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader.ID);
	glDeleteProgram(lightShader.ID);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = yPos - lastY;
	lastX = xPos;
	lastY = yPos;
	
	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}