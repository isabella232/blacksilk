#version 120

uniform  float Width;
uniform  float Height;
uniform  float Weight;

uniform  vec3 FirstColor;
uniform  vec3 SecondColor;

uniform sampler2D MainTexture;

vec4 sample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( position.x / Width, position.y / Height));
}

vec4 overlay( vec4 I, vec4 M ) {
    I.rgb *= 255.0;
    M.rgb *= 255.0;
    vec4 E = (I * 0.0039) * (I + (M * 2.0 * 0.0039) * (255.0 - I));
    E.rgb *= 0.0039;
    return E;
}

void main() {
    vec4 finalColor			= vec4(0.0,0.0,0.0,0.0);

    vec4 currentColor		= sample( MainTexture, vec2(gl_FragCoord.x,gl_FragCoord.y));

    float intensity           = currentColor.x;

    float intensityHighlights = intensity * Weight;
    float intensityRest       = 1.0 - intensityHighlights;
    float intensityShadows    = intensityRest*intensityRest;
    float intensityOriginal   = 1.0 - intensityHighlights - intensityShadows;

    /* top layer */
    finalColor  = overlay( currentColor, vec4(FirstColor,1.0)) * intensityHighlights;
    /* medium layer*/
    finalColor  += overlay( currentColor, vec4(SecondColor,1.0)) * intensityShadows;
    /* bottom layer*/
    finalColor  += vec4( intensity, intensity, intensity, 1.0 ) * intensityOriginal;

    gl_FragColor = finalColor;
}
