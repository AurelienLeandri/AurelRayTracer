#pragma once

class Scene;

class IntegratorOld
{
public:
	virtual ~IntegratorOld() = default;
	virtual void render(const Scene &sceneData) = 0;
};

