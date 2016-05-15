#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform bool wireframe;
uniform vec4 wire_color;

attribute vec4 a_position;
attribute vec2 a_texcoord;

varying vec4 v_color;
varying vec2 v_texcoord;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;

    v_color = wireframe ? wire_color : vec4(1,0,0,1);
    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
}
