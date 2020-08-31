#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Entity.h>

namespace corgi
{
	void Scene::update(float elapsed_time)
	{
		for(auto& system : systems_)
		{
			system.second->update();
		}
		_elapsed_time = elapsed_time;
	}

	void Scene::before_update(float elapsed_time)
	{
		for(auto& system : systems_)
		{
			system.second->before_update();
		}
	}

	void Scene::after_update(float elapsed_time)
	{
		for(auto& system: systems_)
		{
			system.second->after_update();	
		}
	}
}