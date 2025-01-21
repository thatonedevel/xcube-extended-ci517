#version 150 core

in vec2 vertexPos;

// uniform variables - camera
uniform float cameraRotation;
uniform vec2 cameraPosition;
uniform vec2 windowDimensions;

// mesh info
uniform vec2 meshPosition;
uniform float meshRotation;
uniform vec2 meshScale;
uniform float metresToPixelsScale;


// transformations - 

vec2 rotateVertex(vec2 vertex, vec2 origin, float rotation)
{
    // returns the relative position of a given vertex once rotated by the given value. takes rotation in radians
    // (Brett, 2016)
    vec2 relOrigin = vertex - origin; // treat mesh origin as 0, 0
    vec2 res = vec2(0, 0);
    res.x = cos(rotation) * relOrigin.x - sin(rotation) * relOrigin.y;
    res.y = sin(rotation) * relOrigin.x + cos(rotation) * relOrigin.y;

    return res;
}

vec2 scaleVertex(vec2 vertex, vec2 origin, vec2 scale)
{
    vec2 relativePos = vertex - origin; // translate to relative
    relativePos = relativePos * scale;
    return relativePos;
}

vec2 projectCoordinate(vec2 coord, vec2 dim, float multiplier)
{
    // screen origin = 0,0
    // coord x < dim x / 2 --> negative
    // coord y 
    vec2 projected = vec2(0, 0);
    vec2 origin = dim / 2;

    projected.x = (coord.x * multiplier - origin.x) / origin.x;
    projected.y = (coord.y * multiplier - origin.y) / origin.y;

    return projected;
}

void main()
{
    float absoluteAngle =  meshRotation - cameraRotation;
    vec2 absPos = meshPosition - cameraPosition;
    vec2 absVertPos = vertexPos - cameraPosition;
    // carry out translations - rotate, scale, translate
    vec2 rotatedRelPos = rotateVertex(absVertPos, absPos, absoluteAngle);
    vec2 unprojectedPos = scaleVertex(rotatedRelPos + absPos, absPos, meshScale);
    vec2 projectedPos = projectCoordinate(unprojectedPos, windowDimensions, metresToPixelsScale);

    gl_Position = vec4(projectedPos, 1, 1);
}