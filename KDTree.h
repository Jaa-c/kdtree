/* 
 * File:   KDTree.h
 * Author: jaa
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
		bounds[2*dim + 1] = split;
		construct(&left, bounds, node);
		bounds[2*dim + 1] += size/2.0f;
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
		bounds[2*dim] = split;
		construct(&right, bounds, node);
		bounds[2*dim] += size/2.0f;
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
	if(root.left) delete root.left;
	if(root.right) delete root.right;
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
	srand((unsigned)std::time(0)); 
	vector< Point<D> > data = debugTree(&root);
	PlyHandler::save<D>("data/debug.ply", data);
    }
    vector< Point<D> > debugTree(Inner* node) {
	vector< Point<D> > data;
	if(node->left) {
	    
	    if(!node->left->isLeaf()) {
		vector< Point<D> > d = debugTree((Inner *) node->left);
		data.insert(data.end(), d.begin(), d.end());
	    }
	    else {
		Leaf<D> * l = (Leaf<D> *) node->left;
		int r = 0;//rand() % 255;
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
		vector< Point<D> > d = debugTree((Inner *) node->right);
		data.insert(data.end(), d.begin(), d.end());
	    }
	    else {
		Leaf<D> * l = (Leaf<D> *) node->right;
		int r = rand() % 255;
		int g = rand() % 255;
		int b = 0;//rand() % 255;
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

};


#endif	/* KDTREE_H */

