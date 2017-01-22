#ifndef SCENE_H
#define SCENE_H

struct Window;
struct Scene;

typedef struct SDL_Renderer SDL_Renderer;
typedef void(*SceneUpdate)(
	struct Scene* scene, 
	struct Window* window, 
	void* userdata
);

typedef void(*SceneRender)(
	SDL_Renderer* renderer, 
	double interpolation, 
	void* userdata
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
	enum SceneChange change;
};

#endif

