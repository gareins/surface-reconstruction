#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;
uniform bool wireframe;

varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
    // Set fragment color from texture
    gl_FragColor = wireframe ? v_color : texture2D(texture, v_texcoord);
}
