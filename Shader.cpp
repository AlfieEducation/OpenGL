﻿#include "Shader.h"

Shader::Shader()
{
	shader = 0;
	uniformModel = 0;
	uniformProjection = 0;

	pointLightCount = 0;
	spotLightCount = 0;
}

void Shader::CreateFromFiles(const char* vShader, const char* fShader)
{
	std::string vShaderCode = readShaderCodeFromFile(vShader);
	std::string fShaderCode = readShaderCodeFromFile(fShader);

	compileShaders(vShaderCode.c_str(), fShaderCode.c_str());
}


Shader::~Shader()
{
}

std::string Shader::readShaderCodeFromFile(const char* shaderPath)
{
	std::string code;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		// Read data over stream
		shaderStream << shaderFile.rdbuf();
		// Close files
		shaderFile.close();
		// Convert data stream to GLchar
		code = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Shader file " << shaderPath << " cannot be read" << std::endl;
	}

	return code;
}

void Shader::addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar errLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(errLog), NULL, errLog);
		std::cerr << "Error compiling the " << shaderType << " shader: '" << errLog << "'\n";
		return;
	}

	glAttachShader(theProgram, theShader);
}

void Shader::SetTexture(GLuint textureUnit)
{
	glUniform1i(uniformTexture, textureUnit);
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit)
{
	glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::SetDirectionalLightTransform(glm::mat4 * lTransform)
{
	glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lTransform));
}

void Shader::UseShader() 
{
	glUseProgram(this->shader); 
}

GLuint Shader::GetModelLocation() 
{ 
	return this->uniformModel; 
}

GLuint Shader::GetProjectionLocation() 
{ 
	return this->uniformProjection; 
}

GLuint Shader::GetViewLocation() 
{ 
	return this->uniformView; 
}

GLuint Shader::GetAmbientColourLocation() 
{ 
	return this->uniformDirectionalLight.uniformColour; 
}

GLuint Shader::GetAmbientIntensityLocation() 
{ 
	return this->uniformDirectionalLight.uniformAmbientIntensity; 
}

GLuint Shader::GetDiffuseIntensityLocation() 
{ 
	return this->uniformDirectionalLight.uniformDiffuseIntensity; 
}

GLuint Shader::GetDirectionLocation() 
{ 
	return this->uniformDirectionalLight.uniformDirection; 
}

GLuint Shader::GetSpecularIntensityLocation() 
{ 
	return this->uniformSpecularIntensity; 
}

GLuint Shader::GetShininessLocation() 
{ 
	return this->uniformShininess; 
}

GLuint Shader::GetEyePositionLocation() 
{ 
	return this->uniformEyePosition; 
}

void Shader::SetDirectionalLight(DirectionalLight * dLight)
{
	dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColour,
		uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLights(PointLight *pLight, unsigned int lightCount)
{
	if (lightCount > MAX_POINT_LIGHTS) lightCount = MAX_POINT_LIGHTS;

	glUniform1i(uniformPointLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++)
	{
		pLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColour,
			uniformPointLight[i].uniformDiffuseIntensity, uniformPointLight[i].uniformPosition,
			uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);
	}
}

void Shader::SetSpotLights(SpotLight * sLight, unsigned int lightCount)
{
	if (lightCount > MAX_SPOT_LIGHTS) lightCount = MAX_SPOT_LIGHTS;

	glUniform1i(uniformSpotLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++)
	{
		sLight[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity, uniformSpotLight[i].uniformColour,
			uniformSpotLight[i].uniformDiffuseIntensity, uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformDirection,
			uniformSpotLight[i].uniformConstant, uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent,
			uniformSpotLight[i].uniformEdge);
	}
}

void Shader::compileShaders(const char* vShaderCode, const char* fShaderCode)
{
	shader = glCreateProgram();

	if (!shader) {
		std::cerr << "Error creating shader program\n";
		return;
	}

	addShader(shader, vShaderCode, GL_VERTEX_SHADER);
	addShader(shader, fShaderCode, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar errLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(errLog), NULL, errLog);
		std::cerr << "Error linking program: '" << errLog << "'\n";
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(errLog), NULL, errLog);
		std::cerr << "Error validating program: '" << errLog << "'\n";
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
	uniformProjection = glGetUniformLocation(shader, "projection");
	uniformView = glGetUniformLocation(shader, "view");

	uniformDirectionalLight.uniformColour = glGetUniformLocation(shader, "directionalLight.base.colour");
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shader, "directionalLight.base.ambientIntensity");

	uniformDirectionalLight.uniformDirection = glGetUniformLocation(shader, "directionalLight.direction");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shader, "directionalLight.base.diffuseIntensity");

	uniformSpecularIntensity = glGetUniformLocation(shader, "material.specularIntensity");
	uniformShininess = glGetUniformLocation(shader, "material.shininess");
	uniformEyePosition = glGetUniformLocation(shader, "eyePosition");

	uniformPointLightCount = glGetUniformLocation(shader, "pointLightCount");

	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		char locBuff[100] = { '\0' };
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.colour", i);
		uniformPointLight[i].uniformColour = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shader, locBuff);
		
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shader, locBuff);
		

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
		uniformPointLight[i].uniformExponent = glGetUniformLocation(shader, locBuff);
	}

	uniformSpotLightCount = glGetUniformLocation(shader, "spotLightCount");

	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
	{
		char locBuff[100] = { '\0' };
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.colour", i);
		uniformSpotLight[i].uniformColour = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shader, locBuff);


		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shader, locBuff);
	}

	uniformTexture = glGetUniformLocation(shader, "theTexture");
	uniformDirectionalLightTransform = glGetUniformLocation(shader, "directionalLightTransform");
	uniformDirectionalShadowMap = glGetUniformLocation(shader, "directionalShadowMap");

}