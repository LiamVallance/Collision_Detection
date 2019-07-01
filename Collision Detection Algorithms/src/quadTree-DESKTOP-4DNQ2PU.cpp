#include<quadTree.h>

Quadtree::Quadtree(float x, float y, float width, float height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

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
			this->divide();

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

		else return false;
	}
}

void Quadtree::divide()
{
	this->NW = new Quadtree(x - (this->width / 4), y + (this->height / 4), (width / 2), (height / 2));
	this->NE = new Quadtree(x + (this->width / 4), y + (this->height / 4), (width / 2), (height / 2));
	this->SW = new Quadtree(x - (this->width / 4), y - (this->height / 4), (width / 2), (height / 2));
	this->SE = new Quadtree(x + (this->width / 4), y - (this->height / 4), (width / 2), (height / 2));
	
	this->divided = true;
}

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

void Quadtree::PreOrder(Quadtree *quad) {
	if (quad != nullptr) {
		PreOrder(quad->NW);
		PreOrder(quad->NE);
		PreOrder(quad->SW);
		PreOrder(quad->SE);
	}
}

bool Quadtree::contains(Particle *object)
{
	Vector2 pos = object->getPosition();
	float radius = object->getRadius();

	if (this != nullptr)
	{
		if (pos.y + radius >= this->y - (this->height / 2))
			if (pos.y - radius <= this->y + (this->height / 2))
				if (pos.x + radius >= this->x - (this->width / 2))
					if (pos.x - radius <= this->x + (this->width / 2))
						return true;
	}

	return false;
}


void Quadtree::showGrid()
{
	glLineWidth(1);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_LINES);
	glVertex2f(this->x - (this->width / 2), this->y);
	glVertex2f(this->x + (this->width / 2), this->y);
	glVertex2f(this->x, this->y - (this->height / 2));
	glVertex2f(this->x, this->y + (this->height / 2));
	glEnd();

	if (this->NW != nullptr)
		this->NW->showGrid();
	if (this->NE != nullptr)
		this->NE->showGrid();
	if (this->SW != nullptr)
		this->SW->showGrid();
	if (this->SE != nullptr)
		this->SE->showGrid();

	
	//glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 1.0);
	//glVertex2f(0, -width);
	//glVertex2f(0, width);
	//glVertex2f(-height, 0);
	//glVertex2f(height, 0);
	//glEnd();

}