#include "forward_base.vs"
#include "forward_base.ps"

technique11 ForwardBase
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, ForwardBase_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ForwardBase_PS()));
    }
}