#include "BaseApplication.h"

//解析命令行,读取设定,初始化所有子模式
int My::BaseApplication::Initialize()
{
	m_bQuit = false;


	return 0;
}

//终结所有子模式和清理所有运行时临时文件
void My::BaseApplication::Finalize()
{

}


//主循环中的一环
void My::BaseApplication::Tick()
{

}


bool My::BaseApplication::IsQuit()
{
	return m_bQuit;
}
