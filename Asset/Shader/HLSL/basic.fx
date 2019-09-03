#include "basic.vs"
#include "basic.ps"

technique11 Basic
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, Basic_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, Basic_PS()));
    }
}