#include "screen_space_shadow.vs"
#include "screen_space_shadow.ps"

technique11 ScreenSpaceShadow
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, ScreenSpaceShadow_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ScreenSpaceShadow_VS()));
    }
}