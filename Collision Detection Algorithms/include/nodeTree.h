#pragma once
#include <gl/glut.h>
#include <particle.h>
#include <list>
#include <vector>
#include <pcontacts.h>
#include <quadTree.h>

class NodeTree : public ParticleContactGenerator
{
private:
	Vector2 sizeOfScreen;
	Vector2 position;
	int numOfParticles;
	Quadtree* rootNode = nullptr;
	bool checkCollision(Particle*, Particle*) const;

public:
	NodeTree(Particle* particle, Vector2 size, int numOfP);
	void createRootNode();
	Particle* particles;
	void update();
	virtual unsigned addContact(
		ParticleContact *contact,
		unsigned limit
	) const;

	void drawLines();
	void drawTree();
};