#include "ObjLoader.h"

#include <fstream>
#include <sstream>

#include "glm\glm.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "../Loader/MTLLoader.h"

void ObjLoader::load(Mesh * mesh, const std::string & name)
{
	MTLLoader mtlLoader;
	std::vector<std::map<std::string, Material*>> materials;

	mesh->vertices.clear();
	mesh->indices.clear();

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec2> temp_uvs;

	int lineNumber = 0;
	std::ifstream file(GEOMETRY_MAP_PATH+name);
	if (file.is_open())
	{
		std::string line;

		std::string str;
		std::vector<int> indexGroup{-1, -1, -1};
		Mesh::Vertex vertex;
		bool hasNormal = false;
		bool hasUvs = false;
		while (std::getline(file, line))
		{
			lineNumber++;
			std::stringstream ss(line);
			if (line[0] != '#') // If it is not a comment
			{
				std::string c;
				ss >> c;
				if (c.compare("mtllib") == 0)
				{
					ss >> str;
					std::string mtlName(name);
					mtlName = mtlName.substr(0, mtlName.find_last_of("/\\") +1).append(str);
					materials.push_back(mtlLoader.load(mtlName));
				}

				if (c.compare("v") == 0) // Vertex position
				{
					glm::vec3 pos;
					ss >> pos.x >> pos.y >> pos.z;
					temp_vertices.push_back(pos);
				}

				if (c.compare("vt") == 0) // Vertex uvs
				{
					glm::vec2 texCoords;
					ss >> texCoords.x >> texCoords.y;
					temp_uvs.push_back(texCoords);
					hasUvs = true;
				}

				if (c.compare("vn") == 0) // Vertex normal
				{
					glm::vec3 normal;
					ss >> normal.x >> normal.y >> normal.z;
					temp_normals.push_back(normal);
					hasNormal = true;
				}

				if (c.compare("usemtl") == 0) // Use material
				{
					ss >> str;
					std::map<std::string, Material*>::iterator it;
					const int numMtllib = materials.size();
					for (int i = 0; i < numMtllib; i++)
					{
						it = materials[i].find(str);
						if (it != materials[i].end())
						{
							mesh->material = it->second;
						}
					}
				}

				if (c.compare("f") == 0) // Face (triangle)
				{
					int triangle[3];
					for (int i = 0; i < 3; i++)
					{
						ss >> str;

						if(!hasUvs&&hasNormal)
							sscanf_s(str.c_str(), "%d//%d", &indexGroup[0], &indexGroup[2]);
						else if(!hasNormal&&hasUvs)
							sscanf_s(str.c_str(), "%d/%d", &indexGroup[0], &indexGroup[1]);
						else if(!hasNormal&&!hasUvs)
							sscanf_s(str.c_str(), "%d", &indexGroup[0]);
						else
							sscanf_s(str.c_str(), "%d/%d/%d", &indexGroup[0], &indexGroup[1], &indexGroup[2]);
						
						vertex.position[0] = temp_vertices[indexGroup[0] - 1].x;
						vertex.position[1] = temp_vertices[indexGroup[0] - 1].y;
						vertex.position[2] = temp_vertices[indexGroup[0] - 1].z;
						
						if (hasNormal)
						{
							vertex.normal[0] = temp_normals[indexGroup[2] - 1].x;
							vertex.normal[1] = temp_normals[indexGroup[2] - 1].y;
							vertex.normal[2] = temp_normals[indexGroup[2] - 1].z;
						}
						else
						{
							vertex.normal[0] = 0.0f;
							vertex.normal[1] = 0.0f;
							vertex.normal[2] = 0.0f;
						}

						if (hasUvs)
						{
							vertex.uvs[0] = temp_uvs[indexGroup[1] - 1].x;
							vertex.uvs[1] = temp_uvs[indexGroup[1] - 1].y;
						}
						else
						{
							vertex.uvs[0] = 0.0f;
							vertex.uvs[1] = 0.0f;
						}
						
						
						mesh->vertices.push_back(vertex);
						int vertIndex = mesh->vertices.size() - 1;

						mesh->indices.push_back(vertIndex);
						indexGroup[0] = -1;
						indexGroup[1] = -1;
						indexGroup[2] = -1;
						triangle[i] = vertIndex;
					}

					if (!hasNormal)
					{
						glm::vec3 v1 = glm::make_vec3(mesh->vertices[triangle[0]].position);
						glm::vec3 v2 = glm::make_vec3(mesh->vertices[triangle[1]].position);
						glm::vec3 v3 = glm::make_vec3(mesh->vertices[triangle[2]].position);
						glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

						mesh->vertices[triangle[0]].normal[0] = normal[0];
						mesh->vertices[triangle[0]].normal[1] = normal[1];
						mesh->vertices[triangle[0]].normal[2] = normal[2];

						mesh->vertices[triangle[1]].normal[0] = normal[0];
						mesh->vertices[triangle[1]].normal[1] = normal[1];
						mesh->vertices[triangle[1]].normal[2] = normal[2];

						mesh->vertices[triangle[2]].normal[0] = normal[0];
						mesh->vertices[triangle[2]].normal[1] = normal[1];
						mesh->vertices[triangle[2]].normal[2] = normal[2];
					}
				}
			}
		}

		if (materials.empty())
		{
			mesh->material = new Material();
		}

		file.close();
	}
	else
	{
		fprintf(stderr, "Could not open obj file: %s", name.c_str());
	}
}

int ObjLoader::getVertexIndex(const std::vector<Mesh::Vertex>& vertices, const Mesh::Vertex & vertex)
{
	const int length = vertices.size();
	for (int i = 0; i < length; i++)
		if (vertices[i].position[0] == vertex.position[0] && vertices[i].position[1] == vertex.position[1] && vertices[i].position[2] == vertex.position[2] &&
			vertices[i].normal[0] == vertex.normal[0] && vertices[i].normal[1] == vertex.normal[1] && vertices[i].normal[2] == vertex.normal[2] &&
			vertices[i].uvs[0] == vertex.uvs[0] && vertices[i].uvs[1] == vertex.uvs[1])
			return i;
	return -1;
}
