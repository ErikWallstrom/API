#ifndef SCENE_H
#define SCENE_H

enum SceneChange
{
	SCENE_CHANGE_NONE,
	SCENE_CHANGE_PREV,
	SCENE_CHANGE_NEXT
};

struct Game;
struct Scene;

typedef void(*SceneUpdate)(struct Scene*, struct Game*, void*);

struct Scene
{
	SceneUpdate update;
	enum SceneChange change;
};

#endif

