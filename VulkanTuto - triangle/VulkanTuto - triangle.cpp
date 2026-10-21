#include <iostream>

#include "Application.h"

int main()
{
    
    Application app;

    app.Init();
    app.Run();
    app.Destroy();

    return 0;
}

//cool color

//static float2 positions[3] = float2[](
//    float2(0.0, -0.5),
//    float2(0.5, 0.5),
//    float2(-0.5, 0.5)
//    );
//
//static float3 colors[3] = float3[](
//    float3(0.157, 1.0, 0.522),
//    float3(0.408, 0.03, 0.03),
//    float3(0.408, 0.03, 0.03)
//    );