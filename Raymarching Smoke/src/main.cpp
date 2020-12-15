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
	window = glfwCreateWindow(960, 720, "Bunny", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// vsync
	glfwSwapInterval(0);

	if (glewInit() != GLEW_OK) {
		std::cout << "failed to init GLEW" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		objl::Loader loader;
		loader.LoadFile("res/bunny_corrected.obj");

		float * bunnyPositions = new float[loader.LoadedVertices.size() * 8];
		for (unsigned int i = 0; i < loader.LoadedVertices.size(); ++i) {
			bunnyPositions[i * 8] = loader.LoadedVertices[i].Position.X;
			bunnyPositions[i * 8 + 1] = loader.LoadedVertices[i].Position.Y;
			bunnyPositions[i * 8 + 2] = loader.LoadedVertices[i].Position.Z;
			bunnyPositions[i * 8 + 3] = loader.LoadedVertices[i].Normal.X;
			bunnyPositions[i * 8 + 4] = loader.LoadedVertices[i].Normal.Y;
			bunnyPositions[i * 8 + 5] = loader.LoadedVertices[i].Normal.Z;
			bunnyPositions[i * 8 + 6] = loader.LoadedVertices[i].TextureCoordinate.X;
			bunnyPositions[i * 8 + 7] = loader.LoadedVertices[i].TextureCoordinate.Y;
		}

		unsigned int * bunnyIndices = new unsigned int[loader.LoadedIndices.size()];
		for (unsigned int i = 0; i < loader.LoadedIndices.size(); ++i)
			bunnyIndices[i] = loader.LoadedIndices[i];

		VertexArray bunnyVa;
		VertexBuffer bunnyVb(bunnyPositions, loader.LoadedVertices.size() * 8 * sizeof(float));
		VertexBufferLayout bunnyLayout;
		bunnyLayout.Push<float>(3);
		bunnyLayout.Push<float>(3);
		bunnyLayout.Push<float>(2);
		bunnyVa.AddBuffer(bunnyVb, bunnyLayout);

		IndexBuffer bunnyIb(bunnyIndices, loader.LoadedIndices.size());

		loader.LoadFile("res/Floor.obj");

		float * floorPositions = new float[loader.LoadedVertices.size() * 11];
		for (unsigned int i = 0; i < loader.LoadedVertices.size(); ++i) {
			floorPositions[i * 11] = loader.LoadedVertices[i].Position.X;
			floorPositions[i * 11 + 1] = loader.LoadedVertices[i].Position.Y;
			floorPositions[i * 11 + 2] = loader.LoadedVertices[i].Position.Z;
			floorPositions[i * 11 + 3] = loader.LoadedVertices[i].Normal.X;
			floorPositions[i * 11 + 4] = loader.LoadedVertices[i].Normal.Y;
			floorPositions[i * 11 + 5] = loader.LoadedVertices[i].Normal.Z;
			floorPositions[i * 11 + 6] = loader.LoadedVertices[i].TextureCoordinate.X;
			floorPositions[i * 11 + 7] = loader.LoadedVertices[i].TextureCoordinate.Y;
			floorPositions[i * 11 + 8] = 0;
			floorPositions[i * 11 + 9] = 0;
			floorPositions[i * 11 + 10] = 0;
		}

		unsigned int * floorIndices = new unsigned int[loader.LoadedIndices.size()];
		for (unsigned int i = 0; i < loader.LoadedIndices.size(); ++i)
			floorIndices[i] = loader.LoadedIndices[i];

		for (unsigned int i = 0; i < loader.LoadedIndices.size(); i += 3) {
			unsigned int v0Index = loader.LoadedIndices[i];
			unsigned int v1Index = loader.LoadedIndices[i + 1];
			unsigned int v2Index = loader.LoadedIndices[i + 2];

			glm::vec3 edge1 = glm::vec3(loader.LoadedVertices[v1Index].Position.X, loader.LoadedVertices[v1Index].Position.Y, loader.LoadedVertices[v1Index].Position.Z)
				- glm::vec3(loader.LoadedVertices[v0Index].Position.X, loader.LoadedVertices[v0Index].Position.Y, loader.LoadedVertices[v0Index].Position.Z);
			glm::vec3 edge2 = glm::vec3(loader.LoadedVertices[v2Index].Position.X, loader.LoadedVertices[v2Index].Position.Y, loader.LoadedVertices[v2Index].Position.Z)
				- glm::vec3(loader.LoadedVertices[v0Index].Position.X, loader.LoadedVertices[v0Index].Position.Y, loader.LoadedVertices[v0Index].Position.Z);

			float deltaU1 = loader.LoadedVertices[v1Index].TextureCoordinate.X - loader.LoadedVertices[v0Index].TextureCoordinate.X;
			float deltaV1 = loader.LoadedVertices[v1Index].TextureCoordinate.Y - loader.LoadedVertices[v0Index].TextureCoordinate.Y;
			float deltaU2 = loader.LoadedVertices[v2Index].TextureCoordinate.X - loader.LoadedVertices[v0Index].TextureCoordinate.X;
			float deltaV2 = loader.LoadedVertices[v2Index].TextureCoordinate.Y - loader.LoadedVertices[v0Index].TextureCoordinate.Y;

			float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

			glm::vec3 tangent;

			tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
			tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
			tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

			for (unsigned int i = 8; i < 11; ++i) {
				floorPositions[v0Index * 11 + i] += tangent[i - 8];
				floorPositions[v1Index * 11 + i] += tangent[i - 8];
				floorPositions[v2Index * 11 + i] += tangent[i - 8];
			}
		}

		VertexArray floorVa;
		VertexBuffer floorVb(floorPositions, loader.LoadedVertices.size() * 11 * sizeof(float));
		VertexBufferLayout floorLayout;
		floorLayout.Push<float>(3);
		floorLayout.Push<float>(3);
		floorLayout.Push<float>(2);
		floorLayout.Push<float>(3);
		floorVa.AddBuffer(floorVb, floorLayout);

		IndexBuffer floorIb(floorIndices, loader.LoadedIndices.size());

		// Display range 0.1f units <----> 100.0f units
		// glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, 0.1f, 100.0f);
		glm::mat4 proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 100.0f);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		Shader shadowShader("res/shaders/ShadowMapping.shader");
		shadowShader.Bind();

		Shader floorShader("res/shaders/BumpMapping.shader");
		floorShader.Bind();

		Texture bunnyUV("res/uv_mapping.png");
		Texture floorMap("res/wood_normals.png");

		unsigned int width = 2048;
		unsigned int height = 2048;

		FrameBuffer depth(width, height);

		floorVa.Unbind();
		floorVb.Unbind();
		floorIb.Unbind();

		bunnyVa.Unbind();
		bunnyVb.Unbind();
		bunnyIb.Unbind();

		shader.Unbind();
		shadowShader.Unbind();
		floorShader.Unbind();

		Renderer renderer;
		renderer.EnableDepthTesting();

		// setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

		bool displayBunny = true;
		bool displayFloor = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		glm::vec3 bunnyTranslation(0, 0, 0);
		glm::vec3 floorTranslation(0, 0, 0);

		Camera cam = Camera(0.0f, 0.0f, 0.0f);

		glm::mat4 bunnyModel = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)), bunnyTranslation);
		glm::mat4 floorModel = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(5.0f)), floorTranslation);

		// Variables to be used / updated in the window loop
		float lightPosX = -2.0f;
		float lightPosY = 7.0f;
		float lightPosZ = -1.0f;
		float radius = 10;
		float angleDelta = 0;

		auto floorColor = ImColor(94, 76, 33);
		auto lightColor = ImColor(255, 255, 255);
		auto specularColor = ImColor(255, 255, 255);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			cam.rotateCamera(radius);
			cam.setAngleIncrement(angleDelta/500);

			glm::vec3 lightPos(lightPosX, lightPosY, lightPosZ);
			glm::vec3 lightColorVec3(lightColor.Value.x, lightColor.Value.y, lightColor.Value.z);
			glm::vec3 floorColorVec3(floorColor.Value.x, floorColor.Value.y, floorColor.Value.z);
			glm::vec3 specularColorVec3(specularColor.Value.x, specularColor.Value.y, specularColor.Value.z);

			// Change the camera position in real time
			glm::mat4 view = cam.getView();

			glm::mat4 lightProjection, lightView;
			glm::mat4 lightSpaceMatrix;
			float near_plane = 0.1f, far_plane = 100.0f;

			// note that if you use a perspective projection matrix you'll have to change the light position as the current light position sn't enough to reflect the whole scene
			//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); i
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			lightSpaceMatrix = lightProjection * lightView;
			
			// Render scene from light's point of view
			shadowShader.Bind();
			shadowShader.SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);

			glViewport(0, 0, width, height);
			depth.Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glCullFace(GL_FRONT);

			// Render bunny
			if (displayBunny)
			{
				shadowShader.SetUniformMat4f("u_Model", bunnyModel);
				renderer.Draw(bunnyVa, bunnyIb, shadowShader);
			}

			// Render floor
			if (displayFloor)
			{
				shadowShader.SetUniformMat4f("u_Model", floorModel);
				renderer.Draw(floorVa, floorIb, shadowShader);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Render scene from camera's point of view
			glViewport(0, 0, 960, 720);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.Bind();

			// Render bunny
			if (displayBunny)
			{
				bunnyUV.Bind(1);
				shader.SetUniform1i("u_Texture", 1);

				shader.SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);

				glCullFace(GL_BACK);
				glActiveTexture(GL_TEXTURE0);
				depth.BindTexture();
				shader.SetUniformMat4f("u_View", view);
				shader.SetUniformMat4f("u_Proj", proj);
				shader.SetUniformVec3f("u_Light", lightPos);
				shader.SetUniformVec3f("u_LightColor", lightColorVec3);
				shader.SetUniformVec3f("u_SpecularColor", specularColorVec3);

				shader.SetUniformMat4f("u_Model", bunnyModel);
				renderer.Draw(bunnyVa, bunnyIb, shader);
			}

			// Render floor
			if (displayFloor)
			{
				floorShader.Bind();
				floorMap.Bind(1);

				floorShader.SetUniform1i("u_Texture", 1);
				floorShader.SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
				floorShader.SetUniformMat4f("u_View", view);
				floorShader.SetUniformMat4f("u_Proj", proj);
				floorShader.SetUniformVec3f("u_Light", lightPos);
				floorShader.SetUniformVec3f("u_LightColor", lightColorVec3);
				floorShader.SetUniformVec3f("u_Color", floorColorVec3);
				floorShader.SetUniformVec3f("u_SpecularColor", specularColorVec3);

				floorShader.SetUniformMat4f("u_Model", floorModel);
				renderer.Draw(floorVa, floorIb, floorShader);
			}

			{
				ImGui::Begin("Scene settings");	// Title of window

				ImGui::Text("Edit the scene in real time with the settings below.");        
				ImGui::Checkbox("Show bunny", &displayBunny);				
				ImGui::Checkbox("Show floor", &displayFloor);

				ImGui::DragFloat("Camera zoom", &radius, 0.1f, 5.0f, 20.0f);
				ImGui::DragFloat("Camera spin", &angleDelta, 0.025f, -1.0f, 1.0f);
				ImGui::DragFloat("Light x", &lightPosX, 0.1f, -8.0f, 8.0f);
				ImGui::DragFloat("Light z", &lightPosZ, 0.1f, -8.0f, 8.0f);

				ImGui::ColorEdit3("Specular color", (float*)&specularColor);
				ImGui::ColorEdit3("Light color", (float*)&lightColor);
				ImGui::ColorEdit3("Floor color", (float*)&floorColor);

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

		// Clean arrays
		delete[] bunnyPositions;
		delete[] bunnyIndices;

		delete[] floorPositions;
		delete[] floorIndices;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}