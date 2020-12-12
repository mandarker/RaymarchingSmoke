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
#include "FrameBuffer.h"

#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "OBJ-Loader/OBJ_Loader.h"

// new stuff
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

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

	// new stuff
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// vsync
	glfwSwapInterval(0);

	if (glewInit() != GLEW_OK)
		std::cout << "Error" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		objl::Loader loader;
		loader.LoadFile("res/bunny2.obj");

		float * bunnyPositions = new float[loader.LoadedVertices.size() * 6];
		for (unsigned int i = 0; i < loader.LoadedVertices.size(); ++i) {
			bunnyPositions[i * 6] = loader.LoadedVertices[i].Position.X;
			bunnyPositions[i * 6 + 1] = loader.LoadedVertices[i].Position.Y;
			bunnyPositions[i * 6 + 2] = loader.LoadedVertices[i].Position.Z;
			bunnyPositions[i * 6 + 3] = loader.LoadedVertices[i].Normal.X;
			bunnyPositions[i * 6 + 4] = loader.LoadedVertices[i].Normal.Y;
			bunnyPositions[i * 6 + 5] = loader.LoadedVertices[i].Normal.Z;
		}

		unsigned int * bunnyIndices = new unsigned int[loader.LoadedIndices.size()];
		for (unsigned int i = 0; i < loader.LoadedIndices.size(); ++i)
			bunnyIndices[i] = loader.LoadedIndices[i];

		float cubePositions[] = {
			1, 1, 1,   1, 1, 1,
			1, -1, 1,  1, -1, 1,
			-1, 1, 1,  -1, 1, 1,
			-1, -1, 1, -1, -1, 1,
			1, 1, -1,  1, 1, -1,
			1, -1, -1, 1, -1, -1,
			-1, 1, -1, -1, 1, -1,
			-1, -1, -1,-1, -1, -1,
		};

		int cubeNormals[] = {
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

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexArray cubeVa;
		VertexBuffer cubeVb(cubePositions, 8 * 6 * sizeof(float));
		VertexBufferLayout cubeLayout;
		cubeLayout.Push<float>(3);
		cubeLayout.Push<float>(3);
		cubeVa.AddBuffer(cubeVb, cubeLayout);

		IndexBuffer cubeIb(cubeIndices, 36);

		VertexArray bunnyVa;
		VertexBuffer bunnyVb(bunnyPositions, loader.LoadedVertices.size() * 6 * sizeof(float));
		VertexBufferLayout bunnyLayout;
		bunnyLayout.Push<float>(3);
		bunnyLayout.Push<float>(3);
		bunnyVa.AddBuffer(bunnyVb, bunnyLayout);

		IndexBuffer bunnyIb(bunnyIndices, loader.LoadedIndices.size());

		// Display range 0.1f units <----> 100.0f units
		// glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, 0.1f, 100.0f);
		glm::mat4 proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 100.0f);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		Shader shadowShader("res/shaders/ShadowMapping.shader");
		shadowShader.Bind();

		unsigned int width = 1024;
		unsigned int height = 1024;

		FrameBuffer depth(width, height);

		cubeVa.Unbind();
		cubeVb.Unbind();
		cubeIb.Unbind();

		bunnyVa.Unbind();
		bunnyVb.Unbind();
		bunnyIb.Unbind();
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
		glm::vec3 translationB(0, -6.5, 0);

		Camera cam = Camera(0.0f, 0.0f, 0.0f);

		// new stuff
		float lightPosX = -2.0f;
		float lightPosY = 4.0f;
		float lightPosZ = -1.0f;
		float radius = 10;
		float angleDelta = 0.0005;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			cam.rotateCamera(radius);
			cam.setAngleIncrement(angleDelta);

			glm::vec3 lightPos(lightPosX, lightPosY, lightPosZ);

			// Change the camera position in real time
			glm::mat4 view = cam.getView();

			// new stuff
			glm::mat4 lightProjection, lightView;
			glm::mat4 lightSpaceMatrix;
			float near_plane = 0.1f, far_plane = 100.0f;
			//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			lightSpaceMatrix = lightProjection * lightView;
			// render scene from light's point of view
			shadowShader.Bind();
			shadowShader.SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
			
			glViewport(0, 0, 1024, 1024);
			depth.Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glCullFace(GL_FRONT);
			// Render bunny
			if (display_first_object)
			{
				glm::mat4 model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)), translationA);
				shadowShader.SetUniformMat4f("u_Model", model);
				renderer.Draw(bunnyVa, bunnyIb, shadowShader);
			}

			// Render floor
			if (display_second_object)
			{
				glm::mat4 model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 0.1f, 10.0f)), translationB);
				shadowShader.SetUniformMat4f("u_Model", model);
				renderer.Draw(cubeVa, cubeIb, shadowShader);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, 960, 720);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.Bind();

			shader.SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);

			glCullFace(GL_BACK);
			glActiveTexture(GL_TEXTURE0);
			depth.BindTexture();
			shader.SetUniformMat4f("u_View", view);
			shader.SetUniformMat4f("u_Proj", proj);
			shader.SetUniformVec3f("u_Light", lightPos);

			// Render bunny
			if (display_first_object)
			{
				glm::mat4 model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)), translationA);
				shader.SetUniformMat4f("u_Model", model);
				renderer.Draw(bunnyVa, bunnyIb, shader);
			}

			// Render floor
			if (display_second_object)
			{
				glm::mat4 model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 0.1f, 10.0f)), translationB);
				shader.SetUniformMat4f("u_Model", model);
				renderer.Draw(cubeVa, cubeIb, shader);
			}
			
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Scene settings");												// Title of window

				ImGui::Text("Edit the scene in real time with the settings below.");        // Display some text (you can use a format strings too)
				ImGui::Checkbox("Show bunny", &display_first_object);				// Edit bools storing our window open/close state
				ImGui::Checkbox("Show square", &display_second_object);

				//ImGui::Checkbox("Flip camera?", &flipCamera);

				ImGui::DragFloat("Camera zoom", &radius, 0.1f, 0.0f, 20.0f);
				ImGui::DragFloat("Camera spin", &angleDelta, 0.0001f, -0.001f, 0.001f);

				ImGui::DragFloat("Light x", &lightPosX, 0.1f, -10.0f, 10.0f);
				ImGui::DragFloat("Light y", &lightPosY, 0.1f, -10.0f, 10.0f);
				ImGui::DragFloat("Light z", &lightPosZ, 0.1f, -10.0f, 10.0f);


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