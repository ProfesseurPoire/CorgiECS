#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Layer.h>

#include <corgi/containers/Vector.h>

#include <corgi/logger/log.h>

#include <typeindex>
#include <algorithm>
#include <optional>
#include <functional>

namespace corgi
{
	using ParentEntity = std::optional<std::reference_wrapper<Entity>>;
	using String = std::string;
	
/*!
 * @brief	An entity is an object that is used to modelize a behavior by 
 * 			combining basic building blocks called components. For instance, 
 * 			a character entity would need a Transform component, 
 * 			to know its position in world space, a SpriteRenderer component, 
 * 			to display the character's sprite, an Animator component, to call
 * 			and display 2D animations, a StateMachine component, to changed the 
 * 			state of the character depending on specific inputs and events, and 
 * 			a Character component, that could contain thing like health and mana
 * 			points.
 * 
 * 			Underneath, an entity is basically just an integer identifier that 
 * 			is used by the component pools to identifies which component is 
 * 			attached to which entity through a bimap 
 * 			(EntityId -> ComponentIndex, ComponentIndex -> EntityId)
 *		
 *			Entities can be organized in a tree fashion, so it also keeps 
 *			references to its children and parent objects (if any)
 *
 *			Since component pools are stored inside a scene object, you need to
 * 			give a reference to an existing one when creating an entity so it
 * 			"knows" where to store the components that you attach to it.
 */
class Entity 
{
	friend class Scene;
	friend class Game;
	friend class Renderer;
	friend class Physic;

public:

	static inline int unique_id_ = 0;

// Lifecycle

	// TODO : Maybe I should use Scene to create a unique id
	Entity(Scene& scene, const String& name = "Unnamed") :
		scene_(scene),
		id_(unique_id_){}
	
	/*!
	 * @brief 	Constructs a new entity attached to @ascene that copies the content of @a entity
	 */
	Entity(Scene& scene, Entity& entity):
		scene_(scene),
		id_(unique_id_++)
	{
		// will need to copy @a entity components
	}

	/*!
	 * @brief 	Constructs a new entity attached to @a scene that move the content of @a entity
	 */
	Entity(Scene& scene, Entity&& entity):
		scene_(scene),
		id_(unique_id_++){}

	Entity(Scene& scene):
		id_(unique_id_++),
		scene_(scene){}

	Entity(Entity&& entity) noexcept
	{
		move(std::move(entity));
	}

	Entity(const Entity& entity)
	{
		copy(entity);
	}

	Entity& operator=(const Entity& entity)
	{
		copy(entity);
		return *this;
	}

	Entity& operator=(Entity&& entity) noexcept
	{
		move(std::move(entity));
		return *this;
	}

	~Entity() = default;

// Functions

	/*!
	 * @brief	Returns the entity's unique identifier
	 */
	[[nodiscard]] int id()const noexcept
	{
		return id_;
	}

	[[nodiscard]] Scene& scene()noexcept
	{
		return *scene_;
	}

	/*!
	 * @brief	Returns the entity's name
	 */
	[[nodiscard]] const String& name()const
	{
		return name_;
	}

	void name(const String& n)
	{
		name_ = n;
	}

	[[nodiscard]] Layer current_layer()const
	{
		return Layer(static_cast<int>(layer_));
	}

	void current_layer(Layer layer)
	{
		layer_ = static_cast<int>(layer.value());
	}

	/*!
	 * @brief	Returns true if the entity is currently enabled
	 *			A disabled entity will not update its components and ignore by the systems
	 */
	[[nodiscard]] bool is_enabled()const noexcept
	{
		return enabled_;
	}

	void is_enabled(bool v)
	{
		enabled_ = v;
	}

	template<class T>
	T& add_component(T&& copy)
	{
		if (!scene_->get().pools().contains<T>())
		{
			scene_->get().pools().add<T>();
		}
		return scene_->get().pools().get<T>()->add(id_, std::move(copy));
	}

