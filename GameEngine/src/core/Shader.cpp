#include "Shader.h"


Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	:m_VertexShaderPath(vertexShaderPath), m_FragmentShaderPath(fragmentShaderPath)
{
    std::string vertexShaderSource = ParseShader(vertexShaderPath);
    std::string fragmentShaderSource = ParseShader(fragmentShaderPath);

    if (vertexShaderSource.empty() || fragmentShaderSource.empty())
    {
        std::cout << "ERROR\nREADING SHADER FILE\n";
        system("pause");
    }

    m_RendererID = CreateShader(vertexShaderSource, fragmentShaderSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

std::string Shader::ParseShader(const std::string& filepath) const
{
    std::ifstream stream(filepath);

    if (!stream)
    {
        std::cout << "Wrong shader Path!" << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << stream.rdbuf();
    stream.close();

    return ss.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) const
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)_malloca(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
            " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) const
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::LINK::PROGRAM\n" << infoLog << std::endl;
    }

    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

/*
* Parâmetros de glUniform:
* Localização do Uniform: Só podemos chamar glUniform depois de termos nosso
* shader Program, pois quando ele eh compilado, OpenGL reserva uma posição na
* MEM para aquele Uniform, e precisamos saber qual eh essa localização, então
* precisamos de um PONTEIRO para aquele objeto, e podemos perguntar ao OpenGL
* qual a localização desse Uniform pelo nome.
*/
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& v)
{
    glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
    glUniform1i(GetUniformLocation(name), v0);
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
    glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat4)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::SetUniformMaterial(const Material& mat)
{
    this->SetUniform3f("material.ambient", mat.ambient);
    this->SetUniform3f("material.diffuse", mat.diffuse);
    this->SetUniform3f("material.specular", mat.specular);
    this->SetUniform1f("material.shininess", mat.shininess);
}

void Shader::SetUniformLight(const Light& light)
{
    this->SetUniform3f("light.ambient",   light.ambient);
    this->SetUniform3f("light.diffuse",   light.diffuse);
    this->SetUniform3f("light.specular",  light.specular);
    this->SetUniform3f("light.position",  light.position);
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.contains(name))
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "\033[1;33m" << "[WARNING]\nUniform " << "\033[1;34m" << name << "\033[1;33m" << " was not found" << "\033[0m" << "\n";
    
    m_UniformLocationCache[name] = location;
    return location;
}
