#include <nodeTree.h>

NodeTree::NodeTree(Particle* particle, Vector2 size, int numOfP)
{
	this->sizeOfScreen = size;
	this->particles = particle;
	this->numOfParticles = numOfP;
}

void NodeTree::update()
{
	createRootNode();
}

// Creates a new quadtree called root node for use as the dynamic screen segmentation
void NodeTree::createRootNode()
{
	if (rootNode != nullptr)
		delete rootNode;

	rootNode = new Quadtree(0, 0, sizeOfScreen.x, sizeOfScreen.y);

	for (int i = 0; i < numOfParticles; i++)
	{
		rootNode->insert(&particles[i]);
	}
}

/*
	quad tree method checks each particle within a certain grid space with any other particel within that same grid 
	space for possible collision and resolvs that collision - highly efficient compared to brute force and simple quad
*/
unsigned NodeTree::addContact(ParticleContact * contact, unsigned limit) const
{
	unsigned used = 0;

	vector<vector<Particle*>> pGrid;
	pGrid = rootNode->getObjects(pGrid);

	for (vector<vector<Particle*>>::iterator i = pGrid.begin(); i != prev(pGrid.end()); i++)
	{
		if ((*i).size() >= 2)
		{
			for (vector<Particle*>::iterator j = (*i).begin(); j != prev((*i).end()); j++)
			{
				if (used >= limit) return used;
				for (vector<Particle*>::iterator k = next(j); k != (*i).end(); k++)
				{	// passes in the two particles to the checkCollision function and enters the if statement if they are colliding		
					if (checkCollision((*j), (*k)))
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
void NodeTree::drawLines()
{
	vector<vector<Particle*>> pGrid;
	pGrid = rootNode->getObjects(pGrid);

	for (vector<vector<Particle*>>::iterator i = pGrid.begin(); i != prev(pGrid.end()); i++)
	{
		if ((*i).size() >= 2)
		{
			for (vector<Particle*>::iterator j = (*i).begin(); j != prev((*i).end()); j++)
			{
				for (vector<Particle*>::iterator k = next(j); k != (*i).end(); k++)
				{
					glColor3f(0.0f, 1.0f, 0.0f);
					glBegin(GL_LINES);
					glVertex2f((*j)->getPosition().x, (*j)->getPosition().y);
					glVertex2f((*k)->getPosition().x, (*k)->getPosition().y);
					glEnd();
				}
			}
		}
	}
}

// Calls the show grid function for this root node drawing the dynamic grids as they appear and delete
void NodeTree::drawTree()
{
	rootNode->showGrid();
}

bool NodeTree::checkCollision(Particle* circle1, Particle* circle2) const
{
	//checks the distance between the two passed in particles
	float distance = (circle1->getPosition() - circle2->getPosition()).magnitude();

	//if the distance of the particles shows they are colliding then return true else return false
	if (distance < circle1->getRadius() + circle2->getRadius())
		return true;
	else
		return false;
}