
#include "Shader.h"

Shader::Shader(GLenum type, const std::string& source)
{
	compile(type, source);
}

Shader::Shader(const Shader& other)
{
	compile(other.m_type, other.m_source);
}

Shader::Shader(Shader&& other)
{
	m_id = other.m_id;
	m_type = other.m_type;
	m_source = std::move(other.m_source);
	m_errorMessage = std::move(other.m_errorMessage);
	
	other.m_id = 0;
}

Shader& Shader::operator=(const Shader& other)
{
	if (this == &other)
	{
		return *this;
	}

	compile(other.m_type, other.m_source);

	return *this;
}

Shader& Shader::operator=(Shader&& other)
{
	if (this == &other)
	{
		return *this;
	}

	clear();

	m_id = other.m_id;
	m_type = other.m_type;
	m_source = std::move(other.m_source);
	m_errorMessage = std::move(other.m_errorMessage);

	other.m_id = 0;

	return *this;
}

Shader::~Shader()
{
	clear();
}

bool Shader::compile(GLenum type, const std::string& source)
{
	clear();

	m_id = glCreateShader(type);
	m_type = type;
	m_source = source;

	const char* source_c_str = m_source.c_str();
	glShaderSource(m_id, 1, &source_c_str, NULL);

	glCompileShader(m_id);

	int compileStatus;
	glGetShaderiv(m_id, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* infoLog = new GLchar[logSize];
		glGetShaderInfoLog(m_id, logSize, NULL, infoLog);
		m_errorMessage = infoLog;
		delete[] infoLog;
		glDeleteShader(m_id);
		m_id = 0;
		m_source = "";

		return false;
	}
	else
	{
		m_errorMessage = "";

		return true;
	}
}

void Shader::clear()
{
	if (isCompiled())
	{
		glDeleteShader(m_id);
		m_id = 0;
	}
	m_source = "";
	m_errorMessage = "";
}

bool Shader::isCompiled() const
{
	return m_id != 0;
}

GLuint Shader::getId() const
{
	return m_id;
}

GLenum Shader::getType() const
{
	return m_type;
}

const std::string& Shader::getSource() const
{
	return m_source;
}

const std::string& Shader::getErrorMessage() const
{
	return m_errorMessage;
}
