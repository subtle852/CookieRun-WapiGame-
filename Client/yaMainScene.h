#pragma once
#include "yaScene.h"
#include "yaImage.h"

namespace ya
{
	class Character01;
	class Pet01;
	class Sound;

	class MainScene : public Scene
	{
	public:
		MainScene();
		~MainScene();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnEnter() override;
		virtual void OnExit() override;

	private:
		Character01* mCh00;
		Pet01* mPet00;

		Sound* mBGM;

	public:
		static int mSilver;
		static int mDia;

		GameObject* mOb = nullptr;
		GameObject* mObs[6] = { nullptr, };
		GameObject* mObs2[6] = { nullptr, };

		bool mNoReset = false;
	};
}