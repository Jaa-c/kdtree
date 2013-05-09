/* 
 * File:   KDTree.h
 * Author: Daniel Princ
 *
 */

#ifndef KDTREE_H
#define	KDTREE_H

#include <vector>

using namespace std;
#include "Point.h"

struct Node {
    bool leaf;
    bool isLeaf() {
	return leaf;
    }
protected:
    Node(bool leaf) : leaf(leaf) {}
};

struct Inner : Node {
    unsigned char dimension;
    float split;
    Node* left;
    Node* right;
    
    Inner() : Node(false), left(NULL), right(NULL) {}
    ~Inner() {
	if(left) delete left;
	if(right) delete right;
    }
};

template<const int D = 3>
struct Leaf : Node {
    vector< const Point<D>* > bucket;
    
    Leaf(vector< const Point<D>* > bucket) : Node(true), bucket(bucket) {}
    ~Leaf() {}
};

template<const int D = 3>
class KDTree {
    
    typedef vector< const Point<D>* > points;
    typedef typename vector< const Point<D>* >::const_iterator points_it;
    
    const static int bucketSize = 16;
    
    Inner root;
    
    void construct(const points * data, float * bounds, Inner* parent)  {
	
	int dim = -1;
	float size = 0;
	for(int i = 0; i < D; i++) {
	    if(bounds[2*i + 1] - bounds[2*i] > size) {
		size = bounds[2*i + 1] - bounds[2*i];
		dim = i;
	    }
	}
	float split = bounds[2*dim] + size / 2.0f;
		
	points left, right;
	float lmax = 0, rmin = split * bounds[2*dim + 1];
	
	for(points_it it = data->begin(); it != data->end(); ++it) {
	    const Point<D> p = *(*it);
	    if(p[dim] <= split) {
		left.push_back(*it);
		if(p[dim] > lmax)
		    lmax = p[dim];
	    }
	    if(p[dim] > split) {
		right.push_back(*it);
		if(p[dim] < rmin)
		    rmin = p[dim];
	    }
	}
	
	if(right.size() == 0)
	    split = lmax;
	if(left.size() == 0)
	    split = rmin;
	
	
	parent->dimension = dim;
	parent->split = split;
	
	if(left.size() > 0) {
	    if(left.size() > bucketSize) {
		Inner *node = new Inner();
		parent->left = node;
		
		float b[2*D];
		std::copy(bounds, bounds + 2*D, &b[0]);
		b[2*dim + 1] = split;
		construct(&left, &b[0], node);
		
	    }
	    else {
		Leaf<D> * leaf = new Leaf<D>(left);
		parent->left = leaf;
	    }
	}
	
	if(right.size() > 0) {
	    if(right.size() > bucketSize) {
		Inner *node = new Inner();
		parent->right = node;
		
		float b[2*D];
		std::copy(bounds, bounds + 2*D, &b[0]);
		b[2*dim] = split;
		construct(&right, &b[0], node);
	    }
	    else {
		Leaf<D> * leaf = new Leaf<D>(right);
		parent->right = leaf;
	    }
	}
    
    }
    
public:
	
    KDTree() {}
    ~KDTree() {
    }
    
    /**
     *  Builds the KD-Tree on a given set of points
     * 
     * @param[in] data array of points
     * @param[in] dataSize size od given array
     * @param[in] bounds array with bounds of the coordinates \
     *		  expects array like this - 2D: [xmin, xmax, ymin, ymax]
     */
    void construct(const points * data, float * bounds) {
	
	construct(data, bounds, &root);
	
    }
    
    
    
    
    
    
    
    /**
     * Saves points, every bucket in different color
     * makes sense only in 2D
     */
    void debugTree() {
	if(D != 2) return;
	srand((unsigned)std::time(0)); 
	vector< Point<D> > data = debugBuckets(&root);
	PlyHandler::savePoints<D>("data/debug.ply", data);
	
	float bounds[2*D] = {0.f, 1.f, 0.f, 2.f};
	data = debugTree(&root, bounds);
	PlyHandler::saveLines<D>("data/lines.ply", data);
    }
    vector< Point<D> > debugBuckets(const Inner* node) {
	vector< Point<D> > data;
	if(node->left) {
	    
	    if(!node->left->isLeaf()) {
		vector< Point<D> > d = debugBuckets((Inner *) node->left);
		data.insert(data.end(), d.begin(), d.end());
	    }
	    else {
		Leaf<D> * l = (Leaf<D> *) node->left;
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		for(points_it it = l->bucket.begin(); it != l->bucket.end(); ++it) {
		    Point<D> p;
		    p.coords[0] = (*it)->coords[0];
		    p.coords[1] = (*it)->coords[1];
		    p.color[0] = r;
		    p.color[1] = g;
		    p.color[2] = b;
		    data.push_back(p);
		}
	    }
	}
	if(node->right) {
	    if(!node->right->isLeaf()) {
		vector< Point<D> > d = debugBuckets((Inner *) node->right);
		data.insert(data.end(), d.begin(), d.end());
	    }
	    else {
		Leaf<D> * l = (Leaf<D> *) node->right;
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		for(points_it it = l->bucket.begin(); it != l->bucket.end(); ++it) {
		    Point<D> p;
		    p.coords[0] = (*it)->coords[0];
		    p.coords[1] = (*it)->coords[1];
		    p.color[0] = r;
		    p.color[1] = g;
		    p.color[2] = b;
		    data.push_back(p);
		}
	    }
	}
	return data;
    }
    vector< Point<D> > debugTree(const Inner* node, float* bound) {
	vector< Point<D> > data;
	Point<D> p1;
	p1[0] = bound[0];
	p1[1] = bound[2];
	p1[node->dimension] = node->split;
	p1.color[0] = 255;
	p1.color[1] = 255;
	p1.color[2] = 255;
	
	Point<D> p2;
	p2[0] = bound[1];
	p2[1] = bound[3];
	p2[node->dimension] = node->split;
	p2.color[0] = 255;
	p2.color[1] = 255;
	p2.color[2] = 255;
	
	data.push_back(p1);
	data.push_back(p2);
	if(!node->left->isLeaf()) {
	    int temp = bound[2*node->dimension + 1];
	    bound[2*node->dimension + 1] = node->split;
	    vector< Point<D> > d = debugTree((Inner *) node->left, bound);
	    data.insert(data.end(), d.begin(), d.end());
	    bound[2*node->dimension + 1] = temp;
	}
	
	if(!node->right->isLeaf()) {
	    bound[2*node->dimension] = node->split;
	    vector< Point<D> > d = debugTree((Inner *) node->right, bound);
	    data.insert(data.end(), d.begin(), d.end());
	}
	return data;
    }

};


#endif	/* KDTREE_H */
