#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../shading/shaders.h"
#include "framebuffer.h"
#include <random>
#include "postprocess.h"

using namespace glm;

class SSAO {
public:
    inline static FrameBuffer ssaoFBO, ssaoBlurFBO;
    inline static vector<vec3> ssaoKernel;
    inline static GLuint noiseTexture;

    static void generateSSAO(GLuint gPosition, GLuint gNormal, mat4 projection) {
        ssaoFBO.bind();

        Shaders::SSAO.use();
        // Send kernel + rotation 
        for (unsigned int i = 0; i < 64; ++i)
            Shaders::SSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        Shaders::SSAO.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        Shaders::SSAO.setInt("gPosition",0);
        Shaders::SSAO.setInt("gNormal", 1);
        Shaders::SSAO.setInt("noiseTexture", 2);

        Objects::screen.draw();
    }

    static void blurSSAO() {
        ssaoBlurFBO.bind();
        Shaders::SSAOBlur.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoFBO.buffers[0]);
        Objects::screen.draw();
    }

    static void generate_SampleKernal_NoiseTexture() {
        // generate sample kernel
        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
        std::default_random_engine generator;
        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0f;

            // scale samples s.t. they're more aligned to center of kernel
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }


        // generate noise texture
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssaoNoise.push_back(noise);
        }
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    static float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }


public:
    static void setup(GLuint width , GLuint height) {
		ssaoFBO = FrameBuffer(width, height, 0, { {GL_R16F,GL_NEAREST,GL_CLAMP_TO_EDGE} });
		ssaoBlurFBO = FrameBuffer(width, height, 0, { {GL_R16F,GL_NEAREST,GL_CLAMP_TO_EDGE} });
        generate_SampleKernal_NoiseTexture();
	}

    static GLuint generateSSAOFactor(GLuint gPosition , GLuint gNormal, mat4 projection) {
        GLint currentFBO;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);

        generateSSAO(gPosition,gNormal,projection);
        blurSSAO();

        glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);

        return ssaoBlurFBO.buffers[0];
    }
};