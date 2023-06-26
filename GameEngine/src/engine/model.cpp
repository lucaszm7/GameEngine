#include "model.h"

void Model::Draw(Shader& shader) const
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
		meshes[i].Draw(shader);
	}
}

void Model::OnImGui() const
{
	if (ImGui::TreeNode(std::string("Transform " + name).c_str()))
	{
		ImGui::DragFloat3("Position:", (float*)&transform.position[0], 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat3("Rotation:", (float*)&transform.rotation[0], 0.1f, -glm::pi<float>(), glm::pi<float>());
		ImGui::DragFloat3("Scale:",    (float*)&transform.scale[0], 0.01f, -10.0f, 10.0f);
		ImGui::TreePop();
	}
}

void Model::LoadCustomModel(const std::string& path, TriangleOrientation triOrientation)
{
	std::ifstream stream(path);
	if (!stream)
		throw new std::exception(std::string("Could not open file: " + path).c_str());

	std::stringstream ss;
	std::string line;

	unsigned int nTriangles;
	unsigned int nMaterials;

	float r, g, b;

	float shine;
	std::string sHasText;
	bool hasTexture;

	std::getline(stream, line);
	ss << line;
	ss >> line >> line >> line >> name;

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
	std::vector<Texture> textures;

	vertices.reserve(nTriangles * 3);
	indices.reserve(nTriangles * 3);

	unsigned int counter = 0;

	for(int triCounter = 0; triCounter < nTriangles; ++triCounter)
	{
		Vertex v0;
		Vertex v1;
		Vertex v2;

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

	Texture tex("resources/textures/mandrill_256.jpg", Texture::Type::SPECULAR, Texture::Parameter::LINEAR);
	textures.push_back(tex);

	meshes.emplace_back(vertices, indices, textures);
}

void Model::LoadClassicModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	name = path.substr(0, path.find_last_of('/'));
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
	std::vector<Texture> textures;

	// Vertex
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
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
	std::vector<Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, Texture::Type::DIFFUSE);
	std::vector<Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, Texture::Type::SPECULAR);
	std::vector<Texture> emissionMaps = loadMaterialTexture(material, aiTextureType_EMISSION_COLOR, Texture::Type::EMISSION);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); ++j)
		{
			std::string texPath = textures_loaded[j].GetPath();
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
			Texture texture(name + "/" + std::string(str.C_Str()), textureType, Texture::Parameter::REPEAT);
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}