uniform sampler2D textureID;

void main(void)
{
  vec4 color = texture2D(textureID, gl_TexCoord[0].st);
  gl_FragColor = color;
}