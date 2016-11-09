uniform sampler2DRect texture;
varying mediump vec4 texc;
void main(void)
{
	gl_FragColor = texture2DRect(texture, texc.st);
}

