#ifndef __PLT_COMPONENT_H__
#define __PLT_COMPONENT_H__

class Component
{
    public:
    const char * name;

    Component() = delete;
    Component(const char * name) : name(name)
    {
        if (name == nullptr) name = "";
    }
};

#endif