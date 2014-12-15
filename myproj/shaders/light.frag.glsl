#version 330 core

uniform int myrenderStyle;
uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;

uniform vec4 mylight1_position;
uniform vec4 mylight2_position;
uniform vec4 mylight3_position;
uniform vec4 mylight4_position;
uniform vec4 mylight_color;
uniform vec3 mylight_direction;
uniform int mylight_type;
uniform mat4 mymodel_matrix;

uniform sampler2D tex;
uniform sampler2D bump;
uniform samplerCube cubeMap; 
uniform sampler2D mirror;

in vec4 vertex_to_fragment;
in vec3 normal_to_fragment;
in vec2 texture_to_fragment;
in vec3 tangent_to_fragment;

void main (void)
{   
	vec3 n = normalize(mynormal_matrix * normal_to_fragment);
	vec3 t = normalize (mynormal_matrix * tangent_to_fragment);
	vec3 b = normalize (cross(n,t));
	mat3 in_m = mat3(t,b,n);
	mat3 out_m = transpose(in_m);

	// Normal
	vec3 normal_without_bump = normalize(mynormal_matrix * normal_to_fragment);
	vec3 normal = normal_without_bump + normalize (2.0 * texture2D(bump, texture_to_fragment.st).rgb - 1.f);

	//Object position
	vec4 mypos_ = myview_matrix * mymodel_matrix * vertex_to_fragment;
	vec3 mypos = ( mypos_.xyz / mypos_.w );

	//Eye position (origin)
	vec3 eyepos = vec3(0,0,0);
	eyepos = out_m * eyepos;

	//Light position
	vec4 lightpos_ = myview_matrix * mylight1_position;	
	vec3 lightpos1 = lightpos_.xyz/lightpos_.w;
	lightpos_ = myview_matrix * mylight2_position;	
	vec3 lightpos2 = lightpos_.xyz/lightpos_.w;
	lightpos_ = myview_matrix * mylight3_position;	
	vec3 lightpos3 = lightpos_.xyz/lightpos_.w;
	lightpos_ = myview_matrix * mylight4_position;	
	vec3 lightpos4 = lightpos_.xyz/lightpos_.w;
	
	//Kd
	vec4 kd = vec4(1,0,0,0); //without texture
	
	//Cube mapping
	vec3 reflection = normalize(reflect(eyepos-mypos, normal_without_bump));
	reflection = vec3 (inverse(myview_matrix) * vec4 (reflection, 0.0));
	kd = textureCube(cubeMap, reflection); 

	//Texture
	kd += texture2D(tex, texture_to_fragment.st); 

	//Reflection
	kd += texture2D(mirror, texture_to_fragment.st); 

	//Ks
	vec4 ks = vec4(1,1,1,0);

	vec3 reflected_ray;

	//Lumiere ambiante
	gl_FragColor = kd * 0.3;


	/******  Light 1  *******/
 	//Diffuse
	vec4 color = mylight_color * pow(max(dot(normalize(mylight_direction), normalize(mypos - mylight1_position.xyz/mylight1_position.w)),0),1);
	gl_FragColor += color * kd * max(  dot(normal, normalize(lightpos1 - mypos)), 0.0);
	//Specular
	reflected_ray = normalize(reflect(mypos-lightpos1, normal));
	gl_FragColor += color * ks * pow(max(dot(reflected_ray, normalize(eyepos-mypos)),0),60);

	
	/******  Light 2  *******/
 	//Diffuse
	gl_FragColor += mylight_color * kd * max(  dot(normal, normalize(lightpos2 - mypos)), 0.0);
	//Specular
	reflected_ray = normalize(reflect(mypos-lightpos2, normal));
	gl_FragColor += mylight_color * ks * pow(max(dot(reflected_ray, normalize(eyepos-mypos)),0.0),20);

	
	/******  Light 3  *******/
 	//Diffuse
	gl_FragColor += mylight_color * kd * max(  dot(normal, normalize(lightpos3 - mypos)), 0.0);
	//Specular
	reflected_ray = normalize(reflect(mypos-lightpos3, normal));
	gl_FragColor += mylight_color * ks * pow(max(dot(reflected_ray, normalize(eyepos-mypos)),0.0),20);

	
	/******  Light 4  *******/
 	//Diffuse
	gl_FragColor += mylight_color * kd * max(  dot(normal, normalize(lightpos4 - mypos)), 0.0);
	//Specular
	reflected_ray = normalize(reflect(mypos-lightpos4, normal));
	gl_FragColor += mylight_color * ks * pow(max(dot(reflected_ray, normalize(eyepos-mypos)),0.0),20);
}