#version 330 core
in vec2 texCoord;

out vec4 FragColor;

uniform float textureScale;
uniform sampler2D myTexture;
void main() {
  FragColor = texture(myTexture, texCoord * textureScale);
}

