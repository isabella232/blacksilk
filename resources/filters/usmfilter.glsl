#version 120

uniform sampler2D MainTexture;
uniform sampler2D Blurred;
uniform float Width;
uniform float Height;
uniform float Strength;
uniform float Threshold;

vec4 sample( sampler2D tex, vec2 position ) {
        return texture2D( tex, vec2( position.x / Width, position.y / Height));
}

void main() {
        vec4  color     = sample( Blurred, gl_FragCoord.xy );
        vec4  current   = sample( MainTexture, gl_FragCoord.xy );
        vec4  diff		= color - current;
        gl_FragColor = current - (diff * Strength);
}
