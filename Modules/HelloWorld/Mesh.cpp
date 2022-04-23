#include "Mesh.h"
#include "IResourceTable.h"
#include "CMeshLoader_OBJ.h"

void Mesh::initWithMeshDesc(const MeshDesc& desc, IDevice* device)
{
    
}

void Mesh::draw(IDevice* device, IResourceTable* resourceTable)
{
    
}

void Mesh::expandBoundingBox(IResourceTable* resourceTable, BoundingBox* boxP)
{
    boxP->expand(_boundingBox.min());
    boxP->expand(_boundingBox.max());
}
