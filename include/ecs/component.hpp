#ifndef ECS_COMPONENT_HPP
#define ECS_COMPONENT_HPP

#include <utility>

// For storing components. Done like this so that the destructor of a component can be called explicitly
// without knowing its type.

class ComponentBase
{
public :
    virtual ~ComponentBase() = default;
};

template <typename ComponentType>
class ComponentWrapper : public ComponentBase
{
public :
    ComponentType component;

    template<typename... CtorArgTypes>
    ComponentWrapper(CtorArgTypes&&... ctorArgs) :
        component {std::forward<CtorArgTypes>(ctorArgs)...}
    {}
};

#endif