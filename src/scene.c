#include "scene.h"
#include "log.h"

struct Scene* scene_ctor(
	struct Scene* self, 
	SceneUpdate update,
	SceneRender render, 
	void* userdata
)
{
	log_assert(self, "is NULL");

	self->update = update;
	self->render = render;
	self->userdata = userdata;
	self->change = SCENECHANGE_NONE;

	return self;
}

