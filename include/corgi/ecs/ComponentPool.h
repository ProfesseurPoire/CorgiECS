#pragma once

#include <corgi/ecs/Component.h>

#include <corgi/containers/Vector.h>
#include <corgi/containers/Map.h>

#include <typeindex>
#include <algorithm>
#include <stdexcept>

namespace corgi
{

class Component;
	
class AbstractComponentPool
{	
public:

	//friend class ComponentPools;
	
	AbstractComponentPool() = default;
	virtual ~AbstractComponentPool() = default;
	virtual const std::type_info& type() = 0;

	virtual void remove(int entity_id)=0;

	// only here because I need a virtual function
	virtual void useless_crap_of_doom() {}

	virtual int size()const=0;
	virtual void reserve(int size)=0;

	virtual const Component* at(int index) const = 0;
	virtual Component* at(int index) = 0;

	virtual Component* at_id(int entity_id) = 0;
	
	virtual bool has_component(int entity_id)const = 0;

	virtual std::map<int, int> get_map()const=0;
};

template<class T>
class Ref;

template<class T>
class ComponentPool : public  AbstractComponentPool
{
public:

	friend class Entity;
	
// Lifecycle

	ComponentPool(int size=0)
	{
		components_.reserve(size);
	}

// Functions

	/*!
	 *	@brief 	Removes and deletes the component associated with the given @p
	 *			entity_id
	 */
	void remove(int entity_id) override
	{
		if (!has_component(entity_id))
		{
			return;
		}

		// /! Don't forget
		// Always important to use at() instead of [] since [] will create
		// a key if it doesn't exist, which is not what we want here

		components_.erase
		(
			components_.begin() +
			entity_id_to_component_index_.at(entity_id)
		);

		const int index = entity_id_to_component_index_.at(entity_id);

		// Then we need to iterate on every EntityId-Index 
		// Every component that has an index superior to the one we removed
		// must be substracted by one
		for (auto& map_item : entity_id_to_component_index_)
		{
			if (map_item.second > index)
			{
				--map_item.second;
			}
		}

		entity_id_to_component_index_.erase(entity_id);
	}

	/*!
	 * @brief 	Checks if the current pool already owns a component associated
	 * 			with the given @p entity_id
	 * 
	 * @param	entity_id	Unique integer identifying an Entity object
	 * @return 	Returns true if a component associated with @p entity_id exist
	 */
	[[nodiscard]] bool has_component(int entity_id)const override
	{
		return (entity_id_to_component_index_.count(entity_id) != 0);
	}
	
	/*!
	 * @brief Returns the number of component stored by the container
	 */
	int size() const override
	{
		return components_.size();
	}

	void reserve(int new_size) override
	{
		components_.reserve(new_size);
	}

	std::map<int,int> get_map() const override
	{
		return entity_id_to_component_index_;
	}

	/*!
	 * 	@brief 	Access specified element at @p index with bounds checking
	 * 
	 * 			Throws a std::out_of_range exception if @p index 
	 * 			is out of bound
	 * 	@param 	index 	Position of the element to return 
	 *  @return		Returns a pointer to the element
	 */
	const Component* at(int index) const override
	{
		// I'm a bit torn about this, bit I feel like I should
		// give exceptions a chance. I can always remove this at a later
		// stage if I notice some problem with it 

		// TODO : Add a task in my Kaban board about this, to make sure
		// it's not forgotten/that I come back to that later on
		if( index < 0 || index >= size() )
		{
			throw std::out_of_range("Component index is out of bounds");
		}

		// Athough this doesn't check boundaries
		return dynamic_cast<const Component*>(&components_[index]);
	};

	Component* at(int index) override
	{
		// I'm a bit torn about this, bit I feel like I should
		// give exceptions a chance. I can always remove this at a later
		// stage if I notice some problem with it 

		// TODO : Add a task in my Kaban board about this, to make sure
		// it's not forgotten/that I come back to that later on
		if( index < 0 || index >= size() )
		{
			throw std::out_of_range("Component index is out of bounds");
		}

		// Athough this doesn't check boundaries
		return dynamic_cast<Component*>(&components_[index]);
	};
	
	/*! @brief 	Returns the component associated to the given @p entity_id
	 *			If no Component could be found, returns nullptr
	 *
	 *	@return Returns the component associated to the given @p entity_id
	 *			Returns nullptr otherwise
	 */
	Component* at_id(int entity_id) override
	{
		// Actually this explode if it cannot find the component
		// since .at will throw an exception 
		return dynamic_cast<Component*>
		(
			&components_.at(entity_id_to_component_index_.at(entity_id))
		);
	}


	// Oh yeah, so the difference between at_id and get is that at_id is actually
	// used somewhere I don't cast the pool back
	T& get(int entity_id)
	{
		return components_.at(entity_id_to_component_index_.at(entity_id));
	}
	
