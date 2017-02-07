#ifndef SCENE_H
#define SCENE_H

struct Game;
struct Scene;

typedef struct SDL_Renderer SDL_Renderer;
typedef void(*SceneUpdate)(
	struct Scene* scene, 
	struct Game* window, 
	void* userdata
);

typedef void(*SceneRender)(
	struct Game* game,
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

