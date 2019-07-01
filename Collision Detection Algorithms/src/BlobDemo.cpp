/*
 * The Blob demo.
 *
 */
#include <gl/glut.h>
#include "app.h"
#include "coreMath.h"
#include "pcontacts.h"
#include "pworld.h"
#include <stdio.h>
#include <cassert>
#include <vector>
#include <iostream>
#include <list>
#include <ctime>
#include <chrono>
#include <nodeTree.h>
#include <particleCollision.h>
#include <quadCollision.h>
#include <string>;

using namespace std;

// Constant numbers of platforms, boarders and particles
int BLOB_COUNT;
//#define BLOB_COUNT 20
#define PLAT_COUNT 2
#define BAORDER_COUNT 4
#define RADIUS 2

int radius = RADIUS;
float damp = 1;
float mass = 100.0;

// Boolean values used to toggle the lines between the particles and the grids on and off
bool showLines = false;
bool showGrid = true;
bool random = false;

const Vector2 Vector2::GRAVITY = Vector2(0, -9.81);
/*
	Platforms are two dimensional: lines on which the 
	particles can rest. Platforms are also contact generators for the physics.
*/

class Platform : public ParticleContactGenerator
{
public:
    Vector2 start;
    Vector2 end;
    /*
		Holds a pointer to the particles we're checking for collisions with. 
    */
    Particle *particles;
	float restitution;

    virtual unsigned addContact(
        ParticleContact *contact, 
        unsigned limit
        ) const;
};

unsigned Platform::addContact(ParticleContact *contact, 
                              unsigned limit) const
{
    
	//const static float restitution = 0.8f;
	//const static float restitution = 1.0f;
	unsigned used = 0;

	for (unsigned i = 0; i < BLOB_COUNT; i++) {

		if (used >= limit) return used;

		// Check for penetration
		Vector2 toParticle = particles[i].getPosition() - start;
		Vector2 lineDirection = end - start;

		float projected = toParticle * lineDirection;
		float platformSqLength = lineDirection.squareMagnitude();
		float squareRadius = particles[i].getRadius()*particles[i].getRadius();;

		if (projected <= 0)
		{

			// The blob is nearest to the start point
			if (toParticle.squareMagnitude() < squareRadius)
			{
				// We have a collision
				contact->contactNormal = toParticle.unit();
				contact->restitution = restitution;
				contact->particle[0] = particles + i;
				contact->particle[1] = 0;
				contact->penetration = particles[i].getRadius() - toParticle.magnitude();
				used++;
				contact++;
			}

		}
		else if (projected >= platformSqLength)
		{
			// The blob is nearest to the end point
			toParticle = particles[i].getPosition() - end;
			if (toParticle.squareMagnitude() < squareRadius)
			{
				// We have a collision
				contact->contactNormal = toParticle.unit();
				contact->restitution = restitution;
				contact->particle[0] = particles + i;
				contact->particle[1] = 0;
				contact->penetration = particles[i].getRadius() - toParticle.magnitude();
				used++;
				contact++;
			}
		}
		else
		{
			// the blob is nearest to the middle.
			float distanceToPlatform = toParticle.squareMagnitude() - projected * projected / platformSqLength;
			if (distanceToPlatform < squareRadius)
			{
				// We have a collision
				Vector2 closestPoint = start + lineDirection * (projected / platformSqLength);

				contact->contactNormal = (particles[i].getPosition() - closestPoint).unit();
				contact->restitution = restitution;
				contact->particle[0] = particles + i;
				contact->particle[1] = 0;
				contact->penetration = particles[i].getRadius() - sqrt(distanceToPlatform);
				used++;
				contact++;
			}
		}
	}
	return used;
}


class BlobDemo : public Application
{
	Particle *blob;
	Platform *platforms;
	Platform *boarders;
    ParticleWorld world;
	ParticleCollision *blobCollision;
	QuadCollision *quadCollision;
	NodeTree *QuadTree;
	
	// State handeling, each state runs a different collision detection alogrithm
	enum state {BRUTE_FORCE, QUAD, QUAD_TREE};
	state currentState = state::BRUTE_FORCE;
	void currentActiveState(state algorithm);

public:
    /** Creates a new demo object. */
    BlobDemo();
    virtual ~BlobDemo();

	void listGrid();
	void drawGrid();

    /** Returns the window title for the demo. */
    virtual const char* getTitle();

    /** Display the particles. */
    virtual void display();

	void resetContactGenerator();
	void switchCurrentState();

    /** Update the particle positions. */
    virtual void update();

	virtual void keyboard(unsigned char key, int x, int y);
};

