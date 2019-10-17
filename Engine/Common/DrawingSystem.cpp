#include <assert.h>
#include <fstream>

#include "Global.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "TransformComponent.h"
#include "MeshFilterComponent.h"
#include "MeshRendererComponent.h"
#include "FrameGraphComponent.h"

#include "DrawingSystem.h"
#include "D3D11/DrawingDevice_D3D11.h"
#include "D3D12/DrawingDevice_D3D12.h"

using namespace Engine;

DrawingSystem::DrawingSystem() : m_window(nullptr),
    m_bDebug(false),
    m_deviceSize(0),
    m_deviceType(gpGlobal->GetConfiguration<GraphicsConfiguration>().GetDeviceType()),
    m_pDevice(nullptr),
    m_pContext(nullptr),
    m_pEffectPool(nullptr),
    m_pResourceFactory(nullptr),
    m_pResourceTable(nullptr)
{
}

DrawingSystem::~DrawingSystem()
{
}

void DrawingSystem::Initialize()
{
    if (!EstablishConfiguration())
        return;
}

void DrawingSystem::Shutdown()
{
}

void DrawingSystem::Tick(float elapsedTime)
{
    for (auto& pCamera : m_pCameraList)
    {
        auto pFrameGraphComponent = pCamera->GetComponent<FrameGraphComponent>();
        if (pFrameGraphComponent == nullptr)
            continue;

        auto pFrameGraph = pFrameGraphComponent->GetFrameGraph();
        if (pFrameGraph == nullptr)
            continue;

        pFrameGraph->EnqueuePasses();
    }

    m_pDevice->Present(m_pContext->GetSwapChain(), 0);
}

void DrawingSystem::FlushEntity(std::shared_ptr<IEntity> pEntity)
{
    if (pEntity->HasComponent<CameraComponent>() && pEntity->HasComponent<TransformComponent>())
    {
        m_pCameraList.emplace_back(pEntity);
        BuildFrameGraph(pEntity);
    }

    if (pEntity->HasComponent<LightComponent>() && pEntity->HasComponent<TransformComponent>())
        m_pLightList.emplace_back(pEntity);

    if (pEntity->HasComponent<MeshFilterComponent>() && pEntity->HasComponent<TransformComponent>())
    {
        m_pMeshList.emplace_back(pEntity);
        auto pComponent = pEntity->GetComponent<MeshRendererComponent>();
        auto size = pComponent->GetMaterialSize();
        for (uint32_t i = 0; i < size; i++)
        {
            auto pMaterial = pComponent->GetMaterial(i);

            auto pAlbedoMap = pMaterial->GetAlbedoMap();
            if (pAlbedoMap && pAlbedoMap->GetTexture() == nullptr)
            {
                auto uri = pAlbedoMap->GetURI();
                std::shared_ptr<DrawingTexture> pTexture = nullptr;
                m_pDevice->CreateTextureFromFile(uri, pTexture);
                pAlbedoMap->SetTexture(pTexture);
            }

            auto pOcclusionMap = pMaterial->GetOcclusionMap();
            if (pOcclusionMap && pOcclusionMap->GetTexture() == nullptr)
            {
                auto uri = pOcclusionMap->GetURI();
                std::shared_ptr<DrawingTexture> pTexture = nullptr;
                m_pDevice->CreateTextureFromFile(uri, pTexture);
                pOcclusionMap->SetTexture(pTexture);
            }

            auto pMetallicRoughnessMap = pMaterial->GetMetallicRoughnessMap();
            if (pMetallicRoughnessMap && pMetallicRoughnessMap->GetTexture() == nullptr)
            {
                auto uri = pMetallicRoughnessMap->GetURI();
                std::shared_ptr<DrawingTexture> pTexture = nullptr;
                m_pDevice->CreateTextureFromFile(uri, pTexture);
                pMetallicRoughnessMap->SetTexture(pTexture);
            }

            auto pNormalMap = pMaterial->GetNormalMap();
            if (pNormalMap && pNormalMap->GetTexture() == nullptr)
            {
                auto uri = pNormalMap->GetURI();
                std::shared_ptr<DrawingTexture> pTexture = nullptr;
                m_pDevice->CreateTextureFromFile(uri, pTexture);
                pNormalMap->SetTexture(pTexture);
            }

            auto pEmissiveMap = pMaterial->GetEmissiveMap();
            if (pEmissiveMap && pEmissiveMap->GetTexture() == nullptr)
            {
                auto uri = pEmissiveMap->GetURI();
                std::shared_ptr<DrawingTexture> pTexture = nullptr;
                m_pDevice->CreateTextureFromFile(uri, pTexture);
                pEmissiveMap->SetTexture(pTexture);
            }
        }
    }
}

