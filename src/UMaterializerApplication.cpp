#include "UMaterializerApplication.hpp"
#include "UMaterializerContext.hpp"
#include "UInput.hpp"
#include "../lib/glfw/deps/glad/gl.h"

#include <J3D/J3DUniformBufferObject.hpp>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

void DealWithGLErrors(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << "GL CALLBACK: " << message << std::endl;
}

UMaterializerApplication::UMaterializerApplication() {
	mWindow = nullptr;
	mContext = nullptr;
}

bool UMaterializerApplication::Setup() {
	// Initialize GLFW
	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	mWindow = glfwCreateWindow(640, 480, "J3DMaterializer", nullptr, nullptr);
	if (mWindow == nullptr) {
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(mWindow, UInput::GLFWKeyCallback);
	glfwSetCursorPosCallback(mWindow, UInput::GLFWMousePositionCallback);
	glfwSetMouseButtonCallback(mWindow, UInput::GLFWMouseButtonCallback);
	glfwSetScrollCallback(mWindow, UInput::GLFWMouseScrollCallback);

	glfwMakeContextCurrent(mWindow);
	gladLoadGL(glfwGetProcAddress);
	glClearColor(0.5f, 1.0f, 0.5f, 1.0f);
	glfwSwapInterval(1);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DealWithGLErrors, nullptr);

	// Initialize imgui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	ImGui_ImplOpenGL3_Init("#version 150");

	// Create viewer context
	mContext = new UMaterializerContext();

	return true;
}

bool UMaterializerApplication::Teardown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
	glfwDestroyWindow(mWindow);
	glfwTerminate();

	J3DUniformBufferObject::DestroyUBO();

	delete mContext;

	return true;
}

bool UMaterializerApplication::Execute(float deltaTime) {
	// Try to make sure we return an error if anything's fucky
	if (mContext == nullptr || mWindow == nullptr || glfwWindowShouldClose(mWindow))
		return false;

	// Update viewer context
	mContext->Update(deltaTime);

	// Update window title, will probably be removed at some point
	char titleBuffer[32];
	snprintf(titleBuffer, 31, "J3DMaterializer (%f fps)", deltaTime);
	titleBuffer[31] = 0;
	glfwSetWindowTitle(mWindow, titleBuffer);
	
	// Begin actual rendering
	glfwMakeContextCurrent(mWindow);
	glfwPollEvents();

	UInput::UpdateInputState();

	// The context renders both the ImGui elements and the background elements.
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Update buffer size
	int width, height;
	glfwGetFramebufferSize(mWindow, &width, &height);
	glViewport(0, 0, width, height);

	// Clear buffers
	glClearColor(0.353f, 0.294f, 0.647f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render viewer context
	mContext->Render(deltaTime);
	
	// Render imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap buffers
	glfwSwapBuffers(mWindow);

	return true;
}
