varying vec3 vertex_light_position;
varying vec3 vertex_light_half_vector;
varying vec3 vertex_normal;
varying vec3 globalNormal;
varying vec3 lightDir;

void main() {		
	
	// Calculate the normal value for this vertex, in world coordinates (multiply by gl_NormalMatrix)
	vertex_normal = normalize(gl_NormalMatrix * gl_Normal);
	globalNormal = normalize(gl_Normal);

	// Calculate the light position for this vertex
	vertex_light_position =  gl_LightSource[0].position.xzy;
	

	lightDir = -normalize(gl_Vertex.xzy - vertex_light_position);

	// Calculate the light's half vector
	vertex_light_half_vector = normalize(gl_LightSource[0].halfVector.xyz);

	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
}
