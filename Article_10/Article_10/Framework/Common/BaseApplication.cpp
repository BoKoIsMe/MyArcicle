#include "BaseApplication.h"

//����������,��ȡ�趨,��ʼ��������ģʽ
int My::BaseApplication::Initialize()
{
	m_bQuit = false;


	return 0;
}

//�ս�������ģʽ��������������ʱ��ʱ�ļ�
void My::BaseApplication::Finalize()
{

}


//��ѭ���е�һ��
void My::BaseApplication::Tick()
{

}


bool My::BaseApplication::IsQuit()
{
	return m_bQuit;
}
