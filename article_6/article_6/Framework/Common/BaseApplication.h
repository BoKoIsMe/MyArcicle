#pragma once
#include "..\Interface\IApplication.h"

namespace My {
	class BaseApplication:implements IApplication
	{
	public:
		virtual int Initialize();
		virtual void Finalize();
		//��ѭ���е�һ��
		virtual void Tick();

		virtual bool IsQuit();

	protected:
		//����������ѭ���˳�������
		bool m_bQuit;
	private:

	};
}
