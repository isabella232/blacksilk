#version 120

uniform float Width;
uniform float Height;
uniform float Sigma;
uniform float  Radius;
uniform sampler2D MainTexture;

vec4 sample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( abs(position.x / Width), abs(position.y / Height)));
}

void main(void)
{
    if ( Radius > 0.0 ) {
        vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 );
        float i         = 0.0;
        float size      = 4 * ceil(Radius) + 1;
        float halfSize  = float(int(size * 0.5));
        float div       = Radius * 1.141;
        float sum       = 0.0;

        while ( i < size ) {
            float xM = i - halfSize;
            sum    += exp( (-(xM * xM) / div ) );
            i      += 1.0;
        }
        i = 0.0;

        while( i < size ) {
            float xM = i - halfSize;
            color   += exp( (-(xM * xM) / div ) ) * sample(MainTexture, vec2(gl_FragCoord.x, gl_FragCoord.y + xM)) / sum;
            i += 1.0;
        }
        gl_FragColor = color;
    } else {
        gl_FragColor = sample(MainTexture,gl_FragCoord.xy);
    }
 }