// Keystroke handeling to allow the user to toggle lines and grids drawing ans switch between states
void BlobDemo::keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case VK_SPACE:
			switchCurrentState();
			break;

		case 'l':
		case 'L':
			showLines = !showLines;
			break;

		case 'g':
		case 'G':
			showGrid = !showGrid;
			break;
	}
}



// Method definitions
BlobDemo::BlobDemo() :world(PLAT_COUNT + BAORDER_COUNT + BLOB_COUNT, PLAT_COUNT + BAORDER_COUNT + BLOB_COUNT)
{
	cout << "SPACE - switches between collision detection algorithms" << endl;
	cout << "G - turns Grid Lines ON/Off" << endl;
	cout << "L - turns Collision Detection Lines ON/OFF" << endl;
	cout << "Particle Count: ";
	cin >> BLOB_COUNT;
	width = 400; height = 400;
	nRange = 100.0;
	float margin = 0.99;
	

	// Create the blob storage
	blob = new Particle[BLOB_COUNT];

	// Create the boarders
	boarders = new Platform[BAORDER_COUNT];

	blobCollision = new ParticleCollision;

	quadCollision = new QuadCollision;

	// Set restitution to all boarders
	for (unsigned i = 0; i < BAORDER_COUNT; i++)
		boarders[i].restitution = 1.0f;

	// Position boarders round edge of screen
	boarders[0].start = Vector2(-nRange*margin, -nRange * margin);
	boarders[0].end = Vector2(-nRange * margin, nRange*margin);
			   
	boarders[1].start = Vector2(-nRange * margin, nRange*margin);
	boarders[1].end = Vector2(nRange*margin, nRange*margin);
			   
	boarders[2].start = Vector2(nRange*margin, nRange*margin);
	boarders[2].end = Vector2(nRange*margin, -nRange * margin);
			   
	boarders[3].start = Vector2(nRange*margin, -nRange * margin);
	boarders[3].end = Vector2(-nRange * margin, -nRange * margin);

    // Create the platform
	platforms = new Platform[PLAT_COUNT];

	// Possition platform in V shape near top of screen
	platforms[0].start = Vector2(-50.0, 30.0);
	platforms[0].end = Vector2(0.0, 0.0);
	platforms[0].restitution = 1.0f;
				
	platforms[1].start = Vector2(50.0, 30.0);
	platforms[1].end = Vector2(0.0, 0.0);
	platforms[1].restitution = 1.0f;

    // Make sure the platform knows which particle it should collide with.
	for (unsigned i = 0; i < PLAT_COUNT; i++) {
		platforms[i].particles = blob;
		world.getContactGenerators().push_back(platforms + i);
	}
	
	for (unsigned i = 0; i < BAORDER_COUNT; i++) {
		boarders[i].particles = blob;
		world.getContactGenerators().push_back(boarders + i);
	}

    // Create the blob
	for (unsigned i = 0; i < BLOB_COUNT; i++) {
		if (random)
		{
			radius = rand() % 8;
			damp = (float)rand() / RAND_MAX;
			mass = rand() % 100 + 1;
		}

		//blob[i].setPosition(-50.0 + i*25, 80.0);
		blob[i].setPosition((-nRange + radius) * margin + rand()% 200, rand() % 97 - radius);
		blob[i].setRadius(radius);
		blob[i].setVelocity(0, 0);
		blob[i].setDamping(damp);
		blob[i].setAcceleration(Vector2::GRAVITY * 10.0f);
		blob[i].setMass(mass);
		blob[i].clearAccumulator();
				
		world.getParticles().push_back(blob + i);
	}
	// Passes BLOB_COUNT into the particle collision class so that only one value needs to be changed to up the particle count
	blobCollision->blob_count = BLOB_COUNT;

	currentActiveState(BRUTE_FORCE);
}


// Selects next state depending on current state
void BlobDemo::switchCurrentState()
{
	if (currentState == BRUTE_FORCE)
		currentState = state::QUAD;
	else if (currentState == QUAD)
		currentState = state::QUAD_TREE;
	else if (currentState == QUAD_TREE)
		currentState = state::BRUTE_FORCE;

	currentActiveState(currentState);
}

// Passes the correct algorithm into the contact genreator depending on the current state after clearing the previouse algorithm from the contact generator
void BlobDemo::currentActiveState(state algorithm)
{
	if (algorithm == BRUTE_FORCE)
	{
		cout << "BRUTE_FORCE active" << endl;
		resetContactGenerator();
		blobCollision->particles = blob;
		world.getContactGenerators().push_back(blobCollision);
	}
	else if (algorithm == QUAD)
	{
		cout << "QUAD active" << endl;
		resetContactGenerator();
		world.getContactGenerators().push_back(quadCollision);
		listGrid();
	}
	else if (algorithm == QUAD_TREE)
	{
		cout << "QUAD_TREE active" << endl;
		resetContactGenerator();
		QuadTree = new NodeTree(blob, Vector2(nRange * 2.0, nRange * 2.0), BLOB_COUNT);
		world.getContactGenerators().push_back(QuadTree);
		QuadTree->update();
	}
}

