

void main(void)
{
  vec4 color = texture2D(0, gl_textureCoord[0].st);
  gl_FragColor = color;
}