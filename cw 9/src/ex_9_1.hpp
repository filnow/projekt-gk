﻿#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include "SOIL/SOIL.h"

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

int WIDTH = 500, HEIGHT = 500;

namespace models {
	/*Core::RenderContext bedContext;
	Core::RenderContext materaceContext;
	Core::RenderContext tableContext;
	Core::RenderContext doorContext;
	Core::RenderContext planeContext;
	Core::RenderContext roomContext;
	Core::RenderContext spaceshipContext;
	Core::RenderContext window1Context;
	Core::RenderContext window2Context;
	Core::RenderContext window3Context;
	Core::RenderContext testContext;
	Core::RenderContext lampContext;
	Core::RenderContext ceilingfanContext;
	Core::RenderContext chair1Context;
	Core::RenderContext chair2Context;
	Core::RenderContext wall1Context;
	Core::RenderContext wall2Context;
	Core::RenderContext wall3Context;
	Core::RenderContext wall4Context;*/
	Core::RenderContext floorContext;
	Core::RenderContext bedContext;
	Core::RenderContext pillowsContext;
	Core::RenderContext chairContext;
	Core::RenderContext wallsContext;
	Core::RenderContext candleContext;
	Core::RenderContext blanketContext;
	Core::RenderContext cabinetContext;
	Core::RenderContext curtainsContext;
	Core::RenderContext deskContext;
	Core::RenderContext doorContext;
	Core::RenderContext padContext;
	Core::RenderContext wall_bedContext;
	Core::RenderContext door_panelContext;

}

namespace texture {
	/*GLuint kot;
	GLuint wood_floor;
	GLuint table;
	GLuint wallpaper;
	GLuint materace;
	GLuint chairs;
	GLuint metal;
	GLuint bed;*/
	GLuint floor;
	GLuint marble;
	GLuint fabric;
	GLuint blanket;
	GLuint smth;


}

GLuint depthMapFBO;
GLuint depthMap;

GLuint program;
GLuint programSun;
GLuint programTest;
GLuint programTex;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;

glm::vec3 sunPos = glm::vec3(-4.740971f, 2.149999f, 0.369280f);
glm::vec3 sunDir = glm::vec3(-0.93633f, 0.351106, 0.003226f);
glm::vec3 sunColor = glm::vec3(0.9f, 0.9f, 0.7f) * 5;

glm::vec3 cameraPos = glm::vec3(0.479490f, 1.250000f, -2.124680f);
glm::vec3 cameraDir = glm::vec3(-0.354510f, 0.000000f, 0.935054f);


glm::vec3 spaceshipPos = glm::vec3(0.065808f, 1.250000f, -2.189549f);
glm::vec3 spaceshipDir = glm::vec3(-0.490263f, 0.000000f, 0.871578f);
GLuint VAO, VBO;

float aspectRatio = 1.f;

float exposition = 1.f;

glm::vec3 pointlightPos = glm::vec3(0, 2, 0);
glm::vec3 pointlightColor = glm::vec3(0.9, 0.6, 0.6);

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.4, 0.4, 0.9) * 3;
float spotlightPhi = 3.14 / 4;



float lastTime = -1.f;
float deltaTime = 0.f;

void updateDeltaTime(float time) {
	if (lastTime < 0) {
		lastTime = time;
		return;
	}

	deltaTime = time - lastTime;
	if (deltaTime > 0.1) deltaTime = 0.1;
	lastTime = time;
}
glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{

	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectPBR(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic) {


	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);

	Core::DrawContext(context);

}


void drawObjectTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texturePath, int nbr) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programTex, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform1i(glGetUniformLocation(programTex, "nbr"), nbr);
	Core::SetActiveTexture(texturePath, "colorTexture", programTex, 0);
	
	Core::DrawContext(context);

}


