#ifndef MTL_LOADER_H
#define MTL_LOADER_H

#include "../Geometry/Mesh.h"
#include "../Geometry/Material.h"
#include <string>
#include <map>

class MTLLoader
{
public:
	MTLLoader();

	std::map<std::string, Material*> load(const std::string & name);

private:

};

#endif
