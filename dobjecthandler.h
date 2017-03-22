#ifndef DOBJECTHANDLER_H
#define DOBJECTHANDLER_H

#include "dobject.h"
#include "vec.h"

typedef size_t DObjectID;
struct DObjectHandler
{
	SDL_Renderer* renderer;
	Vec(struct DObject*) dobjects;
	struct Scene* scene;
	void* userdata;
};

struct DObjectHandler* dobjecthandler_ctor(
	struct DObjectHandler* self,
	SDL_Renderer* renderer,
	struct Scene* scene,
	void* userdata
);
DObjectID dobjecthandler_add(
	struct DObjectHandler* self, 
	struct DObject* dobject
);
void dobjecthandler_remove(
	struct DObjectHandler* self, 
	struct DObject* dobject
);
void dobjecthandler_update(struct DObjectHandler* self);
void dobjecthandler_render(
	struct DObjectHandler* self, 
	double interpolation
);
void dobjecthandler_dtor(struct DObjectHandler* self);

#endif
