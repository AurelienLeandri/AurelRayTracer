#pragma once

class Scene;

class Integrator
{
public:
	virtual ~Integrator() = default;
	virtual void render(const Scene &sceneData) = 0;
};

