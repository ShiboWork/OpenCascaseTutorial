#pragma once

/**
 * @file BaseScene.h
 * @brief 定义 BaseScene 抽象类，作为所有场景类的基类。
 *
 * 该类提供了一个纯虚函数 displayScene()，用于在 Open CASCADE 交互环境中显示场景。
 */

#include "pch.h"
#include "../Log.h"
#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>

 /**
  * @class BaseScene
  * @brief 场景基类，定义了场景的基本接口。
  *
  * 该类是一个抽象基类，提供了一个纯虚函数 displayScene()，要求所有派生类实现具体的场景显示逻辑。
  */
class BaseScene {
public:
	BaseScene() = default;
	/**
	 * @brief 析构函数，确保派生类正确销毁。
	 */
	virtual ~BaseScene() = default;

	/**
	 * @brief 纯虚函数，派生类需实现具体的场景显示逻辑。
	 *
	 * @param context Open CASCADE 交互上下文，提供显示场景所需的 API。
	 */
	virtual void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) = 0;

	/**
	 * @brief 虚函数，派生类可实现初始化逻辑。
	 *
	 * @param context Open CASCADE 交互上下文，提供显示场景所需的 API。
	 */
	virtual void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) = 0;

	void setLog(OCCImguiTutorial::Log* log) { m_Log = log; };

protected:
	OCCImguiTutorial::Log* m_Log = nullptr;

	bool bIsSceneInit = false;
};