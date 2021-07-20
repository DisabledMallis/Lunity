#ifndef LUNITY_CLIENT_RENDERER_RENDERER_H
#define LUNITY_CLIENT_RENDERER_RENDERER_H

#include "../IPatch.h"

class Renderer : IPatch
{
public:
    Renderer(std::string name) : IPatch(name) { }
};


#endif //LUNITY_CLIENT_RENDERER_RENDERER_H
