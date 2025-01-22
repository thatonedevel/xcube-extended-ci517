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

vec2 rotateVertex(vec2 vertex, float rotation)
{
    // returns the relative position of a given vertex once rotated by the given value. takes rotation in radians
    // (Brett, 2016)
    vec2 res = vec2(0, 0);
    res.x = cos(rotation) * vertex.x - sin(rotation) * vertex.y;
    res.y = sin(rotation) * vertex.x + cos(rotation) * vertex.y;

    return res;
}

vec2 scaleVertex(vec2 vertex, vec2 scale)
{
    vec2 relativePos = vec2(0, 0);
    relativePos.x = vertex.x * scale.x;
    relativePos.y = vertex.y * scale.y;
    return relativePos;
}

vec2 projectCoordinate(vec2 coord, vec2 dim, float multiplier)
{
    // takes an absolute vertex position in metres and produces the device coordinate
    // screen origin = 0,0
    // coord x < dim x / 2 --> negative
    // coord y 
    vec2 pixelPos = coord * multiplier;

    vec2 projected = vec2(0, 0);

    vec2 origin = dim / 2;

    projected.x = pixelPos.x / origin.x;
    projected.y = pixelPos.y / origin.y;

    return projected;
}

void main()
{
    float absoluteAngle =  meshRotation - cameraRotation;
    vec2 absPos = meshPosition - cameraPosition;
    // carry out translations - rotate, scale, translate
    vec2 rotatedRelPos = rotateVertex(vertexPos, absoluteAngle);
    vec2 unprojectedPos = scaleVertex(rotatedRelPos, meshScale);
    vec2 projectedPos = projectCoordinate(unprojectedPos + absPos, windowDimensions, metresToPixelsScale);

    gl_Position = vec4(projectedPos, 0, 1);
}