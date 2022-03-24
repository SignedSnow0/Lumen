#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoordinates;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTextureCoordinates;

layout (push_constant) uniform Object
{
    mat4 model;
} object;

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
} ubo;

void main()
{
    gl_Position = ubo.proj * ubo.view * object.model * vec4(inPosition, 1.0);
    outColor = inColor;
    outTextureCoordinates = inTextureCoordinates;
}