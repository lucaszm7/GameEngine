#include "model.h"

static glm::vec3 _random_normalized_color()
{
	return { rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0) };
}

void Model::Draw(Shader& shader, PRIMITIVE drawPrimitive) const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, transform.scale);
	shader.SetUniformMatrix4fv("model", model);

	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Draw(shader, drawPrimitive);
	}
}

cgl::mat4 Model::GetModelMatrix() const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, transform.scale);
	return model;
}

void Model::OnImGui() const
{
	if (ImGui::TreeNode(std::string("Transform " + name).c_str()))
	{
		ImGui::DragFloat3("Position:", (float*)&transform.position[0], 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Rotation:", (float*)&transform.rotation[0], 0.1f, -2*glm::pi<float>(), 2*glm::pi<float>());
		ImGui::DragFloat3("Scale:",    (float*)&transform.scale[0], 0.01f, -100.0f, 100.0f);
		ImGui::TreePop();
	}
}

void Model::AddTexture(const std::string& path)
{
	std::shared_ptr<Texture> tex = std::make_shared<Texture>(path, Texture::Type::DIFFUSE);
	meshes.front().textures.push_back(tex);
	textures_loaded.push_back(tex);
}

void Model::LoadCustomModel(TriangleOrientation triOrientation)
{
	std::ifstream stream(m_Path);
	if (!stream)
		throw new std::exception(std::string("Could not open file: " + m_Path).c_str());

	std::stringstream ss;
	std::string line;

	unsigned int nTriangles;
	unsigned int nMaterials;

	float r, g, b;

	float shine;
	std::string sHasText;
	bool hasTexture;

	std::string defaultName;

	std::getline(stream, line);
	ss << line;
	ss >> line >> line >> line >> defaultName;

	int id = 0;
	name = defaultName;
	while (m_NamesMap.contains(name))
	{
		id = std::rand() % (int)(2e10);
		name = defaultName + '_' + std::to_string(id);
	}
	m_NamesMap[name] = id;

	std::getline(stream, line);
	ss.str(std::string());
	ss.clear();
	ss << line;
	ss >> line >> line >> line >> nTriangles;

	std::getline(stream, line);
	ss.str(std::string());
	ss.clear();
	ss << line;
	ss >> line >> line >> line >> nMaterials;

	std::getline(stream, line);
	ss.str(std::string());
	ss.clear();
	ss << line;
	ss >> line >> line >> r >> g >> b;
	glm::vec4 ambientColor(r, g, b, 1.0f);

	std::getline(stream, line);
	ss.str(std::string());
	ss.clear();
	ss << line;
	ss >> line >> line >> r >> g >> b;
	glm::vec4 diffuseColor(r, g, b, 1.0f);

	std::getline(stream, line);
	ss.str(std::string());
	ss.clear();
	ss << line;
	ss >> line >> line >> r >> g >> b;
	glm::vec4 specularColor(r, g, b, 1.0f);

	std::getline(stream, line);
	ss.str(std::string());
	ss.clear();
	ss << line;
	ss >> line >> line >> shine;

	std::getline(stream, line);
	ss.str(std::string());
	ss.clear();
	ss << line;
	ss >> line >> line >> sHasText;

	hasTexture = sHasText == "YES";

	std::getline(stream, line);
	ss.str(std::string());
	ss.clear();

	int colorIndex;
	float x, y, z;
	float i, j, k;
	float u = 0, v = 0;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>> textures;

	vertices.reserve(nTriangles * 3);
	indices.reserve(nTriangles * 3);

	unsigned int counter = 0;

	for(int triCounter = 0; triCounter < nTriangles; ++triCounter)
	{
		Vertex v0;
		Vertex v1;
		Vertex v2;

		v0.Color = {1, 0, 0};
		v1.Color = {0, 1, 0};
		v2.Color = {0, 0, 1};

		std::getline(stream, line);
		ss.str(std::string());
		ss.clear();
		ss << line;
		if (hasTexture)
		{
			ss >> line >> x >> y >> z >> i >> j >> k >> colorIndex >> u >> v;
		}
		else
		{
			ss >> line >> x >> y >> z >> i >> j >> k >> colorIndex;
			u = 0.0f;
			v = 0.0f;
		}
		v0.Position = { x, y, z };
		v0.Normal = { i, j, k };
		v0.TexCoord = { u, v };

		std::getline(stream, line);
		ss.str(std::string());
		ss.clear();
		ss << line;
		if (hasTexture)
		{
			ss >> line >> x >> y >> z >> i >> j >> k >> colorIndex >> u >> v;
		}
		else
		{
			ss >> line >> x >> y >> z >> i >> j >> k >> colorIndex;
			u = 1.0f;
			v = 0.0f;
		}
		v1.Position = { x, y, z };
		v1.Normal = { i, j, k };
		v1.TexCoord = { u, v };

		std::getline(stream, line);
		ss.str(std::string());
		ss.clear();
		ss << line;
		if (hasTexture)
		{
			ss >> line >> x >> y >> z >> i >> j >> k >> colorIndex >> u >> v;
		}
		else
		{
			ss >> line >> x >> y >> z >> i >> j >> k >> colorIndex;
			u = 0.0f;
			v = 1.0f;
		}
		v2.Position = { x, y, z };
		v2.Normal = { i, j, k };
		v2.TexCoord = { u, v };

		std::getline(stream, line);
		ss.str(std::string());
		ss.clear();

		if (triOrientation == TriangleOrientation::CounterClockWise)
		{
			vertices.push_back(v0);
			vertices.push_back(v1);
			vertices.push_back(v2);
		}
		else
		{
			vertices.push_back(v0);
			vertices.push_back(v2);
			vertices.push_back(v1);
		}

		indices.push_back(counter++);
		indices.push_back(counter++);
		indices.push_back(counter++);
	}

	std::shared_ptr<Texture> tex;
	tex = std::make_shared<Texture>("resources/textures/mandrill_256.jpg", Texture::Type::DIFFUSE, Texture::Wrap::MIRROR, Texture::Filtering::NEAREST_NEIGHBOR, true);
	textures.push_back(tex);

	meshes.emplace_back(vertices, indices, textures);
}

void Model::LoadClassicModel()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(m_Path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	std::string defaultName = m_Path.substr(m_Path.find_last_of('/') + 1, m_Path.find_last_of('.') - m_Path.find_last_of('/') - 1);
	int id = 0;
	name = defaultName;
	while (m_NamesMap.contains(name))
	{
		id = std::rand() % (int)(2e10);
		name = defaultName + '_' + std::to_string(id);
	}

	m_NamesMap[name] = id;
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>> textures;


	unsigned int triCounter = 0;
	glm::vec3 triColor = { 1.0f,1.0f,1.0f };
	// Vertex
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		if (triCounter == 0)
			triColor = _random_normalized_color();
		triCounter++;
		if (triCounter > 3)
			triCounter = 0;

		vertex.Color = triColor;

		vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

		if (mesh->HasNormals())
			vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

		if (mesh->mTextureCoords[0]) // Has texture coords
			vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

		else
			vertex.TexCoord = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Textures
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, Texture::Type::DIFFUSE);
	std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, Texture::Type::SPECULAR);
	std::vector<std::shared_ptr<Texture>> emissionMaps = loadMaterialTexture(material, aiTextureType_EMISSION_COLOR, Texture::Type::EMISSION);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());

	return Mesh(vertices, indices, textures);
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType)
{
	std::vector<std::shared_ptr<Texture>> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); ++j)
		{
			std::string texPath = textures_loaded[j]->GetPath();
			std::string fileName = texPath.substr(texPath.find_last_of('/') + 1, texPath.size());
			if (std::strcmp(fileName.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			std::shared_ptr<Texture> texture;
			texture = std::make_shared<Texture>(m_Path.substr(0, m_Path.find_last_of('/')) + "/" + std::string(str.C_Str()), textureType, Texture::Wrap::REPEAT, Texture::Filtering::TRILLINEAR);
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}
