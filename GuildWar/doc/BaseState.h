
// *-----------------------------------------------------------------
//
// 状态基类
//
// *-----------------------------------------------------------------

#pragma once

class BaseState
{
public:
	virtual ~BaseState() {}

	// 状态开始时的回调方法
	virtual void enter() {}

	// 状态结束时的回调方法
	virtual void leave() {}

	// 处理接口 各状态自行定义该接口的意义
	// ***TODO*** 用boost::any作参数是否比void*要好一些?
	virtual bool process(void* arg = 0) { return true; }
};
