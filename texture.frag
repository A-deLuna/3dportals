#version 330 core
in vec2 texCoord;

out vec4 FragColor;

uniform vec2 textureScale;
uniform sampler2D myTexture;
void main() {
  vec2 text = texCoord;
  text.x *= textureScale.x;
  text.y *= textureScale.y;
  vec4 texColor = texture(myTexture, text);
  if(texColor.a < 0.1)
    discard;
  FragColor = texColor;
}

