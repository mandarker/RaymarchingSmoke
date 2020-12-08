#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <vector>

#include "Shader.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"

#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "OBJ-Loader/OBJ_Loader.h"

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
		objl::Loader loader;
		loader.LoadFile("res/bunny2.obj");

		float * bunnyPositions = new float[loader.LoadedVertices.size() * 3];
		for (int i = 0; i < loader.LoadedVertices.size(); ++i) {
			bunnyPositions[i * 3] = loader.LoadedVertices[i].Position.X;
			bunnyPositions[i * 3 + 1] = loader.LoadedVertices[i].Position.Y;
			bunnyPositions[i * 3 + 2] = loader.LoadedVertices[i].Position.Z;
		}

		unsigned int * bunnyIndices = new unsigned int[loader.LoadedIndices.size()];
		for (int i = 0; i < loader.LoadedIndices.size(); ++i)
			bunnyIndices[i] = loader.LoadedIndices[i];

		float cubePositions[] = {
			1, 1, 1,
			1, -1, 1,
			-1, 1, 1,
			-1, -1, 1,
			1, 1, -1,
			1, -1, -1,
			-1, 1, -1,
			-1, -1, -1,
		};

		unsigned int cubeIndices[] = {
			6, 4, 0,
			0, 2, 6,
			2, 0, 1,
			1, 3, 2,
			3, 1, 5,
			5, 7, 3,
			7, 5, 4,
			4, 6, 7,
			0, 4, 5,
			5, 1, 0,
			6, 2, 3,
			3, 7, 6,
		};

		unsigned int cubeNormals[] = {
			1, 1, 1,
			1, -1, 1,
			-1, 1, 1,
			-1, -1, 1,
			1, 1, -1,
			1, -1, -1,
			-1, 1, -1,
			-1, -1, -1,
		};

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

		/*VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		
		IndexBuffer ib(indices, 6);

		VertexArray va;
		VertexBuffer vb(cubePositions, 8 * 3 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(cubeIndices, 36);*/

		VertexArray va;
		VertexBuffer vb(bunnyPositions, loader.LoadedVertices.size() * 3 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(bunnyIndices, loader.LoadedIndices.size());


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
		renderer.EnableDepthTesting();

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

		glm::vec3 translationA(0, -0.1, 0);
		glm::vec3 translationB(-0.5, -0.5, 0);

		Camera cam = Camera(0.0f, 0.0f, 0.0f);

		float radius = 10;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			shader.Bind();
			
			cam.rotateCamera(radius);

			// Change the camera position in real time
			glm::mat4 view = cam.getView();

			// render object 1
			if (display_first_object)
			{
				glm::mat4 model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)), translationA);
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

				//ImGui::Checkbox("Flip camera?", &flipCamera);

				ImGui::DragFloat("Camera zoom", &radius, 0.1f, 0.0f, 20.0f);

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

		delete[] bunnyPositions;
		delete[] bunnyIndices;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}