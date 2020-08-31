#pragma once

#include <corgi/ecs/Systems.h>
#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

#include <corgi/containers/TypeMap.h>
#include <corgi/containers/Map.h>

#include <corgi/memory/UniquePtr.h>

#include <typeindex>

namespace corgi
{
	// TODO : Would have been nice to just use a TypeMap here but it has some repercussions in the code for now
class ComponentPools
{
public:

	ComponentPools(Systems& sys) : 
		systems_(sys)
	{

	}

	/*!
		* @brief	Returns a non owning pointer to the component pool of type T.
		*			Returns nullptr if no pool of type T is stored inside the container
		*/
	template<class T>
	[[nodiscard]] ComponentPool<T>* get()
	{
		if (!contains<T>())
		{
			return nullptr;
		}
		return dynamic_cast<ComponentPool<T>*>(pools_.at(typeid(T)).get());
	}

	[[nodiscard]] AbstractComponentPool* get(const std::type_info& component_type)
	{
		if (!contains(component_type))
		{
			return nullptr;
		}
		return pools_.at(component_type).get();
	}

	/*!
		* @brief	Will try to add a new component pool of the specified type.
		*			If a component pool of the same type already exists, it won't
		*			construct it 
		*/
	template<class T>
	void add(int size=0)
	{
		// If there's already a component pool of type T, we skip this part
		if(contains<T>())	
		{
			return;
		}
		
		auto it = pools_.emplace(typeid(T), new ComponentPool<T>(size));

		// Register the new component_pool to systems
		// Not sure it's really needed though
		for (auto& system : systems_)	// This might be a slight pas 
		{
			// just assuming we have an object that inherits from AbstractComponentPool here
			static_cast<AbstractSystem*>(system.second.get())->on_new_component_pool(it.first->second.get());
		}
	}

	template<class T>
	void remove(){remove(typeid(T));}

	template<class T>
	[[nodiscard]] bool contains() const { return contains(typeid(T)); }

	[[nodiscard]] bool contains(const std::type_info& component_type)const
	{
		return (pools_.count(component_type) != 0);
	}
	
	void remove(const std::type_info& component_type)
	{
		pools_.erase(component_type);
	}

	auto begin()
	{
		return pools_.begin();
	}

	auto end()
	{
		return pools_.end();
	}

	/*!
		* @brief	Returns the number of elements in the container
		*/
	[[nodiscard]] auto size() const noexcept{return pools_.size();}

	/*!
		* @brief	Erases all ComponentPool from the container
		* 
		* 			After this call, size() will return 0 
		*/
	void clear() noexcept{pools_.clear();}

private:

	Map<std::type_index, UniquePtr<AbstractComponentPool>> pools_;
	Systems& systems_;
};
}