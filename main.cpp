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

	Texture* albedo = new Texture{"assets/PBR/PavingStones/PavingStones138_2K-JPG_Color.jpg" };
	Texture* normal = new Texture{ "assets/PBR/PavingStones/PavingStones138_2K-JPG_NormalGL.jpg" };
	Texture* displacment = new Texture{ "assets/PBR/PavingStones/PavingStones138_2K-JPG_Displacement.jpg"};
	Texture* roughness = new Texture{ "assets/PBR/PavingStones/PavingStones138_2K-JPG_Roughness.jpg" };
	Texture* metallic = new Texture{ "assets/PBR/PavingStones/Metal049A_2K-JPG_Metalness.jpg" };

	Object3D* space = new Object3D{};

	Mesh* b = new Mesh{
		new SpheroidGeometry{1.5,1.5,1.5},
		//new Material{vec3(1.0f,0,0),0.5f,1.0f,0.0f}
		new Material{albedo,roughness,nullptr,normal,displacment,0.2f}
	};
	space->add(b);


	/*
	PointLight* l1 = new PointLight(
		vec3(1.0f),
		15.0f,
		15.0f
	);
	l1->translate(vec3(2,2,1));
	space->add(l1);

	PointLight* l2 = new PointLight(
		vec3(1.0f),
		15.0f,
		10.0f
	);
	l2->translate(vec3(-2, 2, 1));
	space->add(l2);

	PointLight* l3 = new PointLight(
		vec3(1.0f),
		15.0f,
		10.0f
	);
	l3->translate(vec3(-2, -2, 1));
	space->add(l3);
	
	PointLight* l4 = new PointLight(
		vec3(1.0f),
		15.0f,
		10.0f
	);
	l4->translate(vec3(2, -2, 1));
	space->add(l4);
	
	*/
	RectAreaLight* l5 = new RectAreaLight(
		vec3(1.0f),
		15.0f,
		vec3(-1,0,0),
		4,
		4,
		4
	);
	l5->translate(vec3(4, 0, 0));
	space->add(l5);
	

	RectAreaLight* l6 = new RectAreaLight(
		vec3(1.0f),
		15.0f,
		vec3(1, 0, 0),
		4,
		4,
		4
	);
	l6->translate(vec3(-4, 0, 0));
	space->add(l6);
	
	

	/*
	SpotLight* l7 = new SpotLight(
		vec3(1.0f),
		15.0f,
		vec3(-1, 0, 0),
		radians(30.0f),
		radians(45.0f),
		5.0f
	);
	l7->translate(vec3(4, 0, 0));
	space->add(l7);
	*/
	


	PerspectiveCamera* camera = new PerspectiveCamera{};
	camera->attachControls();
	camera->translate(vec3(0,0,5));
	//space->add(camera);


	auto animationLoop = [&]() {
		//space->rotate(vec3(0, radians(0.1f), 0));
		//l->rotate(vec3(0, radians(0.1f), 0));
		//space->translate(vec3(0.02,0,0));
		//space->rotate(vec3(0, 0.1,0));
		//b->rotate(vec3(0, radians(0.1f), 0));
		//camera.rotate(vec3(0, 0.001,0));
		//space->scale(vec3(1.001));
		renderer.render(space,camera);
	};

	renderer.setAnimationLoop(animationLoop);
}