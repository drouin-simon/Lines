uniform bool pressure_width;
uniform bool pressure_opacity;
uniform float base_width;
uniform float pix_per_unit;
uniform float pix_margin;
uniform float pix_damp_width;
uniform float sigma_large;
uniform float sigma_small;

varying float margin;
varying float sigma;

void main()
{
    float pressure = gl_MultiTexCoord0[2];
    float base_width_pressure = base_width;
    if( pressure_width )
        base_width_pressure *= pressure;
    float pix_width = base_width_pressure * pix_per_unit;
    sigma = sigma_large;
    float damp_factor = 1.0;
    margin = pix_margin / pix_width;
   
    if( pix_width < pix_margin ) 
    {
        margin = 1.0;
        if( pix_width < pix_damp_width )
        {
            sigma = sigma_small;
            damp_factor = pix_width / pix_damp_width;
        }
        else
        {
            float ratio = ( pix_width - pix_damp_width ) / ( pix_margin - pix_damp_width );
            sigma = sigma_small + ratio * ( sigma_large - sigma_small );
        }
       
        pix_width = pix_margin;
    }
   
    float width = pix_width / pix_per_unit;
    vec4 scaled_normal = vec4( 0.0, 0.0, 0.0, 0.0 );
    scaled_normal.xyz = gl_Normal;
    scaled_normal *= width;
    vec4 newVertex = gl_Vertex + scaled_normal;
    gl_Position = gl_ModelViewProjectionMatrix * newVertex;
    gl_FrontColor = gl_Color;
    if( pressure_opacity )
        gl_FrontColor[3] *= pressure;
    gl_FrontColor[3] *= damp_factor;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}