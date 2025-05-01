#include <iostream>
#include "lib/renderer/initiliaze.h"
#include "lib/renderer/renderer.h"
#include "lib/renderer/view/PerspectiveCamera.h"
#include "lib/renderer/geometry/BoxGeometry.h"
#include "lib/renderer/texture/Texture.h"
#include "lib/renderer/light/PointLight.h"
#include "lib/renderer/light/RectAreaLight.h"
#include "lib/renderer/light/SpotLight.h"



int main() {
	GLFWwindow* window = initGL(1000, 1000);
	Renderer renderer = Renderer(window,1000,1000);
	Controls::initiliaze(window);



	Texture* albedo = Texture::T_2D("assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Color.jpg" ,true);
	Texture* normal = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_NormalGL.jpg");
	Texture* displacment = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Displacement.jpg");
	Texture* roughness = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Roughness.jpg");
	Texture* metallic = Texture::T_2D( "assets/PBR/PavingStones/Metal049A_2K-JPG_Metalness.jpg");

	Material* material = new Material(albedo,normal,displacment,NULL,roughness);
	material->uvMultiplier = vec2(30.0f,30.0f);
	
	

	 
	Object3D* space = new Object3D{};

	Mesh* floor = new Mesh{
		new BoxGeometry{100,100,0.1},
		material
	};
	floor->rotate(vec3(radians(90.0f),0,0));
	space->add(floor);


	Model* model = new Model("assets/models/h/adamHead.gltf", false);
	model->scale(vec3(10));
	model->translate(vec3(0, 3, 0));
	space->add(model);

	
	
	PointLight* l1 = new PointLight(
		vec3(1.0f),
		100.0f,
		15.0f
	);
	l1->translate(vec3(0,10,0));
	//space->add(l1);
	
	
	RectAreaLight* l2 = new RectAreaLight(
		vec3(1.0f),
		1.0f,
		50,
		50,
		20,
		true
	);
	l2->translate(vec3(0, 10, 0));
	l2->rotate(vec3(radians(-90.0f), 0, 0));
	//space->add(l2);
	
	
	SpotLight* l3 = new SpotLight(
		vec3(1.0f),
		50.0f,
		radians(35.0f),
		radians(15.0f),
		15.0f
	);
	l3->translate(vec3(0, 10, 0));
	l3->rotate(vec3(radians(-90.0f),0,0));
	//l3->rotate(vec3(0, 0, radians(-30.0f)));
	//space->add(l3);
	



	PerspectiveCamera* camera = new PerspectiveCamera{};
	camera->attachControls();
	camera->translate(vec3(0,5,5));
	//space->add(camera);


	renderer.ibl->generateMaps(Texture::T_HDRI("assets/HDRI/overcast_soil_puresky_4k.hdr"));


	auto animationLoop = [&](float deltaTime) {
		renderer.render(space,camera);


	};

	renderer.setAnimationLoop(animationLoop);
}