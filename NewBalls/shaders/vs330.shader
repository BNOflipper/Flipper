#version 330 core
layout (location = 0) in vec2 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
 
out vec3 ourColor; // output a color to the fragment shader
//uniform vec2 position;

void main()
{
    gl_Position = vec4(aPos, 1.0f, 1.0f);
	ourColor = aColor;// - vec3(position, 0.0f); // set ourColor to the input color we got from the vertex data
}