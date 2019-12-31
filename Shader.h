#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>;

#pragma once
class Shader
{
public:
	Shader();
	void CreateFromFiles(const char* vShader, const char* fShader);
	void UseShader() { glUseProgram(this->shader); }
	GLuint GetModelLocation() { return this->uniformModel; }
	GLuint GetProjectionLocation() { return this->uniformProjection; }
	GLuint GetViewLocation() { return this->uniformView;  }
	GLuint GetAmbientColourLocation() { return this->uniformAmbientColour; }
	GLuint GetAmbientIntensityLocation() { return this->uniformAmbientIntensity; }
	GLuint GetDiffuseIntensityLocation() { return this->uniformDiffuseIntensity; }
	GLuint GetDirectionLocation() { return this->uniformDirection; }
	GLuint GetSpecularIntensityLocation() { return this->uniformSpecularIntensity; }
	GLuint GetShininessLocation() { return this->uniformShininess; }
	GLuint GetEyePositionLocation() { return this->uniformEyePosition; }

	~Shader();
private:
	GLuint shader, uniformModel, uniformProjection, uniformView, uniformAmbientIntensity, 
		uniformAmbientColour, uniformDiffuseIntensity, uniformDirection, uniformEyePosition,
		uniformSpecularIntensity, uniformShininess;
	std::string readShaderCodeFromFile(const char* shaderPath);
	void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
	void compileShaders(const char* vShaderCode, const char* fShaderCode);
};