EConfigurationDeviceType DrawingSystem::GetDeviceType() const
{
    return m_deviceType;
}

void DrawingSystem::SetDeviceType(EConfigurationDeviceType type)
{
    m_deviceType = type;
}

void DrawingSystem::FlipDebugState()
{
    m_bDebug = !m_bDebug;
}

bool DrawingSystem::EstablishConfiguration()
{
    if (!PreConfiguration())
        return false;

    if (!CreateDevice())
        return false;

    if (!CreatePreResource())
        return false;

    if (!RegisterRenderer())
        return false;

    if (!PostConfiguration())
        return false;

    return true;
}

bool DrawingSystem::PreConfiguration()
{
    if (gpGlobal == nullptr)
        return false;

    m_deviceSize.x = gpGlobal->GetConfiguration<AppConfiguration>().GetWidth();
    m_deviceSize.y = gpGlobal->GetConfiguration<AppConfiguration>().GetHeight();
    m_window = gpGlobal->GetConfiguration<AppConfiguration>().GetAppHandle();

    return true;
}

bool DrawingSystem::CreateDevice()
{
    switch(m_deviceType)
    {
        case eDevice_D3D11:
            m_pDevice = CreateNativeDevice<eDevice_D3D11>();
            break;
        case eDevice_D3D12:
            m_pDevice = CreateNativeDevice<eDevice_D3D12>();
            break;
        default:
            assert(false);
    }
    m_pDevice->Initialize();
    m_pContext = std::make_shared<DrawingContext>(m_pDevice);
    return true;
}

bool DrawingSystem::CreatePreResource()
{
    auto pSwapChain = CreateSwapChain();
    auto pDepthBuffer = CreateDepthBuffer(); 

    m_pContext->SetSwapChain(pSwapChain);
    m_pContext->SetDepthBuffer(pDepthBuffer);

    m_pEffectPool = std::make_shared<DrawingEffectPool>(m_pDevice);
    m_pResourceFactory = std::make_shared<DrawingResourceFactory>(m_pDevice);
    m_pResourceTable = std::make_shared<DrawingResourceTable>(*m_pResourceFactory);

    m_pResourceFactory->SetEffectPool(m_pEffectPool);

    return true;
}

bool DrawingSystem::RegisterRenderer()
{
    for (uint32_t type = eRenderer_Start; type != eRenderer_End; type++)
    {
        auto& pRenderer = gpGlobal->GetRenderer((ERendererType)type);
        if (pRenderer != nullptr)
        {
            pRenderer->AttachDevice(m_pDevice, m_pContext);
            pRenderer->BuildPass();
            pRenderer->DefineResources(*m_pResourceTable);
        }
    }
    return true;
}

std::shared_ptr<DrawingTarget> DrawingSystem::CreateSwapChain()
{
    assert(m_window != nullptr);

    DrawingTargetDesc desc;
    desc.mHwnd = m_window;
    desc.mWidth = m_deviceSize.x;
    desc.mHeight = m_deviceSize.y;
    desc.mFormat = eFormat_R8G8B8A8_UNORM;

    std::shared_ptr<DrawingTarget> pSwapChain;

    if (!m_pDevice->CreateTarget(desc, pSwapChain))
        return nullptr;

    return pSwapChain;
}

std::shared_ptr<DrawingDepthBuffer> DrawingSystem::CreateDepthBuffer()
{
    DrawingDepthBufferDesc desc;
    desc.mWidth = m_deviceSize.x;
    desc.mHeight = m_deviceSize.y;
    desc.mFormat = eFormat_R24G8_TYPELESS;

    std::shared_ptr<DrawingDepthBuffer> pDepthBuffer;

    if (!m_pDevice->CreateDepthBuffer(desc, pDepthBuffer))
        return nullptr;

    return pDepthBuffer;
}

bool DrawingSystem::PostConfiguration()
{
    m_pContext->UpdateTargets(*m_pResourceTable);

    if(!m_pResourceTable->BuildResources())
        return false;

    m_pDevice->Flush();

    return true;
}

