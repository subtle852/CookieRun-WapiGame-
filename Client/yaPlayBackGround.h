#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;

	class PlayBackGround : public GameObject
	{
	public:
		PlayBackGround();
		~PlayBackGround();

		virtual void Initialize();
		virtual void Update();
		virtual void Render(HDC hdc);
		virtual void Release();

	private:
		Animator* mAnimator;
	};
}