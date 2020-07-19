#version 120

uniform float Width;
uniform float Height;

uniform float  Strength;
uniform float  Sigma;
uniform sampler2D MainTexture;
uniform sampler2D NoiseMap;
uniform sampler1D Weights;

vec4 sample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( position.x / Width, position.y / Height));
}

vec4 noiseSample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( position.x / Width, position.y / Height));
}


vec4 overlay( vec4 I, vec4 M ) {
    I.rgb *= 255.0;
    M.rgb *= 255.0;
    vec4 E = (I * 0.0039) * (I + (2.0 * M * 0.0039) * (255.0 - I));
    E.rgb *= 0.0039;
    return E;
}

/* http://docs.gimp.org/en/gimp-concepts-layer-modes.html#id597868 */
vec4 grainMerge( vec4 I, float m ) {
    I.rgb *= 255.0;
    vec4 E = (1-m)*I + m*I/ (255.0 / 2);
    E.rgb /= 255.0;
    return E;
}


void main() {
    vec2 npos       = gl_FragCoord.xy;
    vec4  realColor = vec4(0.0,0.0,0.0,0.0);
    realColor       = sample( MainTexture, npos );
    float  value    = sample( NoiseMap, npos ).x;
    float  weight   = texture1D( Weights,  realColor.x ).r;
    float  w        = value;
    vec4  wh        = overlay( realColor, vec4(w,w,w,w) );
    gl_FragColor    = (realColor * (1.0 - weight)) + (wh * weight);
}