void DrawingSystem::BuildFrameGraph(std::shared_ptr<IEntity> pCamera)
{
    std::shared_ptr<FrameGraph> pFrameGraph = std::make_shared<FrameGraph>();

    FrameGraphComponent frameGraphComponent;
    frameGraphComponent.SetFrameGraph(pFrameGraph);
    pCamera->AttachComponent<FrameGraphComponent>(frameGraphComponent);

    auto pCameraComponent = pCamera->GetComponent<CameraComponent>();

    if (pCameraComponent->GetRendererType() == eRenderer_Forward)
        BuildForwardFrameGraph(pFrameGraph, pCamera);

    else if (pCameraComponent->GetRendererType() == eRenderer_Deferred)
        BuildDeferredFrameGraph(pFrameGraph, pCamera);

    pFrameGraph->InitializePasses();
}

bool DrawingSystem::BuildForwardFrameGraph(std::shared_ptr<FrameGraph> pFrameGraph, std::shared_ptr<IEntity> pCamera)
{
    auto& pRenderer = std::dynamic_pointer_cast<ForwardRenderer>(gpGlobal->GetRenderer(eRenderer_Forward));
    if (pRenderer == nullptr)
        return false;

    pRenderer->CreateDataResources(*m_pResourceTable);

    auto pCameraComponent = pCamera->GetComponent<CameraComponent>();
    auto pTransformComponent = pCamera->GetComponent<TransformComponent>();
    assert(pCameraComponent != nullptr && pTransformComponent != nullptr);

    // Depth pass.
    auto pDepthPass = pRenderer->GetPass(ForwardRenderer::DepthPass());
    assert(pDepthPass != nullptr);
    auto& depthPassNode = pFrameGraph->AddPass(pDepthPass, GraphicsBit);

    depthPassNode.SetClearDepthStencilFunc([&](float& depth, uint8_t& stencil, uint32_t& flag) -> void {
        depth = 1.0f;
        stencil = 0;
        flag = eClear_Depth;
    });

    depthPassNode.SetExecuteFunc([&, pCameraComponent, pTransformComponent, pRenderer, pDepthPass](void) -> void {
        float4x4 view;
        float4x4 proj;
        GetProjectionMatrix(pCameraComponent, proj);
        GetViewMatrix(pTransformComponent, view);

        m_pContext->SetViewport(Box2(float2(0, 0), float2((float)gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), (float)gpGlobal->GetConfiguration<AppConfiguration>().GetHeight())));
        m_pContext->UpdateCamera(*m_pResourceTable, proj, view);
        m_pContext->UpdateContext(*m_pResourceTable);

        RenderQueueItemListType items;
        GetVisableRenderable(items);

        pRenderer->AddRenderables(items);
        pRenderer->Render(*m_pResourceTable, pDepthPass);
    });

    // Shadowmap pass.
    auto pShadowPass = pRenderer->GetPass(ForwardRenderer::ShadowCasterPass());
    assert(pShadowPass != nullptr);
    auto& shadowPassNode = pFrameGraph->AddPass(pShadowPass, GraphicsBit);

    shadowPassNode.SetClearColorFunc(0, [&](float4& color) -> void {
        color.x = 1.0f;
    });

    shadowPassNode.SetClearDepthStencilFunc([&](float& depth, uint8_t& stencil, uint32_t& flag) -> void {
        depth = 1.0f;
        stencil = 0;
        flag = eClear_Depth;
    });

    shadowPassNode.SetExecuteFunc([&, pRenderer, pShadowPass](void) -> void {
        auto pLight = m_pLightList.front();
        if (pLight == nullptr)
            return;

        auto pLightTransformComponent = pLight->GetComponent<TransformComponent>();

        float4x4 lightView;
        float4x4 lightProj;
        GetLightViewProjectionMatrix(pLightTransformComponent, lightView, lightProj);

        m_pContext->SetViewport(Box2(float2(0, 0), float2((float)gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), (float)gpGlobal->GetConfiguration<AppConfiguration>().GetHeight())));
        m_pContext->UpdateCamera(*m_pResourceTable, lightProj, lightView);
        m_pContext->UpdateContext(*m_pResourceTable);

        RenderQueueItemListType items;
        GetVisableRenderable(items);

        pRenderer->UpdateShadowMapAsTarget(*m_pResourceTable);

        pRenderer->AddRenderables(items);
        pRenderer->Render(*m_pResourceTable, pShadowPass);
    });

    // Screen space shadow pass.
    auto pSSSPass = pRenderer->GetPass(ForwardRenderer::ScreenSpaceShadowPass());
    assert(pSSSPass != nullptr);
    auto& sssNode = pFrameGraph->AddPass(pSSSPass, GraphicsBit);

    sssNode.SetClearColorFunc(0, [&](float4& color) -> void {
        color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    });

    sssNode.SetExecuteFunc([&, pCameraComponent, pTransformComponent, pRenderer, pSSSPass](void) -> void {
        auto pLight = m_pLightList.front();
        if (pLight == nullptr)
            return;

        auto pLightTransformComponent = pLight->GetComponent<TransformComponent>();

        float4x4 view;
        float4x4 proj;
        GetProjectionMatrix(pCameraComponent, proj);
        GetViewMatrix(pTransformComponent, view);

        m_pContext->SetViewport(Box2(float2(0, 0), float2((float)gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), (float)gpGlobal->GetConfiguration<AppConfiguration>().GetHeight())));
        m_pContext->UpdateCamera(*m_pResourceTable, proj, view);
        m_pContext->UpdateContext(*m_pResourceTable);

        float4x4 lightView;
        float4x4 lightProj;
        float3 lightDir;
        GetLightViewProjectionMatrix(pLightTransformComponent, lightView, lightProj, lightDir);
        UpdateLightDir(lightDir);
        UpdateLightViewMatrix(lightView);
        UpdateLightProjMatrix(lightProj);

        RenderQueueItemListType items;
        GetVisableRenderable(items);

        pRenderer->UpdateShadowMapAsTexture(*m_pResourceTable);
        pRenderer->UpdateScreenSpaceShadowAsTarget(*m_pResourceTable);

        pRenderer->AddRenderables(items);
        pRenderer->Render(*m_pResourceTable, pSSSPass);
    });

    // Forward shading pass.
    auto pForwardShadingPass = pRenderer->GetPass(ForwardRenderer::ForwardShadingPass());
    assert(pForwardShadingPass != nullptr);
    auto& forwardShadingNode = pFrameGraph->AddPass(pForwardShadingPass, GraphicsBit);

    forwardShadingNode.SetClearColorFunc(0, [&, pCameraComponent](float4& color) -> void {
        color = pCameraComponent->GetBackground();
    });

    forwardShadingNode.SetExecuteFunc([&, pCameraComponent, pTransformComponent, pRenderer, pForwardShadingPass](void) -> void {
        auto pLight = m_pLightList.front();
        if (pLight == nullptr)
            return;

        auto pLightTransformComponent = pLight->GetComponent<TransformComponent>();

        float4x4 view;
        float4x4 proj;
        float3 dir;
        GetProjectionMatrix(pCameraComponent, proj);
        GetViewMatrix(pTransformComponent, view, dir);

        m_pContext->SetViewport(Box2(float2(0, 0), float2((float)gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), (float)gpGlobal->GetConfiguration<AppConfiguration>().GetHeight())));
        m_pContext->UpdateCamera(*m_pResourceTable, proj, view);
        m_pContext->UpdateContext(*m_pResourceTable);

        UpdateCameraDir(dir);

        float4x4 lightView;
        float4x4 lightProj;
        float3 lightDir;
        GetLightViewProjectionMatrix(pLightTransformComponent, lightView, lightProj, lightDir);
        UpdateLightDir(lightDir);

        RenderQueueItemListType items;
        GetVisableRenderable(items);

        pRenderer->UpdateScreenSpaceShadowAsTexture(*m_pResourceTable);

        pRenderer->AddRenderables(items);
        pRenderer->Render(*m_pResourceTable, pForwardShadingPass);
    });

    // SSAO pass.
    auto pSSAOPass = pRenderer->GetPass(ForwardRenderer::SSAOPass());
    assert(pSSAOPass != nullptr);
    auto& ssaoNode = pFrameGraph->AddPass(pSSAOPass, GraphicsBit);

    ssaoNode.SetClearColorFunc(0, [&](float4& color) -> void {
        color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    });

    ssaoNode.SetExecuteFunc([&, pRenderer, pSSAOPass](void) -> void {
        m_pContext->SetViewport(Box2(float2(0, 0), float2((float)gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), (float)gpGlobal->GetConfiguration<AppConfiguration>().GetHeight())));
        m_pContext->UpdateContext(*m_pResourceTable);

        pRenderer->UpdateSSAOTextureAsTarget(*m_pResourceTable);
        pRenderer->RenderRect(*m_pResourceTable, pSSAOPass);
    });

    // Debug layer pass.
    auto pDebugLayerPass = pRenderer->GetPass(ForwardRenderer::DebugLayerPass());
    assert(pDebugLayerPass != nullptr);
    auto& debugLayerNode = pFrameGraph->AddPass(pDebugLayerPass, GraphicsBit);

    debugLayerNode.SetClearColorFunc(0, [&](float4& color) -> void {
        color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    });

    debugLayerNode.SetNeedExecuteFunc([&](void) -> bool {
        return m_bDebug;
    });

    debugLayerNode.SetExecuteFunc([&, pRenderer, pDebugLayerPass](void) -> void {
        m_pContext->SetViewport(Box2(float2(0, 0), float2((float)gpGlobal->GetConfiguration<DebugConfiguration>().GetWidth(), (float)gpGlobal->GetConfiguration<DebugConfiguration>().GetHeight())));
        m_pContext->UpdateContext(*m_pResourceTable);

        pRenderer->UpdateDepthAsTexture(*m_pResourceTable);
        pRenderer->UpdateRectTexture(*m_pResourceTable, ForwardRenderer::ScreenDepthTexture());
        pRenderer->RenderRect(*m_pResourceTable, pDebugLayerPass);
        pRenderer->CopyRect(*m_pResourceTable, ForwardRenderer::DebugLayerTarget(), ForwardRenderer::ScreenTarget(), int2(0, 0));

        pRenderer->UpdateShadowMapAsTexture(*m_pResourceTable);
        pRenderer->UpdateRectTexture(*m_pResourceTable, ForwardRenderer::ShadowMapTexture());
        pRenderer->RenderRect(*m_pResourceTable, pDebugLayerPass);
        pRenderer->CopyRect(*m_pResourceTable, ForwardRenderer::DebugLayerTarget(), ForwardRenderer::ScreenTarget(), int2(gpGlobal->GetConfiguration<DebugConfiguration>().GetWidth() + 5, 0));

        pRenderer->UpdateScreenSpaceShadowAsTexture(*m_pResourceTable);
        pRenderer->UpdateRectTexture(*m_pResourceTable, ForwardRenderer::ScreenSpaceShadowTexture());
        pRenderer->RenderRect(*m_pResourceTable, pDebugLayerPass);
        pRenderer->CopyRect(*m_pResourceTable, ForwardRenderer::DebugLayerTarget(), ForwardRenderer::ScreenTarget(), int2(gpGlobal->GetConfiguration<DebugConfiguration>().GetWidth() + 5, 0) * 2);

        pRenderer->UpdateSSAOTextureAsTexture(*m_pResourceTable);
        pRenderer->UpdateRectTexture(*m_pResourceTable, ForwardRenderer::SSAOTexture());
        pRenderer->RenderRect(*m_pResourceTable, pDebugLayerPass);
        pRenderer->CopyRect(*m_pResourceTable, ForwardRenderer::DebugLayerTarget(), ForwardRenderer::ScreenTarget(), int2(gpGlobal->GetConfiguration<DebugConfiguration>().GetWidth() + 5, 0) * 3);
    });

    pFrameGraph->FetchResources(*m_pResourceTable);

    return true;
}

