#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "../Geometry/Mesh.h"
#include "../Constants.h"

class ObjLoader
{
public:
	void load(Mesh* mesh, const std::string& name);

private:
	int getVertexIndex(const std::vector<Mesh::Vertex>& vertices, const Mesh::Vertex& vertex);
};

#endif
