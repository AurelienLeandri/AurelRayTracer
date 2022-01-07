#pragma once

class SceneData;

class Integrator
{
public:
	virtual ~Integrator() = default;
	virtual void render(const SceneData &sceneData) = 0;
};

