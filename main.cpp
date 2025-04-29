#include <iostream>
#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lib/renderer/initiliaze.h"
#include "lib/renderer/renderer.h"
#include "lib/renderer/view/PerspectiveCamera.h"
#include "lib/renderer/geometry/BoxGeometry.h"
#include "lib/renderer/geometry/ConeGeometry.h"
#include "lib/renderer/texture/Texture.h"
#include "lib/renderer/light/PointLight.h"
#include "lib/renderer/light/RectAreaLight.h"
#include "lib/renderer/light/SpotLight.h"



int main() {
	GLFWwindow* window = initGL(800, 800);
	Renderer renderer = Renderer(window,800,800);
	Controls::initiliaze(window);


	
	Texture* t = Texture::T_2D("assets/2D/brickwall.jpg");

	Texture* albedo = Texture::T_2D("assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Color.jpg" ,true);
	Texture* normal = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_NormalGL.jpg");
	Texture* displacment = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Displacement.jpg");
	Texture* roughness = Texture::T_2D( "assets/PBR/PavingStones115/PavingStones115B_1K-JPG_Roughness.jpg");
	Texture* metallic = Texture::T_2D( "assets/PBR/PavingStones/Metal049A_2K-JPG_Metalness.jpg");

	Material* material = new Material(albedo,normal,displacment,NULL,roughness);
	float uRepeat = 30.0f , vRepeat = 30.0f;
	
	


	Object3D* space = new Object3D{};

	Mesh* floor = new Mesh{
		new BoxGeometry{1,1,1},
		material
	};
	floor->rotate(vec3(radians(90.0f),0,0));
	space->add(floor);


	//Model* model = new Model("assets/models/m/scene.gltf", false);
	//space->add(model);

	
	/*
	PointLight* l1 = new PointLight(
		vec3(1.0f),
		100.0f,
		25.0f
	);
	l1->translate(vec3(0,10,0));
	//space->add(l1);
	
	
	RectAreaLight* l2 = new RectAreaLight(
		vec3(1.0f),
		1.0f,
		20,
		20,
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
		40.0f
	);
	l3->translate(vec3(0, 10, 0));
	l3->rotate(vec3(radians(-90.0f),0,0));
	//l3->rotate(vec3(0, 0, radians(-30.0f)));
	//space->add(l3);
	*/



	PerspectiveCamera* camera = new PerspectiveCamera{};
	camera->attachControls();
	camera->translate(vec3(0,5,5));
	//space->add(camera);


	Renderer::lighting->ibl->generateMaps(Texture::T_HDRI("assets/HDRI/container_free_Ref.hdr"));


	auto animationLoop = [&](float deltaTime) {
		renderer.render(space,camera);


		/*
		glViewport(0, 0, 800, 800);
		camera->updateViewProjectionMatrix();

		Shaders::Skybox->use();
		Shaders::Skybox->setMat4("mviewProjectionMatrix",camera->projection * mat4(mat3(camera->view)));
		Shaders::Skybox->setInt("cube", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Renderer::lighting->ibl->maps[0]);
		Shaders::unitBox->render(Shaders::Skybox);
		*/

		/*
		Shaders::PostProcessing->use();
		Shaders::PostProcessing->setInt("buffer", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, maps[2]);
		Shaders::screen->render(Shaders::PostProcessing);
		*/
	};

	renderer.setAnimationLoop(animationLoop);
}