#include <glTF2.hpp>

int main()
{
    gltf2::Asset asset = gltf2::load("Test/TriangleWithoutIndices.gltf");
    return 0;
}