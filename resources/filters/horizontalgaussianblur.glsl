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
    float weight[3];

    weight[0] = 0.2270270270;
    weight[1] = 0.3162162162;
    weight[2] = 0.0702702703;

    float offset[3];

    offset[0] = 0.0;
    offset[1] = 1.3846153846;
    offset[2] = 3.2307692308;


    if ( Radius > 0.0 ) {
        vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 );

        color += weight[0] * texture2D(MainTexture, vec2(gl_FragCoord.x / Width, gl_FragCoord.y / Height) );
        for ( int i = 1; 3 > i; i++ ) {
            vec4 valueRight = weight[i] * texture2D(MainTexture, vec2(gl_FragCoord.x / Width,(gl_FragCoord.y + offset[i] ) / Height ) );
            vec4 valueLeft = weight[i] * texture2D(MainTexture, vec2(gl_FragCoord.x / Width, (gl_FragCoord.y - offset[i] ) / Height ) );

            color += valueRight;
            color += valueLeft;
        }

        gl_FragColor = color;
    } else {
        gl_FragColor = sample(MainTexture,gl_FragCoord.xy);
    }
 }
