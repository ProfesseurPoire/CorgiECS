#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/Systems.h>

#include <corgi/ecs/ComponentPools.h>
#include <corgi/ecs/Entities.h>

namespace corgi
{
class Entity;

/*!
 * @brief 	The goal of the scene object is to glue the different ECS elements
 * 			together. Thus, it stores entities, components and systems that
 * 			interact with each other
 */
class Scene
{
public:

// Functions

	/*!
	 * @brief 	Returns a reference to the System's container
	 */
	[[nodiscard]] Systems& systems()noexcept{ return systems_; }

	/*!
	 * @brief 	Returns a const reference to the System's container
	 */
	[[nodiscard]] const Systems& systems()const noexcept{return systems_;}

	/*!
	 * @brief 	Returns a reference to the Entity's container
	 */
	[[nodiscard]] Entities& entities()noexcept{return entities_;}

	/*!
	 * @brief 	Returns a const reference to the Entity's container
	 */
	[[nodiscard]] const Entities& entities()const noexcept{return entities_;}

	/*!
	 * @brief 	Returns a reference to the ComponentPool's containers
	 */
	[[nodiscard]] ComponentPools& pools()noexcept{return pools_;}

	/*!
	 * @brief 	Returns a const reference to the ComponentPool's containers
	 */
	[[nodiscard]]const ComponentPools& pools()const noexcept{return pools_;}

	[[nodiscard]] float elapsed_time()const noexcept
	{
		return _elapsed_time;
	}

	//void remove_entity(Entity& entity);

	void before_update(float elapsed_time);
	void update(float elapsed_time);
	void after_update(float elapsed_time);

	// /*!
	//  * @brief Use this to create a new entitiy
	//  */
	// Entity& new_entity(const String& name = "NewEntity");

	// /*!
	//  * @brief	Creates a copy of the given entity and attach it to the scene
	//  */
	// Entity& new_entity(const Entity& entity);

	// /*!
	//  * @brief	Appends a new Entity to the scene and returns a new reference to it
	//  */
	// Entity& append(const String& name = "NewEntity");

	// Entity& append(Entity* entity);
	// Entity& append(Entity&& entity)noexcept;
	// Entity& append(const Entity& entity);

	// /*!
	//  * @brief	Removes and delete every entity attached to the scene and
	//  * 			their components
	//  * 			Also delete the systems and component pools
	//  */
	// void clear();

	/*!
	 * @brief 	Returns the canvas's container
	 * 
	 * @return 	Returns a reference to the std::vector<std::unique_ptr<Canvas>>
	 */
	//std::vector<std::unique_ptr<Canvas>>& canvas();

	float _time_step;
	float _elapsed_time = 0.0f;
	
	// Associate an entityId with an EntityPointer
	//Map<int, Entity*> entities_;

private:

	//std::vector<std::unique_ptr<Canvas>> canvas_;
	Systems systems_{*this};
	Entities entities_ {*this};
	ComponentPools pools_{systems_};
};
}
