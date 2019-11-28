#pragma once

#include <GL/glew.h>

#include <string>

class Shader
{

private:

	GLuint m_id = 0;
	GLenum m_type = GL_VERTEX_SHADER;
	std::string m_source;
	std::string m_errorMessage;

public:

	Shader() = default;

	Shader(GLenum type, const std::string& source);

	Shader(const Shader& other);

	Shader(Shader&& other);

	Shader& operator=(const Shader& other);

	Shader& operator=(Shader&& other);

	~Shader();

	bool compile(GLenum type, const std::string& source);

	void clear();

	bool isCompiled() const;

	GLuint getId() const;

	GLenum getType() const;

	const std::string& getSource() const;

	const std::string& getErrorMessage() const;

};
