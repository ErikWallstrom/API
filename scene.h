#ifndef SCENE_H
#define SCENE_H

struct Game;
struct Scene;

typedef struct SDL_Renderer SDL_Renderer;
typedef void(*SceneUpdate)(
	struct Scene* scene, 
	struct Game* game
);

typedef void(*SceneRender)(
	struct Scene* scene, 
	struct Game* game,
	double interpolation
);

enum SceneChange
{
	SCENECHANGE_NONE,
	SCENECHANGE_PREV,
	SCENECHANGE_NEXT
};

struct Scene
{
	SceneUpdate update;
	SceneRender render;
	void* userdata;
	enum SceneChange change;
};

struct Scene* scene_ctor(
	struct Scene* self, 
	SceneUpdate update,
	SceneRender render, 
	void* userdata
);

#endif