bool DrawingSystem::BuildDeferredFrameGraph(std::shared_ptr<FrameGraph> pFrameGraph, std::shared_ptr<IEntity> pCamera)
{
    return true;
}

void DrawingSystem::GetVisableRenderable(RenderQueueItemListType& items)
{
    for (auto& pEntity : m_pMeshList)
    {
        auto pTrans = pEntity->GetComponent<TransformComponent>();
        auto pMeshFilter = pEntity->GetComponent<MeshFilterComponent>();
        auto pMeshRenderer = pEntity->GetComponent<MeshRendererComponent>();

        items.push_back(RenderQueueItem{ dynamic_cast<IRenderable*>(pMeshFilter->GetMesh().get()), pTrans});

        auto& pRenderer = std::dynamic_pointer_cast<ForwardRenderer>(gpGlobal->GetRenderer(eRenderer_Forward));

        std::shared_ptr<ITexture> pTexture = nullptr;

        pTexture = pMeshRenderer->GetMaterial(0)->GetAlbedoMap();
        if (pTexture != nullptr)
            pRenderer->UpdateBaseColorTexture(*m_pResourceTable, pTexture->GetTexture());

        pTexture = pMeshRenderer->GetMaterial(0)->GetOcclusionMap();
        if (pTexture != nullptr)
            pRenderer->UpdateOcclusionTexture(*m_pResourceTable, pTexture->GetTexture());

        pTexture = pMeshRenderer->GetMaterial(0)->GetMetallicRoughnessMap();
        if (pTexture != nullptr)
            pRenderer->UpdateMetallicRoughnessTexture(*m_pResourceTable, pTexture->GetTexture());

        pTexture = pMeshRenderer->GetMaterial(0)->GetNormalMap();
        if (pTexture != nullptr)
            pRenderer->UpdateNormalTexture(*m_pResourceTable, pTexture->GetTexture());

        pTexture = pMeshRenderer->GetMaterial(0)->GetEmissiveMap();
        if (pTexture != nullptr)
            pRenderer->UpdateEmissiveTexture(*m_pResourceTable, pTexture->GetTexture());
    }
}

