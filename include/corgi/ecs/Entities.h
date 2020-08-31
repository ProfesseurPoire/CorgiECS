#pragma once

#include <string>
#include <memory>
#include <map>
#include <optional>
#include <vector>

#include <corgi/containers/Tree.h>
#include <corgi/ecs/Entity.h>

template <class T>
using Optional = std::optional<T>;

namespace corgi
{
    class Scene;
    class Entity;

    template<>
    class Tree<Entity>
    {
        public:

            Tree(Scene& scene):scene_(scene){}
            Scene& scene_;
    };

    template<>
    template<>
    Node<Entity>& Children<Entity>::emplace_back()
    {
        if (parent_)
        {
            return *children_.emplace_back(std::make_unique<Node<Entity>>(parent_, parent_->get().scene() ));
        }
        return *children_.emplace_back(std::make_unique<Node<Entity>>(tree_, tree_.scene_));
    }
	

template <class T>
using Reference = std::reference_wrapper<T>;

template<class T>
using UniquePtr = std::unique_ptr<T>;

template<class T, class U>
using Map = std::map<T,U>;

template<class T>
using Vector = std::vector<T>;

using String = std::string;

/*!
 * @brief   Stores the entities. Entities are tied to a scene
 * 
 */
class Entities
{
    friend class Scene;

public:

// Functions

	/*!
	 * @brief   Constructs a new entity in place at root level
	 */
	Node<Entity>& emplace(const String& name)noexcept;

    /*!
     * @brief  Copy constructs a new entity in place at root level by using @a entity content
     */
    Node<Entity>& emplace(const Entity& entity) noexcept;

    /*!
     * @brief   Move constructs a new entity and move the content of @a entity inside it
     */
    Node<Entity>& emplace(Entity&& entity) noexcept;

    /*!
	 * @brief	Tries to find an entity called "name" inside the scene
	 *			Returns a pointer to the entity if founded, returns nullptr  
	 * 			otherwise
	 */

	// TODO : check if it actually do the desired behavior of not allowing me to
	// change what's inside the reference_wrapper
	[[nodiscard]] Optional<std::reference_wrapper<Node<Entity>>> find(const String& name) ;
	[[nodiscard]] Optional<std::reference_wrapper<const Node<Entity>>> find(const String& name)const ;

    /*!
     * @brief   Tries to find a reference to the entity with an id equals to @a id
    *           If no e
     */
    Optional<std::reference_wrapper<Entity>> operator[](int id) noexcept;

    /*!
     * @brief   Tries to find a reference to an entity with an id equals to @a id
     */    
    Optional<std::reference_wrapper<const Entity>> operator[](int id) const noexcept;
	
	/*Entity* find_by_name(std::string name);
	Entity* find_by_tag(std::string tag);*/

protected:

// Lifecycle

    /*!
     * @brief   The entity manager is tightly coupled to the scene,
     *          and should only be contained inside a scene object
     */
	Entities(Scene& scene);

private:

// Member variables

	Scene& scene_;

    // Entities at the root level
    //Vector<UniquePtr<Entity>> root_;

    Tree<Entity> tree_;

    Map<int, Reference<Entity>> entities_;
};
}