void renderShadowapSun() {
	float time = glfwGetTime();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);







	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void renderScene(GLFWwindow* window)
{
	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float time = glfwGetTime();
	updateDeltaTime(time);
	renderShadowapSun();

	

	glUseProgram(programTex);
	//drawObjectTexture(models::bedContext, glm::mat4(), texture::bed, 10);
	drawObjectTexture(models::floorContext, glm::mat4(), texture::floor, 30);
	drawObjectTexture(models::blanketContext, glm::mat4(), texture::blanket, 20);
	drawObjectTexture(models::wall_bedContext, glm::mat4(), texture::marble, 1);
	drawObjectTexture(models::pillowsContext, glm::mat4(), texture::fabric, 1);
	drawObjectTexture(models::chairContext, glm::mat4(), texture::smth, 1);
	drawObjectTexture(models::deskContext, glm::mat4(), texture::smth, 1);
	drawObjectTexture(models::cabinetContext, glm::mat4(), texture::smth, 1);


	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, (float)glfwGetTime() * 2, glm::vec3(0.0f, 1.0f, 0.0f));
	//drawObjectTexture(models::ceilingfanContext, trans, texture::kot, 20);
	//drawObjectTexture(models::tableContext, glm::mat4(), texture::table, 20);
	//drawObjectTexture(models::doorContext, glm::mat4(), texture::metal, 10);
	//drawObjectTexture(models::window1Context, glm::mat4(), texture::table, 1);
	drawObjectTexture(models::blanketContext, glm::mat4(), texture::blanket, 1);
	drawObjectTexture(models::pillowsContext, glm::mat4(), texture::fabric, 1);
	drawObjectTexture(models::cabinetContext, glm::mat4(), texture::smth, 1);
	drawObjectTexture(models::chairContext, glm::mat4(), texture::smth, 1);
	drawObjectTexture(models::deskContext, glm::mat4(), texture::smth, 1);

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});

	drawObjectPBR(shipContext,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.03f)),
		glm::vec3(0.3, 0.3, 0.5),
		0.2, 1.0
	);

	spotlightPos = spaceshipPos + 0.2 * spaceshipDir;
	spotlightConeDir = spaceshipDir;





	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_9_1.vert", "shaders/shader_9_1.frag");
	programTest = shaderLoader.CreateProgram("shaders/test.vert", "shaders/test.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_8_sun.vert", "shaders/shader_8_sun.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);


	/*loadModelToContext("./models/bed.obj", models::bedContext);
	loadModelToContext("./models/materace.obj", models::materaceContext);
	loadModelToContext("./models/table.obj", models::tableContext);
	loadModelToContext("./models/door.obj", models::doorContext);
	loadModelToContext("./models/plane.obj", models::planeContext);
	loadModelToContext("./models/room.obj", models::roomContext);
	loadModelToContext("./models/spaceship.obj", models::spaceshipContext);
	loadModelToContext("./models/window1.obj", models::window1Context);
	loadModelToContext("./models/window2.obj", models::window2Context);
	loadModelToContext("./models/window3.obj", models::window3Context);
	loadModelToContext("./models/test.obj", models::testContext);
	loadModelToContext("./models/lamp.obj", models::lampContext);
	loadModelToContext("./models/ceilingfan.obj", models::ceilingfanContext);
	loadModelToContext("./models/chair1.obj", models::chair1Context);
	loadModelToContext("./models/chair2.obj", models::chair2Context);
	loadModelToContext("./models/wall1.obj", models::wall1Context);
	loadModelToContext("./models/wall2.obj", models::wall2Context);
	loadModelToContext("./models/wall3.obj", models::wall3Context);
	loadModelToContext("./models/wall4.obj", models::wall4Context);*/
	loadModelToContext("./models/bed.obj", models::bedContext);
	loadModelToContext("./models/blanket.obj", models::blanketContext);
	loadModelToContext("./models/cabinet.obj", models::cabinetContext);
	loadModelToContext("./models/candle.obj", models::candleContext);
	loadModelToContext("./models/chair.obj", models::chairContext);
	loadModelToContext("./models/curtains.obj", models::curtainsContext);
	loadModelToContext("./models/desk.obj", models::deskContext);
	loadModelToContext("./models/door.obj", models::doorContext);
	loadModelToContext("./models/door_panel.obj", models::door_panelContext);
	loadModelToContext("./models/floor.obj", models::floorContext);
	loadModelToContext("./models/pad.obj", models::padContext);
	loadModelToContext("./models/pillows.obj", models::pillowsContext);
	loadModelToContext("./models/wall_bed.obj", models::wall_bedContext);
	loadModelToContext("./models/walls.obj", models::wallsContext);

	/*texture::kot = Core::LoadTexture("./textures/toksa2.png");
	texture::wood_floor = Core::LoadTexture("./textures/wood_floor.jpg");
	texture::table = Core::LoadTexture("./textures/table.jpg");
	texture::wallpaper = Core::LoadTexture("./textures/wallpaper.jpg");
	texture::materace = Core::LoadTexture("./textures/materace.jpg");
	texture::chairs = Core::LoadTexture("./textures/chairs.jpg");
	texture::metal = Core::LoadTexture("./textures/metal.jpg");
	texture::bed = Core::LoadTexture("./textures/bed.jpg");*/

	texture::floor = Core::LoadTexture("./textures/floor.jpg");
	texture::marble = Core::LoadTexture("./textures/marble.jpg");
	texture::fabric = Core::LoadTexture("./textures/fabric.jpg");
	texture::blanket = Core::LoadTexture("./textures/blanket.jpg");
	texture::smth = Core::LoadTexture("./textures/smth.jpg");

}


void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f * deltaTime * 60;
	float moveSpeed = 0.05f * deltaTime * 60;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		spaceshipPos += spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		spaceshipPos -= spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		spaceshipPos += spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		spaceshipPos -= spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		spaceshipPos += spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		spaceshipPos -= spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));

	cameraPos = spaceshipPos - 0.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.2f;
	cameraDir = spaceshipDir;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		printf("spaceshipPos = glm::vec3(%ff, %ff, %ff);\n", spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
		printf("spaceshipDir = glm::vec3(%ff, %ff, %ff);\n", spaceshipDir.x, spaceshipDir.y, spaceshipDir.z);
	}



}
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
