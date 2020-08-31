#include <corgi/ecs/Entities.h>
#include <corgi/ecs/Entity.h>

using namespace corgi;

Entities::Entities(Scene& scene):
	scene_(scene)
    tree_(scene)
{}

Node<Entity>& Entities::emplace(const String& name)noexcept
{
    return tree_.children().emplace_back(scene_, name);
}

Node<Entity>& Entities::emplace(Entity&& entity)noexcept
{
    return tree_.children().emplace_back(std::move(entity));
}

Node<Entity>& Entities::emplace(const Entity& entity)noexcept
{
    return tree_.children().emplace_back(entity);
}

Optional<std::reference_wrapper<Node<Entity>>> Entities::find(const String& name) 
{
    auto it = std::find_if
    (
        tree_.begin(),
        tree_.end(),
        [&](const Node<Entity>& p) { return p.get().name() == name; }
    );

    if (!(it != tree_.end()))
    {
        return std::nullopt;
    }

    return *it;
}

Optional<std::reference_wrapper<const Node<Entity>>> Entities::find(const String& name)const 
{
    auto it = std::find_if
    (
        tree_.begin(),
        tree_.end(),
        [&](const Node<Entity>& p) { return p.get().name() == name; }
    );

    if (!(it != tree_.end()))
    {
        return std::nullopt;
    }
	
    return *it;
}

Optional<std::reference_wrapper<const Entity>> Entities::operator[](int id) const noexcept
{
    if(entities_.contains(id))
    {
        return entities_.at(id);
    }
    return std::nullopt;
}

Optional<std::reference_wrapper<Entity>> Entities::operator[](int id) noexcept
{
    if(entities_.contains(id))
    {
        return entities_[id];
    }
    return std::nullopt;
}