	/*const Component* pop() const override
	{
		Entity* e = new Entity();
		return new T(e);
	}*/

	T& add(int id, T&& comp)
	{
		components_.emplace_back(std::move(comp));
		entity_id_to_component_index_.emplace(id, components_.size() - 1);
		return components_.back();
	}

	T& add(Entity entity, const T& component)
	{
		components_.emplace_back(component);
		entity_id_to_component_index_.emplace(entity.id(), components_.size() - 1);
		return components_.back();
	}
	
	T& add(Entity& entity)
	{
		components_.emplace_back(entity);
		entity_id_to_component_index_.emplace(entity.id(), components_.size() - 1);
		return components_.back();
	}

	T& add(int entity)
	{
		components_.emplace_back();
		entity_id_to_component_index_.emplace(entity, components_.size() - 1);
		return components_.back();
	}

	// template<class ... Args>
	// T& add_param(int id, Args ... args)
	// {
	// 	components_.emplace_back(std::forward<Args>(args)...);
	// 	entity_id_to_component_index_.emplace(id, components_.size() - 1);
	// 	type_ = typeid(T); // why?  where?
	// 	return components_.back();
	// }
	
	template<class ... Args>
	T& add_param(Entity& entity, Args&& ...args)
	{
		entity_id_to_component_index_.emplace(entity.id(), components_.size());
		component_index_to_entity_id_.emplace(components_.size(), entity.id());
		return components_.emplace_back(std::forward<Args>(args)...);
	}

	/*!
	 * 	@brief 	This kinda sucks, but it returns the Entity object using the entity id
	 */
	Entity* get_entity(int entity_id)
	{
		return 
	}
	
	Ref<T> get_ref(int entity_id)
	{
		return Ref<T>(*this, entity_id);
	}

	/*!
	 * @brief 	Making my own iterator for the component pool
	 * 			that stores the entity_id of the component
	 */
	template<class T>
	class  Iterator
	{
	public:

	// Lifecycle

		Iterator(Map<int, int>& e_to_c, Map<int,int>& c_to_e,
		 typename Vector<T>::iterator it):
			iterator_(it),
			entity_id_to_component_index_(e_to_c),
			component_index_to_entity_id_(c_to_e)
		{

		}

	// Functions
		
		std::pair<int, T&> operator*()
		{
			return {  component_index_to_entity_id_[count],*iterator_};
		}

		Iterator<T> operator++()
		{
			count++;
			iterator_++;
			return Iterator<T>(entity_id_to_component_index_, component_index_to_entity_id_,iterator_);
		}

		bool operator==(const Iterator<T> it)const
		{
			return iterator_ == it.iterator_;
		}

		bool operator!=(const Iterator<T> it)const
		{
			return iterator_ != it.iterator_;
		}

	private:

		Map<int, int>& entity_id_to_component_index_;
		Map<int, int>& component_index_to_entity_id_;
		typename std::vector<T>::iterator iterator_;

		int count=0;
	};

	auto begin()
	{
		return Iterator<T>(entity_id_to_component_index_, component_index_to_entity_id_,components_.begin());
	}

	auto end()
	{
		return Iterator<T>(entity_id_to_component_index_, component_index_to_entity_id_,components_.end());
	}

	auto begin()const
	{
		return Iterator<T>(entity_id_to_component_index_.begin(), components_);
	}

	auto end()const
	{
		return Iterator<T>(entity_id_to_component_index_.end(), components_);
	}
	
	[[nodiscard]]const Map<int,int>&entity_id_to_components()const noexcept
	{
		return entity_id_to_component_index_;
	}

	[[nodiscard]] Map<int,int>&entity_id_to_components() noexcept
	{
		return entity_id_to_component_index_;
	}

	[[nodiscard]] const Vector<T>& components()const noexcept
	{
		return components_;
	}

	[[nodiscard]]  Vector<T>& components() noexcept
	{
		return components_;
	}

	const std::type_info& type()
	{
		return T::type();
	}

	auto& component_index_to_entity_id()
	{
		return component_index_to_entity_id_;
	}

private:

	// Keep track of the component position inside the components_ array
	// relative to the entity_id 
	Map<int, int> entity_id_to_component_index_;
	Map<int, int> component_index_to_entity_id_;

	Vector<T> components_;
};

template<class T>
class Ref
{
public:

	T* operator->()
	{
		return &pool_.get(entity_id_);
	}
	
private:

	friend class ComponentPool<T>;

	Ref(ComponentPool<T>& pool, int entity_id)
		:pool_(pool),entity_id_(entity_id){}

	ComponentPool<T>& pool_;
	int entity_id_;
};

}