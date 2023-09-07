#include "Shader.h"

static std::string to_string(ShaderStage ss)
{
    switch (ss)
    {
    case ShaderStage::VERTEX:
        return "vertex";
        break;
    case ShaderStage::FRAGMENT:
        return "fragment";
        break;
    default:
        std::cout << "ERROR: Invalid shader stage...\n";
    }
}

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	:m_VertexShaderPath(vertexShaderPath), m_FragmentShaderPath(fragmentShaderPath)
{
    std::string vertexShaderSource = ParseShader(vertexShaderPath);
    std::string fragmentShaderSource = ParseShader(fragmentShaderPath);

    if (vertexShaderSource.empty() || fragmentShaderSource.empty())
    {
        std::cout << "ERROR\nREADING SHADER FILE\n";
        __cpp_static_assert;
    }

    m_RendererID = CreateShader(vertexShaderSource, fragmentShaderSource);
}

Shader::~Shader()
{
    std::cout << "Deleting Shader "<< m_RendererID <<" of\n" << m_VertexShaderPath << "\n" << m_FragmentShaderPath << "\n";
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

void Shader::Unbind()
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

void Shader::SetUniform3f(const std::string& name, const cgl::vec3& v)
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

void Shader::SetUniformMatrix4fv(const std::string& name, const cgl::mat4& mat4)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, mat4.get_pointer());
}

unsigned int Shader::GetSubroutineIndex(ShaderStage shaderStage, const std::string& subroutineIndexName) const
{
    auto r = glGetSubroutineIndex(m_RendererID, (GLenum)shaderStage, subroutineIndexName.c_str());
#ifdef _DEBUG
    if (r == GL_INVALID_INDEX)
        std::cout << "INVALID SUBROUTINE INDEX\n";
#endif
    return r;
}

void Shader::SetUniformSubroutine(ShaderStage shaderStage, size_t count, unsigned int const* index) const
{
    glUniformSubroutinesuiv((GLenum)shaderStage, count, index);
}

void Shader::SetUniformMaterial(const Material& mat)
{
    this->SetUniform3f("material.ambient", mat.ambient);
    this->SetUniform3f("material.diffuse", mat.diffuse);
    this->SetUniform3f("material.specular", mat.specular);
    this->SetUniform1f("material.shininess", mat.shininess);
}

void Shader::SetUniformLight(const DirectionalLight& light, ShaderStage shaderStage)
{
    std::string stageName = to_string(shaderStage);
    this->SetUniform3f(stageName + "DirectionalLight.ambient", light.ambient);
    this->SetUniform3f(stageName + "DirectionalLight.diffuse", light.diffuse);
    this->SetUniform3f(stageName + "DirectionalLight.specular", light.specular);
    this->SetUniform3f(stageName + "DirectionalLight.direction", light.direction);
}

void Shader::SetUniformLight(const PointLight& light)
{
    this->SetUniform3f("pointLight.ambient", light.ambient);
    this->SetUniform3f("pointLight.diffuse", light.diffuse);
    this->SetUniform3f("pointLight.specular", light.specular);
    this->SetUniform3f("pointLight.position", light.position);
    this->SetUniform1f("pointLight.constant", light.constant);
    this->SetUniform1f("pointLight.linear", light.linear);
    this->SetUniform1f("pointLight.quadratic", light.quadratic);
    this->SetUniform1f("pointLight.intensity", light.intensity);
}

void Shader::SetUniformLight(const std::vector<PointLight>& lights)
{
    for (int i = 0; i < lights.size(); ++i)
    {
        this->SetUniform3f("pointLights[" + std::to_string(i) + "].ambient",  lights[i].ambient);
        this->SetUniform3f("pointLights[" + std::to_string(i) + "].diffuse",  lights[i].diffuse);
        this->SetUniform3f("pointLights[" + std::to_string(i) + "].specular", lights[i].specular);
        this->SetUniform3f("pointLights[" + std::to_string(i) + "].position", lights[i].position);
        this->SetUniform1f("pointLights[" + std::to_string(i) + "].constant", lights[i].constant);
        this->SetUniform1f("pointLights[" + std::to_string(i) + "].linear",   lights[i].linear);
        this->SetUniform1f("pointLights[" + std::to_string(i) + "].quadratic",lights[i].quadratic);
        this->SetUniform1f("pointLights[" + std::to_string(i) + "].intensity", lights[i].intensity);
    }
}

void Shader::SetUniformLight(const SpotLight& light, ShaderStage shaderStage)
{
    std::string stageName = to_string(shaderStage);
    this->SetUniform3f(stageName + "Spotlight.ambient",     light.ambient);
    this->SetUniform3f(stageName + "Spotlight.diffuse",     light.diffuse);
    this->SetUniform3f(stageName + "Spotlight.specular",    light.specular);
    this->SetUniform3f(stageName + "Spotlight.position",    light.position);
    this->SetUniform3f(stageName + "Spotlight.direction",   light.direction);
    this->SetUniform1f(stageName + "Spotlight.cutOff",      light.cutOff);
    this->SetUniform1f(stageName + "Spotlight.outerCutOff", light.outerCutOff);
    this->SetUniform1f(stageName + "Spotlight.constant",    light.constant);
    this->SetUniform1f(stageName + "Spotlight.linear",      light.linear);
    this->SetUniform1f(stageName + "Spotlight.quadratic",   light.quadratic);
    this->SetUniform1f(stageName + "Spotlight.intensity",   light.intensity);
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
