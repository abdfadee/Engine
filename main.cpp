#include <iostream>
#include<filesystem>
#include "lib/renderer/initiliaze.h"
#include "lib/renderer/renderer.h"
#include "lib/renderer/view/PerspectiveCamera.h"
#include "lib/renderer/geometry/BoxGeometry.h"
#include "lib/renderer/texture/Texture.h"
#include "lib/renderer/light/PointLight.h"
#include "lib/renderer/light/RectAreaLight.h"
#include "lib/renderer/light/SpotLight.h"
#include "lib/renderer/model/Model.h"
#include "lib/renderer/animation/Animator.h"


using namespace std;


int main() {
	GLFWwindow* window = initGL(1000, 1000);
	Renderer renderer = Renderer(window,1000,1000);
	Controls::initiliaze(window);



	Object3D* space = new Object3D{};

	Texture* albedo = Texture::T_2D("assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Color.jpg" ,true);
	Texture* normal = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_NormalGL.jpg");
	Texture* displacment = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Displacement.jpg");
	Texture* roughness = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Roughness.jpg");
	Texture* metallic = Texture::T_2D( "assets/PBR/PavingStones/Metal049A_2K-JPG_Metalness.jpg");

	Material* material = new Material(albedo,normal,displacment,NULL,roughness);
	material->uvMultiplier = vec2(300.0f,300.0f);

	Mesh* floor = new Mesh{
		new BoxGeometry{1000,1000,0.1},
		material
	};
	floor->rotate(vec3(radians(90.0f),0,0));
	space->add(floor);



	Model* model = new Model("assets/models/ca/scene.gltf", false);
	model->scale(vec3(0.001));
	//model->rotate(vec3(radians(90.0f), 0, 0));
	model->translate(vec3(0, 0.4, 0));
	space->add(model);


	Node* node = model->getNode("Object_6");
	Animator animator = Animator(model->animations[0]);
	
	



	PerspectiveCamera* camera = new PerspectiveCamera{};
	camera->attachControls();
	camera->translate(vec3(0,0.5,8));
	//space->add(camera);


	renderer.ibl->generateMaps(Texture::T_HDRI("assets/HDRI/kloppenheim_06_puresky_4k.hdr"));


	auto animationLoop = [&](float deltaTime) {
		animator.UpdateAnimation(deltaTime,model);

		renderer.render(space,camera);
	};

	renderer.setAnimationLoop(animationLoop);
}