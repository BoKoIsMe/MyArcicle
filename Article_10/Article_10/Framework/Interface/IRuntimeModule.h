#pragma once

#include "Interface.h"

namespace My {
	Interface IRuntimeModule{
public:
	virtual ~IRuntimeModule() {};

	virtual int Initialize() = 0;//初始化模块
	virtual void Finalize() = 0;//模块结束，清除现场

	virtual void Tick() = 0;//驱动模块执行
	};
}
