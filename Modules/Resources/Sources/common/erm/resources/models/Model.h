#pragma once

#include "erm/math/BoundingBox.h"

#include "erm/resources/data_structs/IAsset.h"

#include <string_view>
#include <vector>

namespace erm {
class Mesh;
}

namespace erm {

class Model : public IAsset
{
public:
	Model(std::string_view path, std::string_view name);

	inline const std::vector<const Mesh*>& GetMeshes() const { return mMeshes; }
	inline const BoundingBox3D& GetLocalBounds() const { return mLocalBounds; }

	void AddMesh(const Mesh& mesh);
	void UpdateLocalBounds();

private:
	std::vector<const Mesh*> mMeshes;
	BoundingBox3D mLocalBounds;
	
};

} // namespace erm
