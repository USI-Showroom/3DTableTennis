uniform int r,g,b;
uniform sampler2D texture;

void main() {
	vec4 texel = texture2D(texture, vec2(gl_TexCoord[0]));
	gl_FragColor = vec4(r,g,b,texel.a);
}
