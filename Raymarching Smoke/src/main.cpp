#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "Shader.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 720, "Raymarching Smoke", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// vsync
	glfwSwapInterval(0);

	if (glewInit() != GLEW_OK)
		std::cout << "Error" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		float positions[] = {
			-0.5f, -0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f, 1.0f, 0.0f,
			 0.5f,  0.5f, 1.0f, 1.0f,
			 -0.5f, 0.5f, 0.0f, 1.0f,
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		// Display range 0.1f units <----> 100.0f units
		// glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, 0.1f, 100.0f);
		glm::mat4 proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 100.0f);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		Texture texture("res/cherno.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		// setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

		bool display_first_object = true;
		bool display_second_object = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		glm::vec3 translationA(0.5, 0.5, 0);
		glm::vec3 translationB(-0.5, -0.5, 0);

		float cameraMinFloat = -100.0f;
		float cameraMaxFloat = 100.0f;

		float cameraX = -4.0f;
		float cameraY = 3.0f;
		float cameraZ = 3.0f;

		float radius = 1;
		float angle = 0;


		glm::vec3 cameraPos = glm::vec3(cameraX, cameraY, cameraZ);

		bool flipCamera = false;
		int headY = 1;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			shader.Bind();
			
			angle += 0.01f;

			// make camera move in circle
			cameraX = radius * sin(angle);
			cameraZ = radius * cos(angle);

			headY = flipCamera ? -1 : 1;
			// Change the camera position in real time
			glm::mat4 view = glm::lookAt(
				//glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
				glm::vec3(cameraX, cameraY, cameraZ),
				glm::vec3(0, 0, 0), // and looks at the origin
				glm::vec3(0, headY, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);

			// render object 1
			if (display_first_object)
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			// render object 2
			if (display_second_object)
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Scene settings");												// Title of window

				ImGui::Text("Edit the scene in real time with the settings below.");        // Display some text (you can use a format strings too)
				ImGui::Checkbox("Show first object", &display_first_object);				// Edit bools storing our window open/close state
				ImGui::Checkbox("Show second object", &display_second_object);
				ImGui::Checkbox("Flip camera?", &flipCamera);

				ImGui::SliderFloat("CameraX", &cameraX, cameraMinFloat, cameraMaxFloat);	
				ImGui::SliderFloat("CameraY", &cameraY, cameraMinFloat, cameraMaxFloat);
				ImGui::SliderFloat("CameraZ", &cameraZ, cameraMinFloat, cameraMaxFloat);

				//ImGui::ColorEdit3("clear color", (float*)&clear_color);						// Edit 3 floats representing a color

				//if (ImGui::Button("Button"))												// Buttons return true when clicked (most widgets return true when edited/activated)
				//	counter++;
				//ImGui::SameLine();
				//ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}