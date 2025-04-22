#include <iostream>
#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lib/renderer/core/initiliaze.h"
#include "lib/renderer/core/renderer.h"
#include "lib/renderer/core/view/PerspectiveCamera.h"
#include "lib/renderer/core/geometry/BoxGeometry.h"
#include "lib/renderer/core/geometry/ConeGeometry.h"
#include "lib/renderer/core/texture/Texture.h"
#include "lib/renderer/core/light/PointLight.h"
#include "lib/renderer/core/light/RectAreaLight.h"
#include "lib/renderer/core/light/SpotLight.h"



int main() {
	GLFWwindow* window = initGL(800, 800);
	Renderer renderer = Renderer(window,800,800);
	Controls::initiliaze(window);


	Texture* t = new Texture{"assets/2D/brickwall.jpg"};

	Texture* albedo = new Texture{"assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Color.jpg" };
	Texture* normal = new Texture{ "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_NormalGL.jpg" };
	Texture* displacment = new Texture{ "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Displacement.jpg"};
	Texture* roughness = new Texture{ "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Roughness.jpg" };
	Texture* metallic = new Texture{ "assets/PBR/PavingStones/Metal049A_2K-JPG_Metalness.jpg" };

	float uRepeat = 3.0f , vRepeat = 3.0f;
	albedo->repeat(uRepeat, vRepeat);
	normal->repeat(uRepeat, vRepeat);
	displacment->repeat(uRepeat, vRepeat);
	roughness->repeat(uRepeat, vRepeat);
	metallic->repeat(uRepeat, vRepeat);

	Object3D* space = new Object3D{};

	Mesh* b = new Mesh{
		new BoxGeometry{10,10,0.05},
		//new Material{vec3(1.0f,0,0),0.5f,1.0f,0.0f}
		new Material{albedo,roughness,nullptr,normal,displacment,0.125f}
	};
	b->rotate(vec3(radians(90.0f),0,0));
	space->add(b);

	
	PointLight* l1 = new PointLight(
		vec3(1.0f),
		25.0f,
		15.0f
	);
	l1->translate(vec3(0,5,0));
	//space->add(l1);
	
	
	RectAreaLight* l2 = new RectAreaLight(
		vec3(1.0f),
		100.0f,
		vec3(-1,-1,1),
		40,
		40,
		40
	);
	l2->translate(vec3(5, 10, -5));
	//space->add(l2);
	
	
	SpotLight* l3 = new SpotLight(
		vec3(1.0f),
		90.0f,
		vec3(-1, -1, 1),
		radians(45.0f),
		radians(25.0f),
		15.0f
	);
	l3->translate(vec3(5, 5, -5));
	space->add(l3);
	
	


	PerspectiveCamera* camera = new PerspectiveCamera{};
	camera->attachControls();
	camera->translate(vec3(0,5,5));
	//space->add(camera);


	auto animationLoop = [&](float deltaTime) {
		//space->rotate(vec3(0, radians(0.1f), 0));
		//l7->rotate(vec3(0, radians(0.1f), 0));
		//space->translate(vec3(0.02,0,0));
		//space->rotate(vec3(0, 0.1,0));
		//b->rotate(vec3(0, radians(0.1f), 0));
		//camera.rotate(vec3(0, 0.001,0));
		//space->scale(vec3(1.001));
		renderer.render(space,camera);
	};

	renderer.setAnimationLoop(animationLoop);
}