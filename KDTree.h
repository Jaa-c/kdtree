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
#include "KDTreeNodes.h"

template<const int D = 3>
class KDTree {
    
    typedef vector< const Point<D>* > points;
    typedef typename vector< const Point<D>* >::const_iterator points_it;
    
    const static int bucketSize = 8;
    
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
     * Returns the root of the tree
     * @return pointer to the root node
     */
    const Inner *getRoot() const {
	return &root;
    }
    
};

#endif	/* KDTREE_H */
