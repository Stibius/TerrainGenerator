#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders)
{
	linkShaders(shaders);
}

ShaderProgram::ShaderProgram(const std::vector<Shader>& shaders)
{
	linkShaders(shaders);
}

ShaderProgram::ShaderProgram(const ShaderProgram& other)
{
	linkShaders(other.m_shaders);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
{
	m_id = other.m_id;
	m_shaders = std::move(other.m_shaders);
	m_errorMessage = std::move(other.m_errorMessage);

	other.m_id = 0;
}

ShaderProgram& ShaderProgram::operator=(const ShaderProgram& other)
{
	if (this == &other)
	{
		return *this;
	}

	linkShaders(other.m_shaders);

	return *this;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
{
	if (this == &other)
	{
		return *this;
	}

	clear();

	m_id = other.m_id;
	m_shaders = std::move(other.m_shaders);
	m_errorMessage = std::move(other.m_errorMessage);

	other.m_id = 0;

	return *this;
}

ShaderProgram::~ShaderProgram()
{
	clear();
}

bool ShaderProgram::linkShaders(std::initializer_list<Shader> shaders)
{
	return linkShaders(std::vector<Shader>(shaders));
}

bool ShaderProgram::linkShaders(const std::vector<Shader>& shaders)
{
	clear();

	m_id = glCreateProgram();
	m_shaders = shaders;

	for (int i = 0; i < m_shaders.size(); i++)
	{
		glAttachShader(m_id, m_shaders[i].getId());
	}

	glLinkProgram(m_id);

	int linkStatus;
	glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE)
	{
		GLint logSize = 0;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* infoLog = new GLchar[logSize];
		glGetProgramInfoLog(m_id, logSize, NULL, infoLog);
		m_errorMessage = infoLog;
		delete[] infoLog;
		for (int i = 0; i < m_shaders.size(); i++)
		{
			glDetachShader(m_id, m_shaders[i].getId());
		}
		m_shaders.clear();
		glDeleteProgram(m_id);
		m_id = 0;

		return false;
	}
	else
	{
		m_errorMessage = "";

		return true;
	}
}

void ShaderProgram::setVertexAttribPointer(
	const std::string& name, 
	GLint size, 
	GLenum type, 
	GLboolean normalized, 
	GLsizei stride, 
	const GLvoid* pointer) const
{
	GLint location = getAttribLocation(name);
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, size, type, normalized, stride, pointer);
}

void ShaderProgram::setUniform(const std::string& name, GLsizei count, GLboolean transpose, glm::mat4 matrix) const
{
	GLint location = getUniformLocation(name);
	glUniformMatrix4fv(location, count, transpose, glm::value_ptr(matrix));
}

void ShaderProgram::setUniform(const std::string& name, GLsizei count, glm::vec3 value) const
{
	GLint location = getUniformLocation(name);
	glUniform3fv(location, count, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, GLint value) const
{
	GLint location = getUniformLocation(name);
	glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string& name, GLfloat value) const
{
	GLint location = getUniformLocation(name);
	glUniform1f(location, value);
}

void ShaderProgram::setUniform(GLint location, GLsizei count, GLboolean transpose, glm::mat4 matrix) const
{
	glUniformMatrix4fv(location, count, transpose, glm::value_ptr(matrix));
}

void ShaderProgram::setUniform(GLint location, GLsizei count, glm::vec3 value) const
{
	glUniform3fv(location, count, glm::value_ptr(value));
}

void ShaderProgram::setUniform(GLint location, GLint value) const
{
	glUniform1i(location, value);
}

void ShaderProgram::setUniform(GLint location, GLfloat value) const
{
	glUniform1f(location, value);
}

void ShaderProgram::use() const
{
	if (isLinked())
	{
		glUseProgram(m_id);
	}
}

void ShaderProgram::clear()
{
	if (isLinked())
	{
		for (int i = 0; i < m_shaders.size(); i++)
		{
			glDetachShader(m_id, m_shaders[i].getId());
		}
		glDeleteProgram(m_id);
		m_id = 0;
	}
	m_shaders.clear();
	m_errorMessage = "";
}

bool ShaderProgram::isLinked() const
{
	return m_id != 0;
}

const std::vector<Shader>& ShaderProgram::getShaders() const
{
	return m_shaders;
}

const std::string& ShaderProgram::getErrorMessage() const
{
	return m_errorMessage;
}

GLint ShaderProgram::getAttribLocation(const std::string& name) const
{
	return glGetAttribLocation(m_id, name.c_str());
}

GLint ShaderProgram::getUniformLocation(const std::string& name) const
{
	return glGetUniformLocation(m_id, name.c_str());
}
