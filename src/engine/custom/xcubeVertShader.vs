#version 150 core

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

void main()
{
    float aspectRatio = (frustumRight - frustumLeft) / (frustumBottom - frustumTop);
    float focalLength = 1/tan(fov);
    vec3 projectionVec = vec3(0, 0, 0);
    projectionVec.x = 
}

float zAxisProjection()
{
    float res = 0.0;
    if (worldSpacePosition.z != 0.0)
    {
        float stepOne = (2 * nearPlaneDist * farPlaneDist) / (farPlaneDist - nearPlaneDist);
        float stepTwo = -(1/worldSpacePosition.z);
        float stepThree = (farPlaneDist + nearPlaneDist) / (farPlaneDist - nearPlaneDist);

        res = (stepOne * stepTwo) + stepThree;
    }

    return res;
}

float xAxisProjection()
{
    
}