// Clears the current algorithm out of the contact generator and sets it back to default ready for a new collision algorithm
void BlobDemo::resetContactGenerator()
{
	world.getContactGenerators().clear();

	// Make sure the platform knows which particle it should collide with.
	for (unsigned i = 0; i < PLAT_COUNT; i++) {
		//platforms[i].particles = blob;
		world.getContactGenerators().push_back(platforms + i);
	}

	for (unsigned i = 0; i < BAORDER_COUNT; i++) {
		//boarders[i].particles = blob;
		world.getContactGenerators().push_back(boarders + i);
	}
}

// Places particles into appropriate vector based on particles curront position
void BlobDemo::listGrid()
{
	quadCollision->clearList();

	for (unsigned i = 0; i < BLOB_COUNT; i++)
	{
		Vector2 pos = blob[i].getPosition();
		float radius = blob[i].getRadius();
		// checks particle position in the grid layout and fills lists with relevent particles
		// grid[0] = top left, grid[1] = top right, grid[2] = bottom right, grid[3] = bottom left
		if (pos.y + radius > 0)
		{
			if (pos.x - radius < 0)
				quadCollision->particles[0]->push_back(&blob[i]);
			if (pos.x + radius > 0)
				quadCollision->particles[1]->push_back(&blob[i]);
		}
		if (pos.y - radius < 0)
		{
			if (pos.x + radius > 0)
				quadCollision->particles[2]->push_back(&blob[i]);
			if (pos.x - radius < 0)
				quadCollision->particles[3]->push_back(&blob[i]);
		}
	}
	
}

// Simply draws the Quad grid on screen to help visualise the the screen segmentation
void BlobDemo::drawGrid()
{
	if (currentState == QUAD)
	{
		glLineWidth(1);
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex2f(0, -nRange);
		glVertex2f(0, nRange);
		glVertex2f(-nRange, 0);
		glVertex2f(nRange, 0);
		glEnd();
	}
}


BlobDemo::~BlobDemo()
{
    delete blob;
	delete platforms;
	delete boarders;

	delete blobCollision;
}

void BlobDemo::display()
{
  Application::display();

	glBegin(GL_LINES);
	glColor3f(0, 1, 1);
	for (unsigned i = 0; i < BAORDER_COUNT; i++) {
		  const Vector2 &p0 = boarders[i].start;
		  const Vector2 &p1 = boarders[i].end;
		  glVertex2f(p0.x, p0.y);
		  glVertex2f(p1.x, p1.y);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0, 1, 1);
	for (unsigned i = 0; i < PLAT_COUNT; i++) {
		  const Vector2 &p0 = platforms[i].start;
		  const Vector2 &p1 = platforms[i].end;
		  glVertex2f(p0.x, p0.y);
		  glVertex2f(p1.x, p1.y);
	}
	glEnd();
	
	for (unsigned i = 0; i < BLOB_COUNT; i++) {
		glColor3f((i%2)?0:1,(i%3)?0:1, (i%4)?1:0); // Absolute beautiful code for setting colours (I love it, Jake hates it)
		const Vector2 &p = blob[i].getPosition();
		glPushMatrix();
		glTranslatef(p.x, p.y, 0);
		glutSolidSphere(blob[i].getRadius(), 12, 12);
		glPopMatrix();
	}

	// Checks to see if show lines is toggled on, if so then runs appropriate function depending on current state
	if (showLines)
	{
		if (currentState == BRUTE_FORCE)
			blobCollision->drawLines();
		if (currentState == QUAD)
			quadCollision->drawLines();
		if (currentState == QUAD_TREE)
			QuadTree->drawLines();
	}

	// Checks to see if show grid is toggled on, if so then runs appropriate function depending on current state
	if (showGrid && currentState == QUAD)
		drawGrid();
	
	if (showGrid && currentState == QUAD_TREE)
		QuadTree->drawTree();

	glutSwapBuffers();
	
    
}

void BlobDemo::update()
{
    // Recenter the axes
	float duration = timeinterval/1000;
    // Run the simulation
    world.runPhysics(duration);

	// On each update runs the listGrid function to check particle possitions and place in appropriate vector when in the Quad state
	if (currentState == QUAD)
		listGrid();

	// Runs the QuadTree update function if the quad tree state is active
	if (currentState == QUAD_TREE)
		QuadTree->update();

    Application::update();
}

const char* BlobDemo::getTitle()
{
    return "Blob Demo";
}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new BlobDemo();
}