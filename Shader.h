#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, identity
#include <glm/gtc/type_ptr.hpp>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "CommonValues.h"
#include "SpotLight.h"

#pragma once
class Shader
{
public:
	Shader();
	void CreateFromFiles(const char* vShader, const char* fShader);
	void UseShader();
	GLuint GetModelLocation();
	GLuint GetProjectionLocation();
	GLuint GetViewLocation();
	GLuint GetAmbientColourLocation();
	GLuint GetAmbientIntensityLocation();
	GLuint GetDiffuseIntensityLocation();
	GLuint GetDirectionLocation();
	GLuint GetSpecularIntensityLocation();
	GLuint GetShininessLocation();
	GLuint GetEyePositionLocation();

	void SetDirectionalLight(DirectionalLight * dLight);
	void SetPointLights(PointLight *pLight, unsigned int lightCount);
	void SetSpotLights(SpotLight *sLight, unsigned int lightCount);
	void SetTexture(GLuint textureUnit);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lTransform);

	~Shader();
private:
	int pointLightCount;
	int spotLightCount;

	GLuint shader, uniformModel, uniformProjection, uniformView, uniformEyePosition,
		uniformSpecularIntensity, uniformShininess,
		uniformTexture,
		uniformDirectionalLightTransform, uniformDirectionalShadowMap;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	GLuint uniformPointLightCount;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[MAX_POINT_LIGHTS];

	GLuint uniformSpotLightCount;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformDirection;
		GLuint uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

	std::string readShaderCodeFromFile(const char* shaderPath);
	void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
	void compileShaders(const char* vShaderCode, const char* fShaderCode);
};