#include "dobjecthandler.h"
#include "log.h"

struct DObjectHandler* dobjecthandler_ctor(
	struct DObjectHandler* self,
	SDL_Renderer* renderer,
	struct Scene* scene,
	void* userdata
)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");

	self->dobjects = vec_ctor(struct DObject*, 0);
	self->renderer = renderer;
	self->userdata = userdata;
	self->scene = scene;
	return self;
}

DObjectID dobjecthandler_add(
	struct DObjectHandler* self, 
	struct DObject* dobject
)
{
	log_assert(self, "is NULL");
	log_assert(dobject, "is NULL");

	return 1;
}

void dobjecthandler_update(struct DObjectHandler* self)
{
	log_assert(self, "is NULL");
	for(size_t i = 0; i < vec_getsize(&self->dobjects); i++)
	{
		DObjectBehavior behavior = self->dobjects[i]->behavior;
		if(behavior)
		{
			behavior(self->dobjects[i], self->scene, self->userdata);
		}
	}
}

void dobjecthandler_render(
	struct DObjectHandler* self,
	double interpolation
)
{
	log_assert(self, "is NULL");
	for(size_t i = 0; i < vec_getsize(&self->dobjects); i++)
	{
		DObjectRender render = self->dobjects[i]->render;
		if(render)
		{
			render(
				self->dobjects[i], 
				self->renderer, 
				interpolation
			);
		}
	}
}

void dobjecthandler_dtor(struct DObjectHandler* self)
{
	log_assert(self, "is NULL");
	vec_dtor(&self->dobjects);
}

