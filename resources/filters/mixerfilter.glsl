#version 120

uniform sampler2D MainTexture;
uniform vec3 BrightWeights;
uniform vec3 DarkWeights;
uniform float Weight;
uniform float Width;
uniform float Height;

vec4 texelFetch( sampler2D tex, float x, float y ) {
        return texture2D( tex, vec2( float(x) / Width, float(y) / Height ));
}

void main() {
    vec4 color = texelFetch( MainTexture, gl_FragCoord.x, gl_FragCoord.y );

    float luma = Weight + ((color.r * 0.299 ) + (color.g * 0.587 ) + (color.b * 0.114 ));

    float bright = ((color.r * BrightWeights.r ) + (color.g * BrightWeights.g ) + (color.b * BrightWeights.b ));
    float dark = ((color.r * DarkWeights.r ) + (color.g * DarkWeights.g ) + (color.b * DarkWeights.b ));

    float combined = ( dark * ( 1.0 - luma ) ) + ( bright * luma );

    gl_FragColor = vec4(combined,combined,combined,1.0);
}
