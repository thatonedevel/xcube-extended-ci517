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

// other camera info
uniform float cameraFOV;
uniform vec3 cameraPos;

void main()
{
    float aspectRatio = (frustumRight - frustumLeft) / (frustumBottom - frustumTop);
    float focalLength = 1/tan(fov);

    vec3 camSpaceVertexPos = worldSpacePosition - cameraPos;

    vec4 projectionVec = vec4(0, 0, 0, 1);
    projectionVec.x = -(focalLength/camSpaceVertexPos.z) * camSpaceVertexPos.x;
    projectionVec.y = -(focalLength/camSpaceVertexPos.z) * camSpaceVertexPos.y;
    projectionVec.z = zAxisProjection(camSpaceVertexPos.z);

    gl_Position = projectionVec;
}

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