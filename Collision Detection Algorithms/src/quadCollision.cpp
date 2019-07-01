#include <quadCollision.h>

using namespace std;

// Stores a list in each array slot
QuadCollision::QuadCollision()
{	
	for (int i = 0; i < GRID_COUNT; i++)
		particles[i] = new vector<Particle*>;
}


/*
	ParticleCollision::addContanct runs through the particles and if there is collision
	uses the two particles in question to run through the collision resolution
*/
unsigned QuadCollision::addContact(ParticleContact *contact, unsigned limit) const
{
	unsigned used = 0;

	used = listMethod(contact, limit, used);

	return used;
}


/*
	listMethod function runs the particle collision test using the list screen grid method
	filling vectors with particles depending on there position one the screen and then
	running through each list(grid) to check the particle collisions in that grid
*/
unsigned QuadCollision::listMethod(ParticleContact *contact, unsigned limit, unsigned used) const
{
	for (int i = 0; i < GRID_COUNT; i++)
	{
		if (particles[i]->size() >= 2)
		{
			for (vector<Particle*>::const_iterator j = particles[i]->cbegin(); j != particles[i]->cend()--; j++)
			{
				if (used >= limit) return used;
				for (vector<Particle*>::const_iterator k = particles[i]->cbegin()++; k != particles[i]->cend(); k++)
				{	// passes in the two particles to the checkCollision function and enters the if statement if they are colliding					
					if (checkCollision(*(*j), *(*k)))
					{
						// We have a collision
						// Run collision resolution for particles
						contact->contactNormal = ((*j)->getPosition() - (*k)->getPosition()).unit();
						contact->restitution = 1.0f;
						contact->particle[0] = (*j);
						contact->particle[1] = (*k);
						contact->penetration = (*j)->getRadius() - sqrt((*k)->getPosition().magnitude());
						used++;
						contact++;
					}
				}
			}
		}
	}
	return used;
}

// Draws a small green line between each particle that checks for collision with another particel
// This is used to easily visualise how many collisions are currently being done during runtime
void QuadCollision::drawLines()
{
	for (int i = 0; i < GRID_COUNT; i++)
	{
		if (particles[i]->size() >= 2)
		{
			for (vector<Particle*>::const_iterator j = particles[i]->cbegin(); j != particles[i]->cend()--; j++)
			{
				for (vector<Particle*>::const_iterator k = particles[i]->cbegin()++; k != particles[i]->cend(); k++)
				{
					glLineWidth(1);
					glColor3f(0.0, 1.0, 0.0);
					glBegin(GL_LINES);
					glVertex2f((*j)->getPosition().x, (*j)->getPosition().y);
					glVertex2f((*k)->getPosition().x, (*k)->getPosition().y);
					glEnd();
				}
			}
		}
	}
}


// checkCollision function that takes in two particles to check if they are colliding
bool QuadCollision::checkCollision(Particle circle1, Particle circle2) const
{
	//checks the distance between the two passed in particles
	float distance = (circle1.getPosition() - circle2.getPosition()).magnitude();

	//if the distance of the particles shows they are colliding then return true else return false
	if (distance < circle1.getRadius() + circle2.getRadius())
		return true;
	else
		return false;
}

// Clears lists
void QuadCollision::clearList()
{	
	for (int i = 0; i < GRID_COUNT; i++)
		particles[i]->clear();
}