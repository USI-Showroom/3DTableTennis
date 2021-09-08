varying vec3 vertex_light_position;
varying vec3 vertex_light_half_vector;
varying vec3 vertex_normal;
uniform sampler2D texture;
uniform int isTexture;

varying vec3 globalNormal;
varying vec3 lightDir;

void main() {
	// Calculate the ambient term
	vec4 ambient_color = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;

	// Calculate the diffuse term
	vec4 diffuse_color = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

	// Calculate the specular value
	float specInt = pow(abs(dot(globalNormal, vertex_light_half_vector)), 4.0*gl_FrontMaterial.shininess);
	vec4 specular_color = gl_FrontMaterial.specular * gl_LightSource[0].specular;
	specular_color = vec4( specular_color.rgb * specInt, specular_color[3]);

	// Set the diffuse value (darkness). This is done with a dot product between the normal and the light
	// and the maths behind it is explained in the maths section of the site.
	float diffuse_value = max(dot(globalNormal, lightDir), 0.0);

	vec3 tmp=diffuse_color.rgb;
	tmp=tmp*diffuse_value;
	diffuse_color=vec4(tmp,diffuse_color[3]);

	vec4 phong =  diffuse_color + ambient_color + specular_color;
	vec3 cf = phong.rgb;
	float af = phong.a;

	vec4 texel = texture2D(texture, vec2(gl_TexCoord[0]));
	vec3 ct = texel.rgb;
	float at = texel.a;

	if(isTexture == 0)
	{
		ct[0]=1.0;
		ct[1]=1.0;
		ct[2]=1.0;
		at=1.0;
	}

	// Set the output color of our current pixel
	gl_FragColor = vec4(ct*cf, at*af);
}
