#pragma once

namespace corgi
{
class AbstractComponentPool;
class ComponentPools;
class Scene;

/*!
	* @brief	New Systems must inherits from this class and implement its
	*			virtuals functions
	*			// TODO : Systems could also always take a corgi::ComponentPools thing?
	*/
class AbstractSystem
{
public:

// Friend classes

	friend class Scene;
	friend class ComponentPools;

// Lifecycle

	AbstractSystem(Scene&  scene) : 
		scene_(scene)
	{
			
	}

// Virtual functions

	virtual ~AbstractSystem() = default;

protected :

// Virtual functions

	/*!
		* @brief 	Will be called everytime a new component pool is created by 
		* 			the ECS
		*/
	virtual void on_new_component_pool(AbstractComponentPool* pool){}

	virtual void before_update(){}
	virtual void update(){}
	virtual void after_update(){}

// Member variables 

	Scene& scene_;
};
}