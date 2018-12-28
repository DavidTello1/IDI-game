#ifndef __P2ANIMATION_H__
#define __P2ANIMATION_H__
#include <assert.h>
#include "SDL/include/SDL_rect.h"
#include "j1Scene.h"

#define MAX_FRAMES 50

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];


private:

	float current_frame;
	int last_frame = 0;
	int loops = 0;

public:

	void PushBack(const SDL_Rect& rect)
	{
		if (last_frame < MAX_FRAMES)
			frames[last_frame++] = rect;
		assert(last_frame < MAX_FRAMES);
	}

	SDL_Rect& GetCurrentFrame(float dt)
	{
		float tmp = speed;

		if (App->scene->pause == false)
		{
			current_frame += tmp*dt;
		}

		if (current_frame < 0)
		{
			current_frame = 0;
		}

		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}

		return frames[(int)current_frame];
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
	}

	void ResetLoops()
	{
		loops = 0;
	}

	int GetLastFrame()
	{
		return last_frame;
	}
};

#endif