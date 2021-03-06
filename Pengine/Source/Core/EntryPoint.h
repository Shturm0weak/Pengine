#pragma once

#include "Core.h"
#include "Application.h"

namespace Pengine
{

	class PENGINE_API EntryPoint
	{
	private:

		static Application* m_Application;

		void PrepareResources();
	public:

		EntryPoint() = default;
		~EntryPoint();

		static Application& GetApplication() { return *m_Application; }

		void SetApplication(Application* application, bool startState = false);
		
		void OnStart();
		
		void OnUpdate();
		
		void OnClose();
	};

}