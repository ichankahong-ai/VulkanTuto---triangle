#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "Application.h"

int main()
{
    
    Application app;

    app.Init();
    app.Run();
    app.Destroy();

    return 0;
}