#include "basic.vs"
#include "basic.ps"

technique11 BasicPrimitive
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, BasicPrimitive_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, BasicPrimitive_PS()));
    }
}