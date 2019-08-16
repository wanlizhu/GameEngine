#include "shadow_caster.vs"
#include "shadow_caster.ps"

technique11 ShadowCaster
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, ShadowCaster_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ShadowCaster_PS()));
    }
}