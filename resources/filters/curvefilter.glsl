#version 120

uniform float     Width;
uniform float     Height;
uniform sampler2D MainTexture;
uniform sampler1D Curve;

vec4 sample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( position.x / Width, position.y / Height));
}

vec4 noiseSample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( position.x * 0.0019, position.y * 0.0019));
}

vec4 curveSample( float value, float orig ) {
    if ( (value - 0.007 <= orig) && (value + 0.007 >= orig) ) {
        return vec4( orig, orig, orig, orig );
    } else {
        return vec4( value, value, value, value );
    }
}


void main() {
    vec2 npos       = vec2( gl_FragCoord.x, gl_FragCoord.y );
    vec4  realColor = sample( MainTexture, npos );
    float val       = realColor.r;

    float  weight   = texture1D( Curve,  min(0.999,max(0.0,val)) ).r;
    gl_FragColor = curveSample( weight, val );
}
