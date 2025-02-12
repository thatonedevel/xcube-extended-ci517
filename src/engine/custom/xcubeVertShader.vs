#version 150 core

// vertex attributes
in vec3 worldSpacePosition;

// frustum planes
uniform float frustumLeft;
uniform float frustumRight;
uniform float frustumTop;
uniform float frustumBottom;
uniform float nearPlaneDist;
uniform float farPlaneDist;

uniform int renderAs2D;

// other camera info
uniform float cameraFOV;
uniform vec3 cameraPos;

// calculations follow the projection formula as described by eric lengyel (2012)

float zAxisProjection(float camZ)
{
    float res = 0.0;
    if (worldSpacePosition.z != 0.0)
    {
        float stepOne = (2 * nearPlaneDist * farPlaneDist) / (farPlaneDist - nearPlaneDist);
        float stepTwo = -(1/camZ);
        float stepThree = (farPlaneDist + nearPlaneDist) / (farPlaneDist - nearPlaneDist);

        res = (stepOne * stepTwo) + stepThree;
    }

    return res;
}

void main()
{
    float aspectRatio = (frustumRight - frustumLeft) / (frustumBottom - frustumTop);
    float focalLength = 1/tan(cameraFOV);

    vec3 camSpaceVertexPos = worldSpacePosition - cameraPos;

    vec4 projectionVec = vec4(0, 0, 0, 1);
    projectionVec.x = -(focalLength/camSpaceVertexPos.z) * camSpaceVertexPos.x;
    projectionVec.y = -(focalLength/camSpaceVertexPos.z) * camSpaceVertexPos.y;
    if (renderAs2D == 1)
    {
        projectionVec.z = 1;
    }
    else
    {
        projectionVec.z = zAxisProjection(camSpaceVertexPos.z);
    }
    

    gl_Position = projectionVec;
}
