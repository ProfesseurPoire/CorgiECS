#pragma once

#include <corgi/ecs/System.h>

#include <corgi/containers/TypeMap.h>

#include <concepts>

namespace corgi
{

template<typename T>
concept IsSystem = std::is_base_of<AbstractSystem, T>::value;

class ComponentPools;

class Systems
{
public:

// Lifecycle

    Systems(Scene& scene) :
        scene_(scene)
    {

    }

// Methods

    template<class T, class ... Args>
    void add(Args&& ... params)
    {
        //static_assert(std::is_base_of<AbstractSystem,T>::value);

        // This line here is basically the only
        // reason I'm making this class
        systems_.add<T>(pools_,std::forward<Args>(params)...);
    }

    template<class T>
    void add()
    {
        //static_assert(std::is_base_of<AbstractSystem,T>::value);
        systems_.add<T>(scene_);
    }

    template<class T>
    [[nodiscard]] T& get()
    {
        return *systems_.get<T>();
    }

    void clear()
    {
        systems_.clear();
    }

    template<class T>
    [[nodiscard]] bool contains()
    {
        return false;
    }

    auto begin()
    {
        return systems_.begin();
    }

    auto end()
    {
        return systems_.end();
    }

private:

// Member variables

    Scene& scene_;
    containers::TypeMap<AbstractSystem> systems_;
};
}