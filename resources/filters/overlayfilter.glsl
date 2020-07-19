#version 120

uniform float Width;
uniform float Height;
uniform sampler2D MainTexture;
uniform sampler2D OverlayTexture;

vec4 sample( sampler2D tex, vec2 position ) {
    return texture2D( tex, vec2( position.x / Width, position.y / Height));
}

vec4 overlay( vec4 I, float m ) {
    I.rgb *= 255.0;
    vec4 E = (1-m)*I + m*( I * I * 0.0039 );
    E.rgb *= 0.0039;
    return E;
}

void main() {
   gl_FragColor =  overlay( sample( MainTexture, gl_FragCoord.xy ), sample(OverlayTexture, gl_FragCoord.xy).r );
}

