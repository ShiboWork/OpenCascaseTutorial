#pragma once

/**
 * @file BaseScene.h
 * @brief ���� BaseScene �����࣬��Ϊ���г�����Ļ��ࡣ
 *
 * �����ṩ��һ�����麯�� displayScene()�������� Open CASCADE ������������ʾ������
 */

#include "pch.h"
#include "../Log.h"
#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>

 /**
  * @class BaseScene
  * @brief �������࣬�����˳����Ļ����ӿڡ�
  *
  * ������һ��������࣬�ṩ��һ�����麯�� displayScene()��Ҫ������������ʵ�־���ĳ�����ʾ�߼���
  */
class BaseScene {
public:
	BaseScene() = default;
	/**
	 * @brief ����������ȷ����������ȷ���١�
	 */
	virtual ~BaseScene() = default;

	/**
	 * @brief ���麯������������ʵ�־���ĳ�����ʾ�߼���
	 *
	 * @param context Open CASCADE ���������ģ��ṩ��ʾ��������� API��
	 */
	virtual void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) = 0;

	/**
	 * @brief �麯�����������ʵ�ֳ�ʼ���߼���
	 *
	 * @param context Open CASCADE ���������ģ��ṩ��ʾ��������� API��
	 */
	virtual void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) = 0;

	void setLog(OCCImguiTutorial::Log* log) { m_Log = log; };

protected:
	OCCImguiTutorial::Log* m_Log = nullptr;

	bool bIsSceneInit = false;
};