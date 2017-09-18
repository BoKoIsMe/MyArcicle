#pragma once
#include "..\Interface\IApplication.h"

namespace My {
	class BaseApplication:implements IApplication
	{
	public:
		virtual int Initialize();
		virtual void Finalize();
		//主循环中的一环
		virtual void Tick();

		virtual bool IsQuit();

	protected:
		//从主程序主循环退出的旗帜
		bool m_bQuit;
	private:

	};
}