void DrawingSystem::GetViewMatrix(TransformComponent* pTransform, float4x4& view, float3& dir)
{
    float3 pos = pTransform->GetPosition();
    float3 rotate = pTransform->GetRotate();

    float3 up = float3(0.0f, 1.0f, 0.0f);
    dir = float3(1.0f, 0.0f, 0.0f);
    dir = Mat::Mul(dir, Mat::EulerRotateLH(rotate.x, rotate.y, rotate.z));
    auto at = dir + pos;

    view = Mat::LookAtLH(pos, at, up);
}

void DrawingSystem::GetProjectionMatrix(CameraComponent* pCamera, float4x4& proj)
{
    auto fovy = pCamera->GetFov();
    auto zn = pCamera->GetClippingNear();
    auto zf = pCamera->GetClippingFar();
    auto aspect = (float)(gpGlobal->GetConfiguration<AppConfiguration>().GetWidth())
                / (float)(gpGlobal->GetConfiguration<AppConfiguration>().GetHeight());

    proj = Mat::PerspectiveFovLH(fovy, aspect, zn, zf);
}

void DrawingSystem::GetLightViewProjectionMatrix(TransformComponent* pTransform, float4x4& view, float4x4& proj, float3& dir)
{
    float3 rotate = pTransform->GetRotate();

    float3 at = float3(0.0f, 0.0f, 0.0f);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    dir = float3(1.0f, 0.0f, 0.0f);
    dir = Vec::Normalize(Mat::Mul(dir, Mat::EulerRotateLH(rotate.x, rotate.y, rotate.z)));
    float3 pos = at - dir;

    view = Mat::LookAtLH(pos, at, up);
    proj = Mat::OrthoLH(30.f, 30.f, -20.f, 20.f);
}

