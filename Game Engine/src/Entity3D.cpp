#include "Entity3D.hpp"

namespace Pringine{

    Entity3D::Entity3D(EntityType type, Graphics3D* graphics):graphics(graphics),Entity(type)
    {
        graphics->model = &transform.get_transformation();
    }

    Entity3D::~Entity3D()
    {
        
    }



}