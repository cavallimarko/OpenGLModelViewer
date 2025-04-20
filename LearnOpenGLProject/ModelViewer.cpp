#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Filesystem.h"

#include <iostream>
#include "stb_image/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Windows.h>
#include <algorithm>
#include <filesystem>
#include <iostream>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
std::string AddSpaceToLargeNumbers(int num);
void CenterCameraOnModel();

// settings
unsigned int SCR_WIDTH = 1600;
unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool wireframe = false;
glm::vec3 minimumScenePosition;
glm::vec3 maximumScenePosition;

int main(int argc, char* argv[])
{
	std::string filename;
	if (argc <= 1)
	{
		//load backpack when no model is provided
		filename = "resources/models/backpack/backpack.obj";
		stbi_set_flip_vertically_on_load(true);
	}
	else
	{
		filename = argv[1];
	}

	HMODULE this_process_handle = GetModuleHandle(NULL);
	wchar_t this_process_path[MAX_PATH];

	GetModuleFileNameW(NULL, this_process_path, sizeof(this_process_path));
	std::wstring stringw(this_process_path);
	std::string str(stringw.begin(), stringw.end());
	std::string path = str.substr(0, str.find_last_of("\\/"));
	std::replace(path.begin(), path.end(), '\\', '/');
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ModelViewer", NULL, NULL);
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

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	//glfwSwapInterval(0);//disble vsync
	// load models
	// -----------
	std::filesystem::path cwd = std::filesystem::current_path();
	std::cout << "CWD: " << cwd << std::endl;
	std::string pathModelFolder = filename;
	//std::string pathModelFolder = cwd.string().append("\\").append(filename);
	std::replace(pathModelFolder.begin(), pathModelFolder.end(), '\\', '/');
	Model ourModel(pathModelFolder);

	minimumScenePosition = ourModel.GetMinimumPosition();
	maximumScenePosition = ourModel.GetMaximumPosition();

	//load and compile shaders
	std::string vert = "/resources/shaders/lighting.vert";
	std::string frag;
	if (ourModel.GetTexturesCount() > 0)
	{
		frag = "/resources/shaders/model.frag";
	}
	else
	{
		frag = "/resources/shaders/modelNoTex.frag";
	}
	//std::cout << vert << std::endl;
	Shader ourShader((path + vert).c_str(), (path + frag).c_str());
	Shader wireframeShader((path + vert).c_str(), (path + "/resources/shaders/unlitWhite.frag").c_str());
	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	// -----------
	CenterCameraOnModel();

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------

		int vertexCount = ourModel.GetVertexCount();

		int triangleCount = ourModel.GetTriangleCount();
		std::string StatsString = "Vertices= " + AddSpaceToLargeNumbers(vertexCount) + ", Triangles= " + AddSpaceToLargeNumbers(triangleCount) + " CameraMovementSpeed(ScrollWheel)= " + std::to_string(camera.MovementSpeed) + " WireFrameToggleHotkey:T, Movement: WASDQE, Focus on model: F";
		//std::cout << StatsString << std::endl;
		glfwSetWindowTitle(window, StatsString.c_str());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// don't forget to enable shader before setting uniforms
		ourShader.use();
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1, 1);
		ourModel.Draw(ourShader, camera);
		glDisable(GL_POLYGON_OFFSET_FILL);
		if (wireframe)
		{
			wireframeShader.use();
			wireframeShader.setMat4("projection", projection);
			wireframeShader.setMat4("view", view);
			wireframeShader.setMat4("model", model);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			ourModel.Draw(wireframeShader, camera);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.MovementSpeed += 1;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.MovementSpeed = 1 > camera.MovementSpeed < -1 ? camera.MovementSpeed - 1 : 1;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//camera.ProcessMouseScroll(static_cast<float>(yoffset));
	float speed = camera.MovementSpeed;
	speed += yoffset * 0.5f * camera.MovementSpeed;
	camera.MovementSpeed = std::clamp(speed, 0.05f, 2000.0f);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		wireframe = !wireframe;
		//glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		CenterCameraOnModel();
	}
}
std::string AddSpaceToLargeNumbers(int num)
{
	std::string s = std::to_string(num);
	int n = s.length() - 3;
	int end = (num >= 0) ? 0 : 1; // Support for negative numbers
	while (n > end)
	{
		s.insert(n, " ");
		n -= 3;
	}
	return s;
}
void CenterCameraOnModel()
{
	glm::vec3 halfDiagonal = (maximumScenePosition - minimumScenePosition) * 0.5f;
	glm::vec3 center = minimumScenePosition + halfDiagonal;
	float r = glm::length(halfDiagonal);
	float distanceFromModelToCamera = r / glm::sin(glm::radians(camera.Zoom / 2));
	//std::cout << "distanceFromModelToCamera: " << distanceFromModelToCamera << std::endl;
	//std::cout << "r: " << r << std::endl;
	camera.Position = glm::vec3(center.x, center.y, center.z + distanceFromModelToCamera);
	camera.ResetCameraRotation();
}