void DrawingSystem::UpdateCameraDir(float3 dir)
{
    auto pEntry = m_pResourceTable->GetResourceEntry(ForwardRenderer::CameraDirVector());
    assert(pEntry != nullptr);
    auto pCB = std::dynamic_pointer_cast<DrawingConstantBuffer>(pEntry->GetResource());
    if (pCB == nullptr)
        return;
    auto pParam = pCB->GetParameter(strPtr("gCameraDir"));
    if (pParam != nullptr)
        pParam->AsFloat3(dir);
}

void DrawingSystem::UpdateLightDir(float3 dir)
{
    auto pEntry = m_pResourceTable->GetResourceEntry(ForwardRenderer::LightDirVector());
    assert(pEntry != nullptr);
    auto pCB = std::dynamic_pointer_cast<DrawingConstantBuffer>(pEntry->GetResource());
    if (pCB == nullptr)
        return;
    auto pParam = pCB->GetParameter(strPtr("gLightDir"));
    if (pParam != nullptr)
        pParam->AsFloat3(dir);
}

void DrawingSystem::UpdateLightViewMatrix(float4x4 view)
{
    auto pEntry = m_pResourceTable->GetResourceEntry(ForwardRenderer::LightViewMatrix());
    assert(pEntry != nullptr);
    auto pCB = std::dynamic_pointer_cast<DrawingConstantBuffer>(pEntry->GetResource());
    if (pCB == nullptr)
        return;
    auto pParam = pCB->GetParameter(strPtr("gLightViewMatrix"));
    if (pParam != nullptr)
        pParam->AsFloat4x4(view);
}

void DrawingSystem::UpdateLightProjMatrix(float4x4 proj)
{
    auto pEntry = m_pResourceTable->GetResourceEntry(ForwardRenderer::LightProjMatrix());
    assert(pEntry != nullptr);
    auto pCB = std::dynamic_pointer_cast<DrawingConstantBuffer>(pEntry->GetResource());
    if (pCB == nullptr)
        return;
    auto pParam = pCB->GetParameter(strPtr("gLightProjMatrix"));
    if (pParam != nullptr)
        pParam->AsFloat4x4(proj);
}