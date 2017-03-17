#ifndef DOBJECT_H
#define DOBJECT_H

#include "texture.h"

enum DObjectRegPoint
{
	DOBJECTREG_CENTER,
	DOBJECTREG_CENTERTOP,
	DOBJECTREG_CENTERLEFT,
	DOBJECTREG_CENTERRIGHT,
	DOBJECTREG_CENTERBOTTOM,

	DOBJECTREG_TOPLEFT,
	DOBJECTREG_TOPRIGHT,
	DOBJECTREG_BOTTOMLEFT,
	DOBJECTREG_BOTTOMRIGHT
};

struct DObject;
struct Scene;
typedef void(*DObjectBehavior)(struct DObject*, struct Scene*, void*);
typedef void(*DObjectRender)( struct DObject*, SDL_Renderer*, double);

struct DObjectPos
{
	double x, y;
};

struct DObject
{
	SDL_Rect srect;
	DObjectBehavior behavior;
	DObjectRender render;
	struct Texture* texture;
	void* userdata;
	double x, y, changex, changey;
	double rotation;
	int flipx, flipy;
	int w, h;
	enum DObjectRegPoint rpoint;
};

struct DObject* dobject_ctor(
	struct DObject* self,
	struct Texture* texture,
	double x, 
	double y,
	enum DObjectRegPoint rpoint,
	DObjectBehavior behavior, 
	DObjectRender render, 
	void* userdata
);
struct DObjectPos dobject_getrealpos(struct DObject* self);
void dobject_defaultbehavior(
	struct DObject* self, 
	struct Scene* scene, 
	void* userdata
);
void dobject_defaultrender(
	struct DObject* self, 
	SDL_Renderer* renderer,
	double interpolation
);
void dobject_dtor(struct DObject* self);

#endif
