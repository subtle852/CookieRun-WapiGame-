#include "yaAnimator.h"
#include "yaResources.h"
#include "yaInput.h"

#include "yaCharacter01.h"

namespace ya
{
	Animator::Animator()
		: Component(eComponentType::Animator)
		, mActiveAnimation(nullptr)
		, mSpriteSheet(nullptr)
		, mbLoop(false)
	{

	}

	Animator::~Animator()
	{
		for (auto animation : mAnimations)
		{
			delete animation.second;
			animation.second = nullptr;
		}

		for (auto events : mEvents)
		{
			delete events.second;
			events.second = nullptr;
		}
	}

	void Animator::Initialize()
	{

	}

	void Animator::Update()
	{
		if (mActiveAnimation)
		{
			mActiveAnimation->Update();

			if (mActiveAnimation->IsComplete())
			{
				Animator::Events* events
					= FindEvents(mActiveAnimation->GetName());

				if (events != nullptr)
					events->mCompleteEvent();
			}

			if (mbLoop && mActiveAnimation->IsComplete())
				mActiveAnimation->Reset();
		}
	}

	void Animator::Render(HDC hdc)
	{
		if (mActiveAnimation)
		{
			mActiveAnimation->Render(hdc);
		}
	}

	void Animator::Release()
	{

	}

	void Animator::CreateAnimation(const std::wstring& name
		, Image* sheet, Vector2 leftTop
		, UINT coulmn, UINT row, UINT spriteLength
		, Vector2 offset, float duration, bool apb)
	{
		mApb = apb;

		Animation* animation = FindAnimation(name);


		if (animation != nullptr)
			return;

		animation = new Animation();
		animation->Create(sheet, leftTop, coulmn, row, spriteLength, offset, duration, apb);
		animation->SetName(name);
		animation->SetAnimator(this);

		mAnimations.insert(std::make_pair(name, animation));
		Events* event = new Events();
		mEvents.insert(std::make_pair(name, event));
	}

	void Animator::CreateAnimations(const std::wstring& path, Vector2 offset, float duration, bool apb)
	{
		mApb = apb;

		UINT width = 0;
		UINT height = 0;
		UINT fileCount = 0;

		std::filesystem::path fs(path);
		std::vector<Image*> images = {};
		for (const auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			std::wstring fileName = p.path().filename();
			std::wstring fullName = path + L"\\" + fileName;

			const std::wstring ext = p.path().extension();
			if (ext == L".png")
				continue;

			Image* image = Resources::Load<Image>(fileName, fullName);
			images.push_back(image);

			if (width < image->GetWidth())
			{
				width = image->GetWidth();
			}
			if (height < image->GetHeight())
			{
				height = image->GetHeight();
			}
			fileCount++;
		}

		std::wstring key = fs.parent_path().filename();
		key += fs.filename();
		mSpriteSheet = Image::Create(key, width * fileCount, height);

		//
		int index = 0;
		for (Image* image : images)
		{
			int centerX = (width - image->GetWidth()) / 2;
			int centerY = (height - image->GetHeight());

			BitBlt(mSpriteSheet->GetHdc()
				, width * index + centerX
				, 0 + centerY
				, image->GetWidth(), image->GetHeight()
				, image->GetHdc(), 0, 0, SRCCOPY);

			index++;
		}

		CreateAnimation(key, mSpriteSheet, Vector2::Zero, index, 1, index, offset, duration, apb);
	}

	Animation* Animator::FindAnimation(const std::wstring& name)
	{
		std::map<std::wstring, Animation*>::iterator iter
			= mAnimations.find(name);

		if (iter == mAnimations.end())
			return nullptr;

		return iter->second;
	}

	void Animator::Play(const std::wstring& name, bool loop)
	{
		//if (mActiveAnimation != nullptr)
		//{
		//	Animator::Events* prevEvents = FindEvents(mActiveAnimation->GetName());

		//	if (prevEvents != nullptr)
		//	{
		//		prevEvents->mEndEvent();
		//	}
		//}

		//mActiveAnimation = FindAnimation(name);

		//// 지금 문제가 
		//// a 애니메이션 진행중인 도중에(a 애니메이션 덜 끝난 상태)
		//// 커맨드로 인해 b 동작이 실행되었어
		//// 그럴 때 a 애니메이션이 덜 끝났지만 강제로 리셋을 시켜줘야함

		//if (Character01::mDJmpcnt <= 1 && Input::GetKeyUp(eKeyCode::W))
		//{
		//	mActiveAnimation->Reset();
		//}

		////애니메이션이 끝났을 때만 리셋해주어야 함
		//if (mActiveAnimation->IsComplete())
		//{
		//	mActiveAnimation->Reset();
		//}
		//mbLoop = loop;

		//Animator::Events* events = FindEvents(mActiveAnimation->GetName());

		//if (events != nullptr)
		//{
		//	events->mStartEvent();
		//}

		if (FindAnimation(name) == mActiveAnimation)
			return;
		Animator::Events* events = FindEvents(name);
		if (events != nullptr) events->mStartEvent();


		Animation* prevAnimation = mActiveAnimation;

		mActiveAnimation = FindAnimation(name);

		mActiveAnimation->Reset();
		mbLoop = loop;

		if (prevAnimation != mActiveAnimation) {
			if (events != nullptr) events->mEndEvent();
		}
	}

	Animator::Events* Animator::FindEvents(const std::wstring& name)
	{
		std::map<std::wstring, Events*>::iterator iter = mEvents.find(name);

		if (iter == mEvents.end())
			return nullptr;

		return iter->second;
	}

	std::function<void()>& Animator::GetStartEvent(const std::wstring& name)
	{
		Animation* animation = FindAnimation(name);

		Animator::Events* events = FindEvents(animation->GetName());

		return events->mStartEvent.mEvent;
	}

	std::function<void()>& Animator::GetCompleteEvent(const std::wstring& name)
	{
		Animation* animation = FindAnimation(name);

		Animator::Events* events = FindEvents(animation->GetName());

		return events->mCompleteEvent.mEvent;
	}

	std::function<void()>& Animator::GetEndEvent(const std::wstring& name)
	{
		Animation* animation = FindAnimation(name);

		Animator::Events* events = FindEvents(animation->GetName());

		return events->mEndEvent.mEvent;
	}
}