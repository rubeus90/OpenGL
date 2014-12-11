#version 330 core

uniform int myrenderStyle;
uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;

uniform vec4 mylight_position;
uniform vec4 mylight_color;
uniform vec3 mylight_direction;
uniform int mylight_type;
uniform mat4 mymodel_matrix;
uniform sampler2D tex;
uniform sampler2D bump;
uniform samplerCube cubeMap; 

in vec4 vertex_to_fragment;
in vec3 normal_to_fragment;
in vec2 texture_to_fragment;
in vec3 tangent_to_fragment;

void main (void)
{   
	//if (myrenderStyle == 0) gl_FragColor = vec4(1,0,0,0);
	//if (myrenderStyle == 1) gl_FragColor = vec4(0,0,1,0);

	vec3 n = normalize(mynormal_matrix * normal_to_fragment);
	vec3 t = normalize (mynormal_matrix * tangent_to_fragment);
	vec3 b = normalize (cross(n,t));
	mat3 in_m = mat3(t,b,n);
	mat3 out_m = transpose(in_m);

	// Normal
	//vec3 normal = normalize(mynormal_matrix * normal_to_fragment);
	vec3 normal = normalize (2.0 * texture2D(bump, texture_to_fragment.st).rgb - 1.f);

	//Object position
	vec4 mypos_ = myview_matrix * mymodel_matrix * vertex_to_fragment;
	vec3 mypos = ( mypos_.xyz / mypos_.w );

	//Eye position (origin)
	vec3 eyepos = vec3(0,0,0);
	eyepos = out_m * eyepos;

	//Light position
	//vec4 lightpos_ = vec4(2,0,0,1);
	vec4 lightpos_ = myview_matrix * mylight_position;	
	vec3 lightpos = lightpos_.xyz/lightpos_.w;
	lightpos = out_m * lightpos;
	
	//Kd
	vec4 kd = vec4(1,0,0,0); //without texture
	kd= texture2D(tex, texture_to_fragment.st); //with texture
	//Cube mapping
	vec3 reflection = normalize(reflect(normalize(eyepos-mypos), normalize(normal)));
	reflection = vec3 (inverse (myview_matrix) * vec4 (reflection, 0.0));
	kd = textureCube(cubeMap, reflection); 

	vec4 ks = vec4(1,1,1,0);

	vec3 mypos_tolightpos = normalize(lightpos - mypos);
	mypos_tolightpos = normalize(out_m*mypos_tolightpos);

	vec3 mypos_toeyepos = normalize(eyepos-mypos);
	mypos_toeyepos = normalize(out_m * mypos_toeyepos);

	vec3 reflected_ray;

	//Lumiere ambiante
	gl_FragColor += kd * 0.3;

	//Silhouette
//	if(dot(normal, normalize(eyepos-mypos)) < 0.2  && myrenderStyle == 0){
	//	gl_FragColor = vec4(0,1,0,0);
	//}
	{
		switch(mylight_type){
			case 0:  // Point light
				//Diffuse
				gl_FragColor += mylight_color * kd * max(  dot(normal, normalize(lightpos - mypos)), 0.0);
				//Specular
				reflected_ray = normalize(reflect(mypos-lightpos, normal));
				gl_FragColor += mylight_color * ks * pow(max(dot(reflected_ray, normalize(eyepos-mypos)),0.0),20);				
				break;

			case 1: // Directional light
				//Diffuse
				gl_FragColor += mylight_color * kd * max(  dot(normal, normalize(-mynormal_matrix*mylight_direction)), 0.0);
				//Specular
				reflected_ray = normalize(reflect(mynormal_matrix*mylight_direction, normal));
				gl_FragColor += mylight_color * ks * pow(max(dot(reflected_ray, normalize(eyepos-mypos)),0),60);
				break;

			case 2: // Spotlight
				//Diffuse
				vec4 color = mylight_color * pow(max(dot(normalize(mylight_direction), normalize(mypos - mylight_position.xyz/mylight_position.w)),0),1);
				gl_FragColor += color * kd * max(  dot(normal, normalize(lightpos - mypos)), 0.0);
				//Specular
				reflected_ray = normalize(reflect(mypos-lightpos, normal));
				gl_FragColor += color * ks * pow(max(dot(reflected_ray, normalize(eyepos-mypos)),0),60);				
				break;
		}
	}

	//Texture
	//if(myrenderStyle == 1) gl_FragColor = texture2D(tex, texture_to_fragment.st);

	if(myrenderStyle == 4) gl_FragColor = vec4(1,1,1,0);
}