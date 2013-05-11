/* 
 * File:   KDTree.h
 * Author: Daniel Princ
 *
 */

#ifndef KDTREE_H
#define	KDTREE_H

#include <vector>
#include <limits>
#include <math.h>

using namespace std;
#include "Point.h"
#include "KDTreeNodes.h"

/**
 * kd-tree!
 */
template<const int D = 3>
class KDTree {
    
    typedef vector< const Point<D>* > points;
    typedef typename vector< const Point<D>* >::const_iterator points_it;
    
    const static int bucketSize = 8;
    
    Inner * root;
    
    void construct(const points * data, float * bounds, Inner* parent)  {
	
	int dim = -1; //dimension to split
	float size = 0;
	for(int i = 0; i < D; i++) {
	    if(bounds[2*i + 1] - bounds[2*i] > size) {
		size = bounds[2*i + 1] - bounds[2*i];
		dim = i;
	    }
	}
	float split = bounds[2*dim] + size / 2.0f; //split value
		
	points left, right; //TODO: is this local?
	float lmax = 0, rmin = split * bounds[2*dim + 1];
	
	for(points_it it = data->begin(); it != data->end(); ++it) {
	    const Point<D> p = *(*it);
	    if(p[dim] <= split) { //NOTE: points exactly on split line belong to left node!
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
	//sliding midpoint split
	if(right.size() == 0)
	    split = lmax;
	if(left.size() == 0)
	    split = rmin;
	
	//set split to node
	parent->dimension = dim;
	parent->split = split;
	
	//create nodes
	if(left.size() > 0) {
	    if(left.size() > bucketSize) {
		Inner *node = new Inner(parent);
		parent->left = node;
		
		float b[2*D];
		std::copy(bounds, bounds + 2*D, &b[0]);
		b[2*dim + 1] = split;
		construct(&left, &b[0], node);
	    }
	    else {
		Leaf<D> * leaf = new Leaf<D>(parent, left);
		parent->left = leaf;
	    }
	}
	
	if(right.size() > 0) {
	    if(right.size() > bucketSize) {
		Inner *node = new Inner(parent);
		parent->right = node;
		
		float b[2*D];
		std::copy(bounds, bounds + 2*D, &b[0]);
		b[2*dim] = split;
		construct(&right, &b[0], node);
	    }
	    else {
		Leaf<D> * leaf = new Leaf<D>(parent, right);
		parent->right = leaf;
	    }
	}
    
    }
    
    /**
     * Find in which bucket does given point belong
     * @param point point in question
     * @return bucket in which the point belongs
     */
    Leaf<D> * findBucket(const Point<D> *point) {
	Inner* node = root;
	while(true) {
	    if((*point)[node->dimension] <= node->split) {
		if(node->left->isLeaf())
		    return (Leaf<D> *) node->left;
		node = (Inner *) node->left;
	    }
	    else {
		if(node->right->isLeaf())
		    return (Leaf<D> *) node->right;
		node = (Inner *) node->right;
	    }
	}
    }
    
    
    const float distance(const Point<D> * p1, const Point<D> * p2) const {
	float dist = 0;
	for(int d = 0; d < D; d++) {
	    static float tmp = abs(p1[d] - p2[d]);
	    dist += tmp*tmp;
	}
	return sqrt(dist);
    }
    
    
public:

    /**
     * Creates empty kd-tree
     */
    KDTree() {
	root = new Inner(NULL);
    }
    ~KDTree() {
	delete root;
    }
    
    /**
     *  Builds the KD-Tree on a given set of points
     * 
     * @param[in] data vector of pointers to points
     * @param[in] dataSize size od given array
     * @param[in] bounds array with bounds of the coordinates \
     *		  expects array like this - 2D: [xmin, xmax, ymin, ymax]
     */
    void construct(const points * data, float * bounds) {
	construct(data, bounds, root);
    }
    
    /**
     *  Builds the KD-Tree on a given set of points
     * 
     * @param[in] data vector of  points
     * @param[in] dataSize size od given array
     * @param[in] bounds array with bounds of the coordinates \
     *		  expects array like this - 2D: [xmin, xmax, ymin, ymax]
     */
    void construct(const vector< Point<D> > * data, float * bounds) {
	vector< const Point<D>* > pointers;
	for(typename vector< Point<D> >::const_iterator it = data->begin(); it != data->end(); ++it) {
	    pointers.push_back(&(*it));
	}
	construct(&pointers, bounds);
    }
    
    /**
     * Returns the root of the tree
     * @return pointer to the root node
     */
    const Inner *getRoot() const {
	return root;
    }
    
    enum Status { RightVisited, LeftVisited, AllVisited};
    
    
    
    
    /**
     * Returns the exact nearest neighbor (NN).
     * If there are more NNs, method retuns one random.
     * @param query the point whose NN we search
     * @return nearest neigbor
     */
    const Point<D> * nearestNeighbor(const Point<D> *query) const {
	Leaf<D> *leaf = findBucket(query);
	float distance = numeric_limits<float>::max();
	Point<D> * nearest;
	
	for(points_it it = leaf->bucket.begin(); it != leaf->bucket.end(); ++it) {
	    float tmp = distance(query, *it);
	    if(tmp < distance && distance > 0) { //ie points are not the same!
		distance = tmp;
		nearest = *it;
	    }
	}
	
	float window[2*D];
	for(int d = 0; d < D; d++) {
	    window[2*d] = query[d] - distance;
	    window[2*d + 1] = query[d] + distance;
	}
	
	Status status;
	if((Leaf<D> *)leaf->parent->left == leaf) {
	    status = LeftVisited;
	}
	else {
	    status = RightVisited;
	}
	
	Inner * node = leaf->parent;
	
	if(status != RightVisited) {
	    if(window[2*D] < node->split) {
		if(node->right->isLeaf()) {
		    for(points_it it = node->right->bucket.begin(); it != node->right->bucket.end(); ++it) {
			float tmp = distance(query, *it);
			if(tmp < distance && distance > 0) { //ie points are not the same!
			    distance = tmp;
			    nearest = *it;
			    window[2*node->dimension] = query[node->dimension] - distance;
			    window[2*node->dimension + 1] = query[node->dimension] + distance;
			}
		    }
		}
		else {
		    //Not leaf
		}
	    }
	}
	
	if(status != LeftVisited) {
	    if(window[2*D + 1] >= node->split) {
	    
	    
	    
	    }
	}
	
	
    
    }
    
    
    /**
     * Inserts point into the tree
     * @param point point to insert
     */
    void insert(const Point<D> *point) { //TODO: insert to empty tree
	Leaf<D> * leaf = findBucket(point);
	if(leaf->bucket.size() < bucketSize) {
	    leaf->bucket.push_back(point);
	    return; //OK, bucket is not full yet
	}
	else { //split the bucket into 2 new leaves
	    points data(leaf->bucket);
	    data.push_back(point); //add the point to bucket
	    //create new inner node
	    Inner * node = new Inner(leaf->parent);
	    if((Leaf<D> *)leaf->parent->left == leaf) {
		leaf->parent->left = node;
	    }
	    else if((Leaf<D> *)leaf->parent->right == leaf) {
		leaf->parent->right = node;
	    }
	    else {
		cerr << "somethig is very wrong! Point not inserted.\n";
		return;
	    }
	    delete leaf; //no longer necessary
	    
	    //split the nodes along the dimension with greatest local variance
	    Point<D> min = *(data[0]);
	    Point<D> max = *(data[0]);
	    for(points_it it = data.begin(); it != data.end(); ++it) {
		Point<D> p = *(*it);
		for(int d = 0; d < D; d++) {
		    if(p[d] < min[d]) {
			min[d] = p[d];
			continue;
		    }
		    if(p[d] > max[d]) {
			max[d] = p[d];
		    }
		}
	    }
	    int dim;
	    float dist = 0;
	    for(int d = 0; d < D; d++) {
		if(max[d] - min[d] > dist) {
		    dist = max[d] - min[d];
		    dim = d;
		}
	    }
	    node->split = min[dim] + dist / 2.0f;
	    
	    points l, r; //split the data
	    for(points_it it = data.begin(); it != data.end(); ++it) {
		Point<D> p = *(*it);
		if(p[dim] <= node->split) {
		    l.push_back(*it);
		}
		if(p[dim] > node->split) {
		    r.push_back(*it);
		}
	    }
	    
	    //create two new leaves
	    Leaf<D> * left = new Leaf<D>(node, l);
	    node->left = left;
	    
	    Leaf<D> * right = new Leaf<D>(node, r);
	    node->right = right;
	    
	}
    }
    
};

#endif	/* KDTREE_H */
