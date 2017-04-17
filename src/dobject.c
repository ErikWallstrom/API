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

int intersects_(double amin, double amax, double bmin, double bmax)
{
	if(bmin > amin)
		amin = bmin;
	if(bmax < amax)
		amax = bmax;
	if(amax <= amin)
		return 0;

	return 1;
}

int dobject_intersects(struct DObject* self, struct DObject* dobject)
{
	log_assert(self, "is NULL");
	log_assert(dobject, "is NULL");

	struct DObjectPos selfpos = dobject_getrealpos(self);
	struct DObjectPos dobjpos = dobject_getrealpos(dobject);

	int intersectx = intersects_(
		selfpos.x, 
		selfpos.x + self->w, 
		dobjpos.x, 
		dobjpos.x + dobject->w
	);

	if(!intersectx)
		return 0;

	int intersecty = intersects_(
		selfpos.y, 
		selfpos.y + self->h, 
		dobjpos.y, 
		dobjpos.y + dobject->h
	);

	if(!intersecty)
		return 0;

	return 1;
}

int dobject_intersectspoint(struct DObject* self, double x, double y)
{
	log_assert(self, "is NULL");
	struct DObjectPos pos = dobject_getrealpos(self);
	if((x >= pos.x) && (x <= (pos.x + self->w - 1)) && 
		(y >= pos.y) && (y <= (pos.y + self->h - 1)))
	{
		return 1;
	}

	return 0;
}

int dobject_hittestleft(struct DObject* self, struct DObject* dobject)
{
	log_assert(self, "is NULL");
	log_assert(dobject, "is NULL");

	struct DObjectPos selfpos = dobject_getrealpos(self);
	struct DObjectPos dobjpos = dobject_getrealpos(dobject);

	if(selfpos.x < dobjpos.x + dobject->w)
		return 0;

	if(selfpos.x + self->changex >= dobjpos.x + dobject->w + dobject->changex)
		return 0;

	int intersecty = intersects_(
		selfpos.y + self->changey, 
		selfpos.y + self->h + self->changey, 
		dobjpos.y + dobject->changey, 
		dobjpos.y + dobject->h + dobject->changey
	);

	if(!intersecty)
		return 0;

	return 1;
}

int dobject_hittestright(struct DObject* self, struct DObject* dobject)
{
	log_assert(self, "is NULL");
	log_assert(dobject, "is NULL");

	struct DObjectPos selfpos = dobject_getrealpos(self);
	struct DObjectPos dobjpos = dobject_getrealpos(dobject);

	if(selfpos.x + self->w > dobjpos.x)
		return 0;

	if(selfpos.x + self->w + self->changex <= dobjpos.x + dobject->changex)
		return 0;

	int intersecty = intersects_(
		selfpos.y + self->changey, 
		selfpos.y + self->h + self->changey, 
		dobjpos.y + dobject->changey, 
		dobjpos.y + dobject->h + dobject->changey
	);

	if(!intersecty)
		return 0;

	return 1;
}

int dobject_hittesttop(struct DObject* self, struct DObject* dobject)
{
	log_assert(self, "is NULL");
	log_assert(dobject, "is NULL");

	struct DObjectPos selfpos = dobject_getrealpos(self);
	struct DObjectPos dobjpos = dobject_getrealpos(dobject);

	if(selfpos.y < dobjpos.y + dobject->h)
		return 0;

	if(selfpos.y + self->changey >= dobjpos.y + dobject->h + dobject->changey)
		return 0;

	int intersectx = intersects_(
		selfpos.x + self->changex, 
		selfpos.x + self->w + self->changex, 
		dobjpos.x + dobject->changex,  
		dobjpos.x + dobject->w + dobject->changex
	);

	if(!intersectx)
		return 0;

	return 1;
}

int dobject_hittestbottom(struct DObject* self, struct DObject* dobject)
{
	log_assert(self, "is NULL");
	log_assert(dobject, "is NULL");

	struct DObjectPos selfpos = dobject_getrealpos(self);
	struct DObjectPos dobjpos = dobject_getrealpos(dobject);

	if(selfpos.y + self->h > dobjpos.y)
		return 0;

	if(selfpos.y + self->h + self->changey <= dobjpos.y + dobject->changey)
		return 0;

	int intersectx = intersects_(
		selfpos.x + self->changex, 
		selfpos.x + self->w + self->changex, 
		dobjpos.x + dobject->changex,  
		dobjpos.x + dobject->w + dobject->changex
	);

	if(!intersectx)
		return 0;

	return 1;
}

void dobject_dtor(struct DObject* self)
{
	log_assert(self, "is NULL");
}

