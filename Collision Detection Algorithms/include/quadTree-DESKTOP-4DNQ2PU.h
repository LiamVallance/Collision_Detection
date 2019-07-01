#pragma once
#include <gl/glut.h>
#include <vector>
#include <particle.h>
#include <iostream>
using namespace std;

class Quadtree {
public:
	Quadtree(float x, float y, float width, float height);
	~Quadtree();
	void PreOrder(Quadtree* quad);
	bool insert(Particle* particles);
	vector<vector<Particle*>> getObjects(vector<vector<Particle*>>);
	void showGrid();

private:
	float x;
	float y;
	float width;
	float height;
	int capacity = 2;
	bool divided = false;
	void divide();
	vector<Particle*>objects;
	Particle* pGrid;
	Quadtree *NW = nullptr;
	Quadtree *NE = nullptr;
	Quadtree *SW = nullptr;
	Quadtree *SE = nullptr;
	bool contains(Particle *object);
};