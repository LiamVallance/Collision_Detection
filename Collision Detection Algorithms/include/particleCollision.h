#pragma once
#include <gl/glut.h>
#include <particle.h>
#include <pcontacts.h>
#include <list>

using namespace std;



class ParticleCollision : public ParticleContactGenerator
{
public:
	/*
		Holds a pointer to the particles we're checking for collisions with.
	*/
	Particle *particles;
	int blob_count;
	void drawLines();

	unsigned bruteForce(ParticleContact *contact, unsigned limit, unsigned used) const;

	virtual unsigned addContact(
		ParticleContact *contact,
		unsigned limit
	) const;

	// bool checks if collison has happened
	bool checkCollision(Particle, Particle) const;
};