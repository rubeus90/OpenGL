#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 textures;
layout(location = 3) in vec3 tangents;

uniform mat4 myprojection_matrix;
uniform mat4 myview_matrix;
uniform mat4 mynormal_matrix;
uniform mat4 mymodel_matrix;

out vec4 vertex_to_fragment;
out vec3 normal_to_fragment;
out vec2 texture_to_fragment;
out vec3 tangents_to_fragment;

void main() {
    gl_Position = myprojection_matrix * myview_matrix * mymodel_matrix * vertex_modelspace; 
	vertex_to_fragment = vertex_modelspace;
	normal_to_fragment = normal_modelspace;
	texture_to_fragment = textures;
	tangents_to_fragment = tangents;
}
