#include <particleCollision.h>
#include <particle.h>

using namespace std;

/*
	ParticleCollision::addContanct runs through the particles and if there is collision
	uses the two particles in question to run through the collision resolution
*/
unsigned ParticleCollision::addContact(ParticleContact *contact, unsigned limit) const
{
	unsigned used = 0;

	used = bruteForce(contact, limit, used);

	return used;
}

// Draws a small green line between each particle that checks for collision with another particel
// This is used to easily visualise how many collisions are currently being done during runtime
void ParticleCollision::drawLines()
{
		for (int i = 0; i < blob_count; i++)
		{
			for (int j = i + 1; j < blob_count; j++)
			{
				glLineWidth(1);
				glColor3f(0.0, 1.0, 0.0);
				glBegin(GL_LINES);
				glVertex2f(particles[i].getPosition().x, particles[i].getPosition().y);
				glVertex2f(particles[j].getPosition().x, particles[j].getPosition().y);
				glEnd();
			}
		}
}


/*
	bruteForce dunction runs the particle collision test using the brute force method
	checking every particle with every particle (inefficent at larger particle numbers)
*/
unsigned ParticleCollision::bruteForce(ParticleContact *contact, unsigned limit, unsigned used) const
{
	for (int i = 0; i < blob_count; i++)
	{
		if (used >= limit) return used;
		for (int j = i + 1; j < blob_count; j++)
		{
			if (checkCollision(particles[i], particles[j]))
			{
				//std::cout << "Contact between " << i << " + " << j << std::endl;
				// We have a collision
				// Run collision resolution for particles
				contact->contactNormal = (particles[i].getPosition() - particles[j].getPosition()).unit();
				contact->restitution = 1.0f;
				contact->particle[0] = particles + i;
				contact->particle[1] = particles + j;
				contact->penetration = particles[i].getRadius() - sqrt(particles[j].getPosition().magnitude());
				used++;
				contact++;
			}
		}
	}
	return used;
}


// checkCollision function that takes in two particles to check if they are colliding
bool ParticleCollision::checkCollision(Particle circle1, Particle circle2) const
{
	//checks the distance between the two passed in particles
	float distance = (circle1.getPosition() - circle2.getPosition()).magnitude();

	//if the distance of the particles shows they are colliding then return true else return false
	if (distance < circle1.getRadius() + circle2.getRadius())
		return true;
	else
		return false;
}