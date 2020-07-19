#version 120

uniform sampler2D MainTexture;
uniform sampler2D Blurred0;
uniform sampler2D Blurred1;
uniform sampler2D Blurred2;
uniform sampler2D Blurred3;
uniform float     Intensity0;
uniform float     Intensity1;
uniform float     Intensity2;
uniform float     Intensity3;
uniform float     Threshold;
uniform float     Width;
uniform float     Height;

vec4 sample( sampler2D tex, vec2 position ) {
        return texture2D( tex, vec2( position.x / Width, position.y / Height));
}
vec4 pixelUSM( vec4 base, sampler2D texture, vec2 position) {
    vec4 color = sample( texture, position );
    vec4 diff  = color - base;

    return diff;
}
vec4 pixelBlend( vec4 orig, vec4 value, float intensity ) {
    float c = intensity / 100.0;
    return ( value * c ) + ( orig * (1.0 - c));
}
vec4 grainExtract( vec4 first, vec4 second ) {
    return vec4(
        first.r - second.r + 0.5,
        first.g - second.g + 0.5,
        first.b - second.b + 0.5,
        first.a - second.a + 0.5
    );
}
vec4 negate(vec4 a) {
    return vec4(
        1.0 - a.r,
        1.0 - a.g,
        1.0 - a.b,
        1.0 - a.a
    );
}

void main() {
    vec4  basePixel      = sample( MainTexture, gl_FragCoord.xy );

    vec4 blurred0    = sample( Blurred0, gl_FragCoord.xy);
    vec4 blurred1    = sample( Blurred1, gl_FragCoord.xy);
    vec4 blurred2    = sample( Blurred2, gl_FragCoord.xy);
    vec4 blurred3    = sample( Blurred3, gl_FragCoord.xy);

    vec4  raw0       = (blurred0 - basePixel);
    vec4  raw1       = (blurred1 - basePixel);
    vec4  raw2       = (blurred2 - basePixel);
    vec4  raw3       = (blurred3 - basePixel);

    vec4 weighted0   = (raw0       ) * (Intensity0 / 100.0);
    vec4 weighted1   = (raw1 - raw0) * (Intensity1 / 100.0);
    vec4 weighted2   = (raw2 - raw1) * (Intensity2 / 100.0);
    vec4 weighted3   = (raw3 - raw2) * (Intensity3 / 100.0);

    vec4 w1  = grainExtract(blurred0,basePixel);
    vec4 w2  = grainExtract(blurred1,basePixel);
    vec4 w3  = grainExtract(blurred2,basePixel);
    vec4 w4  = grainExtract(blurred3,basePixel);

    float quotient = 1.0 - max( Threshold / 100.0, 0.01 ); //1.0 / ( max( (Threshold  / 100.0),0.05) );
    vec4 max_overall = vec4(
        max( 0.0, min( 1.0, max(w1.r, max(w2.r, max(w3.r, w4.r ) ) ) * quotient ) ),
        max( 0.0, min( 1.0, max(w1.g, max(w2.g, max(w3.g, w4.g ) ) ) * quotient ) ),
        max( 0.0, min( 1.0, max(w1.b, max(w2.b, max(w3.b, w4.b ) ) ) * quotient ) ),
        max( 0.0, min( 1.0, max(w1.a, max(w2.a, max(w3.a, w4.a ) ) ) * quotient ) )
    );
    vec4 temp = weighted0 + weighted1 + weighted2 + weighted3;

    gl_FragColor   = max_overall * (basePixel - temp) + negate(max_overall) * basePixel;

}
