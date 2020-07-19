#version 120

uniform float Width;
uniform float Height;
uniform float MaxWidth;
uniform float MaxHeight;

uniform vec3   VignetteCenter;
uniform float  Strength;
uniform float  Radius;

vec4 sample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( position.x / Width, position.y / Height));
}

vec4 noiseSample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( position.x * 0.01, position.y * 0.01));
}

/* http://docs.gimp.org/en/gimp-concepts-layer-modes.html#id606488 */
vec4 overlay( vec4 I, float m ) {
    I.rgb *= 255.0;
    vec4 E = (1-m)*I + m*( I * I * 0.0039 );
    E.rgb *= 0.0039;
    return E;
}


void main() {
    float vignetteValue = 0.0;
    /* calculate distance from vignette center */
    vec2 currentPosition	= vec2( gl_FragCoord.x, gl_FragCoord.y );
    vec2 centerPosition		= (VignetteCenter.xy);
    centerPosition.x		= (Width * 0.01 * centerPosition.x);
    centerPosition.y		= (Height * 0.01 * centerPosition.y);
    float currentDistance   = abs( distance(centerPosition, currentPosition ));

    /* distance to max blackness */
    float maxDistance   = Radius * 0.01 * ((Width + Height) * 0.5);

    /* merging with black layer and alpha vignette */
    vignetteValue = Strength * 0.01 * currentDistance / maxDistance;


    gl_FragColor = vec4(vignetteValue,vignetteValue,vignetteValue,0.0);
}