	template<class T, class ...Args>
	T& add_component(Args&& ... args)
	{
		// TODO : Would there be a way to avoid making this check
		// everytime I add a component? Maybe also making it so we can add
		// multiple component at the same time?
		
		// We first check if a pool that can stores components of type T exist 
		if (!scene_->get().pools().contains<T>())
		{
			scene_->get().pools().add<T>();	// if not, we add a new pool to the scene
		}

		if(scene_->get().pools().get<T>()->has_component(id()))
		{
			log_warning("The entity already has a component of type T associated with it");
			return scene_->get().pools().get<T>()->get(id());
		}

		// Then we simply forward the parameters to the add_param function of our pool
		return scene_->get().pools().get<T>()->add_param(*this, std::forward<Args>(args)...);
	}

	/*
		* @brief	Checks if the entity stores a component of the given
		*			template parameter type
		*
		* @return	Returns true if the component is stored, false otherwise
		*/
	template<class T>
	[[nodiscard]] bool has_component() const{return has_component(typeid(T));}
	[[nodiscard]] bool has_component(const std::type_info& component_type)const
	{
		// We first check if there's a pool that could hold the component 
		if (!scene_->get().pools().contains(component_type))
		{
			return false;
		}
		return scene_->get().pools().get(component_type)->has_component(id_);
	}

	template<class T>
	T& get_component()  // Still const since technically we don't change the entity directly 
	{
		if (!scene_->get().pools().contains<T>())
		{
			//log_error("Boom");
			throw;
		}

		auto& pool = *scene_->get().pools().get<T>();

		if (!pool.has_component(id_))
		{
			//log_error("Boom");
			throw;
		}

		return pool.get(id_);
	}

	template<class T>
	const T& get_component()const
	{
		//check_template_argument<T>();

		if (!scene_->pools().contains<T>())
		{
			// Todo : maybe map the typeid to a string so I can easily 
			// log which component failed?
			//log_error("Could not get a Component Pool");
			throw;
		}

		auto& pool = *scene_->pools().get<T>();

		if (!pool.has_component(id_))
		{
			//log_error("Boom");
			throw;
		}

		return pool.get(id_);
	}

	void remove_component(const std::type_info& component_type)
	{
		if (!scene_->get().pools().contains(component_type))
		{
			return;
		}

		if (!has_component(component_type))
		{
			return;
		}

		scene_->get().pools().get(component_type)->remove(id_);
	}

	template<class T>
	void remove_component()
	{
		//check_template_argument<T>();
		remove_component(typeid(T));
	}

	void enable()
	{
		enabled_=true;
	}

	void disable()
	{
		enabled_=false;
	}


	[[nodiscard]] std::vector<std::string>& tags()noexcept
	{
		return tags_;
	}

	[[nodiscad]] const std::vector<std::string>& tags()const noexcept
	{
		return tags_;
	}

	/*!
		*	@brief	Returns the transform component attached to the entity.
		*			TODO : Probably return a * since this can not work
		*/
	/*const Transform& transform() const;
	Transform& transform();

	const SpriteRenderer& sprite_renderer()const;
	SpriteRenderer& sprite_renderer();*/

	[[nodiscard]] int depth()const
	{
		return depth_;
	}

private:

	template<class T>
	void check_template_argument()
	{
		static_assert(std::is_base_of<T, Component>(),"Template argument must inherit from Component.");
	}

// Variables 

	

	// This is optional in case I want/need to be able
	// to copy or move an entity
	// though if I copy an entity, this reference would probably
	// be problematic, since I wouldn't want to actually copy that
	// from, so, maybe this is wrong?
	// Actually, nonsensical, I can't add component or do
	// anything really if something hasn't already a scene
	// Maybe I could still have a sort of "prefab" scene
	// and then copy stuff from that?
	// I guess I should probably desactivate default
	// constructor then, as I don't want to be able to
	// copy construct an entity?

	std::optional<std::reference_wrapper<Scene>> scene_;	
	Vector<String> tags_;
	String name_;
	char layer_ = 0;
	bool enabled_ = true;
	int id_;
	int depth_ = 0;
	
	void copy(const Entity& e)
	{
		enabled_ = e.enabled_;
		layer_ = e.layer_;
		name_ = e.name_;
	}

	void move(Entity&& e)noexcept
	{
		tags_		= e.tags_;
		name_		= e.name_;
		layer_		= e.layer_;
		enabled_	= e.enabled_;
	}
};
}
