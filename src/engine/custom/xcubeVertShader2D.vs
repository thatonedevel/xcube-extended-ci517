#version 150 core

in vec2 vertexPos;

// uniform variables
uniform vec2 cameraPosition;
uniform vec2 meshRotation;
uniform float meshRotation;
uniform vec2 meshScale;
uniform float cameraRotation;

// transformations - 

vec2 rotateVertex(vec2 vertex, vec2 origin, float rotation)
{
    // returns the absolute world-space position of a given vertex. takes rotation in radians
    // formula sourced from Matthew Brett (2016)
    vec2 relOrigin = vertex - origin; // treat mesh origin as 0, 0
    vec2 res = vec2(0, 0);
    res.x = cos(rotation) * relOrigin.x - sin(rotation) * relOrigin.y;
    res.y = sin(rotation) * relOrigin.x + cos(rotation) * relOrigin.y;

    // translate rotated point back to absolute position
    res = res + origin;
    return res;
}

vec2 scaleVertex(vec2 vertex, vec2 origin, vec2 scale)
{
    vec2 relativePos = vertex - origin; // translate to relative
    relativePos = reltivePos * scale;
    relativePos = relativePos + origin;

    return relativePos;
}

vec2 translateVertex(vec2 vertex, vec2 origin, vec2 translation)
{
    // returns the absolute position of a vertex once tranlsated in a given direction
    return vertex + origin + translation;
}

void main()
{
    float absoluteAngle = cameraRotation - meshRotation;
    gl_Position
}