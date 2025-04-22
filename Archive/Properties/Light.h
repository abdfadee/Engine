#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>
#include "../include/ShadowMapping.h"


using namespace glm;
using namespace std;


class Light {
public:
	vec3 ambient;
	vec3 diffuse;
	vec3 speculer;

	Light(){};

	Light(vec3 ambient, vec3 diffuse, vec3 speculer) :
		ambient(ambient) ,
		diffuse(diffuse) ,
		speculer(speculer) {}


	void updateUniforms(int shaderId , string prefix) {
		string a = prefix + ".ambient", d = prefix + ".diffuse", s = prefix + ".speculer";
		glUniform3fv(glGetUniformLocation(shaderId, a.c_str()), 1, &this->ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderId, d.c_str()), 1, &this->diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderId, s.c_str()), 1, &this->speculer[0]);
	}

};



class DirectionalLight : public Light {
public:
	vec3 direction;
	vec3 virtualPosition;
	DirectionalShadowMapping shadowMapping;
	GLuint textureUnit;

	DirectionalLight(){}
	DirectionalLight(vec3 direction, vec3 ambient, vec3 diffuse, vec3 speculer,vec3 virtualPosition,vec3 shadowReach,GLuint textureUnit) :
		direction(direction), virtualPosition(virtualPosition), textureUnit(textureUnit),
		shadowMapping(DirectionalShadowMapping(1000,1000, shadowReach.x, shadowReach.y, shadowReach.z)),
		Light(ambient,diffuse,speculer) {}


	void updateUniforms(string prefix,void (*render)(Shader &shader)) {
		GLint shaderId;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shaderId);
		shadowMapping.updateDepthMap(virtualPosition,direction,render);
		glUseProgram(shaderId);
		Light::updateUniforms(shaderId, prefix);
		string dir = prefix + ".direction";
		string mat = prefix + ".spaceMatrix";
		string dep = prefix + ".depthMap";
		glUniform3fv(glGetUniformLocation(shaderId, dir.c_str()), 1, &this->direction[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, mat.c_str()), 1, GL_FALSE, value_ptr(shadowMapping.lightMatrix));
		glUniform1i(glGetUniformLocation(shaderId, dep.c_str()),textureUnit);
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, shadowMapping.fbo.buffers[0]);
	}

};



class PointLight : public Light {
public:
	float constant;
	float linear;
	float quadratic;
	vec3 position;
	OmnidirectionalShadowMapping shadowMapping;
	GLuint textureUnit;

	
	PointLight(){}
	PointLight(float constant , float linear , float quadratic , vec3 position, vec3 ambient, vec3 diffuse, vec3 speculer,float shadowReach,GLuint textureUnit) :
	constant(constant) ,
	linear(linear) ,
	quadratic(quadratic) ,
	position(position),
	shadowMapping(OmnidirectionalShadowMapping(1000,1000, shadowReach)),
	textureUnit(textureUnit),
	Light(ambient, diffuse, speculer) {}


	void updateUniforms(string prefix,void (*render)(Shader& shader)) {
		string c = prefix + ".constant", l = prefix + ".linear", q = prefix + ".quadratic", p = prefix + ".position", d = prefix + ".depthCubeMap", f = prefix + ".far_plane";
		GLint shaderId;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shaderId);
		shadowMapping.updateDepthCubeMap(position,render);
		glUseProgram(shaderId);
		Light::updateUniforms(shaderId, prefix);
		glUniform1f(glGetUniformLocation(shaderId, c.c_str()), this->constant);
		glUniform1f(glGetUniformLocation(shaderId, l.c_str()), this->linear);
		glUniform1f(glGetUniformLocation(shaderId, q.c_str()) ,this->quadratic);
		glUniform3fv(glGetUniformLocation(shaderId, p.c_str()), 1, value_ptr(this->position));
		glUniform1f(glGetUniformLocation(shaderId, f.c_str()), shadowMapping.frustumDepth);
		glUniform1i(glGetUniformLocation(shaderId, d.c_str()), textureUnit);
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMapping.fbo.buffers[0]);
	}

};



class SpotLight : public Light {
public:
	float innerCutOff;
	float outerCutOff;
	vec3 position;
	vec3 direction;
	DirectionalShadowMapping shadowMapping;
	GLuint textureUnit;


	SpotLight(){}
	SpotLight(float innerCutOff , float outerCutOff, vec3 position , vec3 direction , vec3 ambient, vec3 diffuse, vec3 speculer, vec3 shadowReach, GLuint textureUnit) :
		innerCutOff(cos(radians(innerCutOff))),		// cosine of cutOff angle
		outerCutOff(cos(radians(outerCutOff))),
		position(position),
		direction(direction),
		shadowMapping(DirectionalShadowMapping(1000, 1000, shadowReach.x, shadowReach.y, shadowReach.z)),
		textureUnit(textureUnit),
		Light(ambient, diffuse, speculer) {}


	void updateUniforms(string prefix, void (*render)(Shader& shader)) {
		string i = prefix + ".innerCutOff", o = prefix + ".outerCutOff", p = prefix + ".position", d = prefix + ".direction" , mat = prefix + ".spaceMatrix" , dep = prefix + ".depthMap";;
		GLint shaderId;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shaderId);
		shadowMapping.updateDepthMap(position, direction, render);
		glUseProgram(shaderId);
		Light::updateUniforms(shaderId, prefix);
		glUniform1f(glGetUniformLocation(shaderId, i.c_str()),this->innerCutOff);
		glUniform1f(glGetUniformLocation(shaderId, o.c_str()), this->outerCutOff);
		glUniform3fv(glGetUniformLocation(shaderId, p.c_str()), 1, value_ptr(this->position));
		glUniform3fv(glGetUniformLocation(shaderId, d.c_str()), 1, value_ptr(this->direction));
		glUniformMatrix4fv(glGetUniformLocation(shaderId, mat.c_str()), 1, GL_FALSE, value_ptr(shadowMapping.lightMatrix));
		glUniform1i(glGetUniformLocation(shaderId, dep.c_str()), textureUnit);
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, shadowMapping.fbo.buffers[0]);
	}

};