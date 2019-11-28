#pragma once

#include <vector>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderProgram
{
private:

	GLuint m_id = 0;
	std::vector<Shader> m_shaders;
	std::string m_errorMessage;

public:

	ShaderProgram() = default;

	ShaderProgram(std::initializer_list<Shader> shaders);

	ShaderProgram(const std::vector<Shader>& shaders);

	ShaderProgram(const ShaderProgram& other);

	ShaderProgram(ShaderProgram&& other);

	ShaderProgram& operator=(const ShaderProgram& other);

	ShaderProgram& operator=(ShaderProgram&& other);

	~ShaderProgram();

	bool linkShaders(std::initializer_list<Shader> shaders);

	bool linkShaders(const std::vector<Shader>& shaders);

	void setVertexAttribPointer(
		const std::string& name, 
		GLint size, 
		GLenum type, 
		GLboolean normalized,
		GLsizei stride,
		const GLvoid* pointer) const;

	void setUniform(const std::string& name, GLsizei count, GLboolean transpose, glm::mat4 matrix) const;

	void setUniform(const std::string& name, GLsizei count, glm::vec3 value) const;

	void setUniform(const std::string& name, GLint value) const;

	void setUniform(const std::string& name, GLfloat value) const;

	void setUniform(GLint location, GLsizei count, GLboolean transpose, glm::mat4 matrix) const;

	void setUniform(GLint location, GLsizei count, glm::vec3 value) const;

	void setUniform(GLint location, GLint value) const;

	void setUniform(GLint location, GLfloat value) const;

	void use() const;

	void clear();

	bool isLinked() const;

	const std::vector<Shader>& getShaders() const;

	const std::string& getErrorMessage() const;

	GLint getAttribLocation(const std::string& name) const;

	GLint getUniformLocation(const std::string& name) const;
};
