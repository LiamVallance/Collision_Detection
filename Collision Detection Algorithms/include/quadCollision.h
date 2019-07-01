#pragma once
#include <gl/glut.h>
#include <particle.h>
#include <pcontacts.h>
#include <vector>

#define GRID_COUNT 4

using namespace std;

class QuadCollision : public ParticleContactGenerator
{
public:
	/*
		Holds a pointer to the particles we're checking for collisions with.
	*/
	vector<Particle*> *particles[GRID_COUNT];
	void clearList();
	void drawLines();

	unsigned listMethod(ParticleContact *contact, unsigned limit, unsigned used) const;

	QuadCollision();

	virtual unsigned addContact(
		ParticleContact *contact,
		unsigned limit
	) const;


	// bool checks if collison has happened
	bool checkCollision(Particle, Particle) const;
};
