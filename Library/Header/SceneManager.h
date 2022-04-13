#pragma once
#include "SceneChenge.h"
#include "BaseScene.h"
#include <memory>
#include <stack>

class SceneManager : public SceneChenger
{
public: // �����o�֐�
	SceneManager();
	~SceneManager() {}

	void Loop() const;
	void SceneChenge(const SceneChenger::Scene scene, const bool stackClear) override;

private: // �����o�ϐ�
	std::stack<std::shared_ptr<BaseScene>> sceneStack;
};
