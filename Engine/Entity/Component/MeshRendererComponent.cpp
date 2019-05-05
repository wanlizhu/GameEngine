#include "MeshRendererComponent.h"

using namespace Engine;

std::shared_ptr<IRenderer> MeshRendererComponent::GetRenderer()
{
    return m_pRenderer;
}

void MeshRendererComponent::SetRenderer(std::shared_ptr<IRenderer> pRenderer)
{
    m_pRenderer = pRenderer;
}