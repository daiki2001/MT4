#pragma once

class SceneChenger
{
public: // �T�u�N���X
	enum Scene
	{
		Test
	};

public: // �����o�֐�
	SceneChenger() {}
	~SceneChenger() {}

	virtual void SceneChenge(const SceneChenger::Scene scene, const bool stackClear) = 0;
};
