#include<quadTree.h>

Quadtree::Quadtree(float x, float y, float width, float height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

// Deconstructs nodes
Quadtree::~Quadtree()
{
	if (NW != nullptr)
		delete NW;
	if (NE != nullptr)
		delete NE;
	if (SW != nullptr)
		delete SW;
	if (SE != nullptr)
		delete SE;
}

/* 
	First checks to ensure a partucle is within the current nodes bounds
	If yes then checks to make sure the this particle has node filled the current nodes capacity
	If so then checks to see if the node as already been divided, divides the node if needed
	Lastly moves down a node and checks again if the particel is within the current nodes bounds
 */
bool Quadtree::insert(Particle* particles)
{
	if (!this->contains(particles))
		return false;

	if (this->objects.capacity() < this->capacity)
	{
		this->objects.push_back(particles);
		return true;
	}
	else {
		if (!this->divided) {
			divide();

			for (int i = 0; i < this->capacity; i++)
				this->insert(objects[i]);

			this->objects.clear();
		}
		if (this->NW->insert(particles))
			return true;
		if (this->NE->insert(particles))
			return true;
		if (this->SW->insert(particles))
			return true;
		if (this->SE->insert(particles))
			return true;

		return false;
	}
}

// Sub-divides the current node into another 4 nodes for NorthWest(NW), NorthEast(NE), SouthWest(SW), SouthEast(SE)
// Then sets the nodes divided bool to true to show it has already been divided
void Quadtree::divide()
{
	this->NW = new Quadtree(x - (this->width / 4), y + (this->height / 4), (width / 2), (height / 2));
	this->NE = new Quadtree(x + (this->width / 4), y + (this->height / 4), (width / 2), (height / 2));
	this->SW = new Quadtree(x - (this->width / 4), y - (this->height / 4), (width / 2), (height / 2));
	this->SE = new Quadtree(x + (this->width / 4), y - (this->height / 4), (width / 2), (height / 2));
	this->divided = true;
}

// Checks if the node has been divided 
// If so recursively runs through checking each node returning the particles that exist within
// If not adds the particles to the particleSectors vector
vector<vector<Particle*>> Quadtree::getObjects(vector<vector<Particle*>> particleSectors)
{
	if (this->divided == true)
	{
		particleSectors = NW->getObjects(particleSectors);
		particleSectors = NE->getObjects(particleSectors);
		particleSectors = SW->getObjects(particleSectors);
		particleSectors = SE->getObjects(particleSectors);

		return particleSectors;
	}
	else
	{
		particleSectors.push_back(this->objects);
		return particleSectors;
	}
}

// Takes in a collision objects and checks to make sure it is within the current grid segments bounds
bool Quadtree::contains(Particle *object)
{
	Vector2 pos = object->getPosition();
	float radius = object->getRadius();

	if (pos.y + radius >= this->y - (this->height / 2))
	{
		if (pos.y - radius <= this->y + (this->height / 2))
		{
			if (pos.x + radius >= this->x - (this->width / 2))
			{
				if (pos.x - radius <= this->x + (this->width / 2))
					return true;
			}
		}
	}

	return false;
}

// Draws the grids of each node that exists dynamically as long as a node exists by taking the current nodes
// centre point halved for the width and hight to represent the new 4 nodes generated inside the current node
void Quadtree::showGrid()
{
	glLineWidth(1);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_LINES);
	glVertex2f(this->x - (width / 2), this->y);
	glVertex2f(this->x + (width / 2), this->y);
	glVertex2f(this->x, this->y - (height / 2));
	glVertex2f(this->x, this->y + (height / 2));
	glEnd();

	if (this->NW->NW != nullptr)
			this->NW->showGrid();
	if (this->NE->NE != nullptr)
			this->NE->showGrid();
	if (this->SW->SW != nullptr)
			this->SW->showGrid();
	if (this->SE->SE != nullptr)
			this->SE->showGrid();

}