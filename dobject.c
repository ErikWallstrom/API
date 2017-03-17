#include "dobject.h"
#include "log.h"

struct DObject* dobject_ctor(
	struct DObject* self,
	struct Texture* texture, 
	double x, 
	double y,
	enum DObjectRegPoint rpoint,
	DObjectBehavior behavior,
	DObjectRender render,
	void* userdata
)
{
	log_assert(self, "is NULL");
	log_assert(texture, "is NULL");
	log_assert(
		rpoint <= DOBJECTREG_BOTTOMRIGHT, 
		"invalid rpoint (%i)", 
		rpoint
	);

	self->texture = texture;
	self->rpoint = rpoint;
	self->x = x;
	self->y = y;
	self->flipx = 0;
	self->flipy = 0;
	self->changex = 0;
	self->changey = 0;
	self->rotation = 0.0;
	self->w = texture->width;
	self->h = texture->height;
	self->behavior = behavior;
	self->render = render;
	self->userdata = userdata;
	self->srect = (SDL_Rect){0, 0, 0, 0};
	
	return self;
}

struct DObjectPos dobject_getrealpos(struct DObject* self)
{
	log_assert(self, "is NULL");
	struct DObjectPos pos;
	switch(self->rpoint)
	{
	case DOBJECTREG_CENTER:
		pos.x = self->x - self->w / 2.0;
		pos.y = self->y - self->h / 2.0;
		break;
	case DOBJECTREG_CENTERTOP:
		pos.x = self->x - self->w / 2.0;
		pos.y = self->y;
		break;
	case DOBJECTREG_CENTERLEFT:
		pos.x = self->x;
		pos.y = self->y - self->h / 2.0;
		break;
	case DOBJECTREG_CENTERRIGHT:
		pos.x = self->x - self->w;
		pos.y = self->y - self->h / 2.0;
		break;
	case DOBJECTREG_CENTERBOTTOM:
		pos.x = self->x - self->w / 2.0;
		pos.y = self->y - self->h;
		break;
	case DOBJECTREG_TOPLEFT:
		pos.x = self->x;
		pos.y = self->y;
		break;
	case DOBJECTREG_TOPRIGHT:
		pos.x = self->x - self->w;
		pos.y = self->y;
		break;
	case DOBJECTREG_BOTTOMLEFT:
		pos.x = self->x;
		pos.y = self->y - self->h;
		break;
	case DOBJECTREG_BOTTOMRIGHT:
		pos.x = self->x - self->w;
		pos.y = self->y - self->h;
		break;
	}

	return pos;
}

void dobject_defaultbehavior(
	struct DObject* self, 
	struct Scene* scene, 
	void* userdata
)
{
	log_assert(self, "is NULL");
	log_assert(scene, "is NULL");
	(void)userdata;

	self->x += self->changex;
	self->y += self->changey;

	self->changex = 0;
	self->changey = 0;
}

void dobject_defaultrender(
	struct DObject* self, 
	SDL_Renderer* renderer,
	double interpolation
)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");

	SDL_Rect* srect;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if(self->srect.x || self->srect.y || self->srect.w || self->srect.h)
		srect = &self->srect;
	else
		srect = NULL;

	if(self->flipx)
		flip |= SDL_FLIP_HORIZONTAL;
	if(self->flipy)
		flip |= SDL_FLIP_VERTICAL;

	SDL_RenderCopyEx(
		renderer,
		self->texture->raw,
		srect,
		&(SDL_Rect){
			dobject_getrealpos(self).x + 
				self->changex * interpolation,
			dobject_getrealpos(self).y +
				self->changey * interpolation,
			self->w,
			self->h
		},
		self->rotation,
		NULL,
		flip
	);
}

void dobject_dtor(struct DObject* self)
{
	log_assert(self, "is NULL");
}

