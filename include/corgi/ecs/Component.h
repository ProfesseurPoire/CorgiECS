#pragma once

#include <typeindex>

namespace corgi
{
	class Entity;

	/*!
	 * @brief Components are data object attached to an entity and updated by systems
	 */
	class Component
	{
		friend class Entity;

	public:

	// Lifecycle

		Component()=default;
		Component(Entity& entity);
		Component(Entity* entity);
	
		// No virtual destructor because we don't actually really
		// use polymorphism since the components are stored in a pool
		// of their type

	// Functions

		//[[nodiscard]] int id() const;

		// TODO :	this is hacky and basically only exist because I have a mechanism
		//			where systems are made aware of new component pool on the fly
		//			solution : manually add the component pools instead of having
		//			this?
		static const std::type_info& type() 
		{
			return typeid(Component);
		}
	};
}