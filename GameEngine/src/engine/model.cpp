#include "model.h"

void Model::DrawOpenGL(Shader& shader, DrawPrimitive drawPrimitive) const
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

void Model::DrawCGL(Shader& shader, DrawPrimitive drawPrimitive, const cgl::mat4& view, const cgl::mat4& projection, bool isCulling, bool isCullingClockWise) const
{
	cgl::mat4 model = cgl::mat4::identity();
	model.translate(cgl::vec4(transform.position, 0.0f));
	// model.rotateX(transform.rotation.x);
	// model.rotateY(transform.rotation.y);
	// model.rotateZ(transform.rotation.z);
	model.scale(transform.scale);

	cgl::mat4 mvp = projection.transpose() * view.transpose() * model;
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		std::vector<cgl::vec4> cglVertices;
		cglVertices.reserve(meshes[i].vertices.size());

		for (unsigned int j = 0; j < meshes[i].vertices.size(); j+=3)
		{
			// Go To Homogeneus Clipping Space
			// Vertex Transforms
			cgl::vec4 v0 = mvp * cgl::vec4(meshes[i].vertices[j+0].Position, 1.0f);
			cgl::vec4 v1 = mvp * cgl::vec4(meshes[i].vertices[j+1].Position, 1.0f);
			cgl::vec4 v2 = mvp * cgl::vec4(meshes[i].vertices[j+2].Position, 1.0f);

			// Go To Normalized Device Coordinates
			// Perspective division
			v0 /= v0.w;
			v1 /= v1.w;
			v2 /= v2.w;

			// Clipping
			if (!v0.is_canonic_cube() || !v1.is_canonic_cube() || !v2.is_canonic_cube())
				continue;

			// Culling
			if (isCulling)
			{
				cgl::vec3 u = (v1 - v0).to_vec3();
				cgl::vec3 v = (v2 - v0).to_vec3();
				float sign = (u.x * v.y) - (v.x * u.y);
				if (isCullingClockWise && sign > 0.0f)
					continue;
				if (!isCullingClockWise && sign < 0.0f)
					continue;
			}

			cglVertices.push_back(v0);
			cglVertices.push_back(v1);
			cglVertices.push_back(v2);
		}

		Mesh::DrawRaw(shader, cglVertices, drawPrimitive);
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
	std::string defaultName = path.substr(path.find_last_of('/') + 1, path.find_last_of('.') - path.find_last_of('/'));
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
