varying float margin;
varying float sigma;

void main()
{
    gl_FragColor = gl_Color;
    float s = gl_TexCoord[0].x;
    float t = gl_TexCoord[0].y;
    float r = sqrt( s*s + t*t );
    float invR = 1.0 - r;
    if( invR < margin )
    {
        float x = 1.0 - invR / margin;
        float exponent = - ( x * x / sigma );\
        float fact = exp( exponent );
        gl_FragColor[3] *= fact;
    }
}