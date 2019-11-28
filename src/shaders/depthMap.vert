#version 430

in vec3 position;
in vec3 normal;

uniform mat4 vp;

void main()
{
    //normal offset shadows
    //http://urho3d.prophpbb.com/topic1991.html
    gl_Position = vp * vec4(position - normal, 1.0f);
}
