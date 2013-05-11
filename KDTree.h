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
#include "PlyHandler.h"

/**
 * kd-tree!
 */
template<const int D = 3>
class KDTree {
    
    typedef vector< Point<D> *> points;
    typedef typename vector< Point<D> *>::iterator points_it;
    
    const static int bucketSize = 8;
    
    Inner * root;
    
    void construct( points * data, float * bounds, Inner* parent)  {
	
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
	    Point<D> p = *(*it);
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
    Leaf<D> * findBucket(const Point<D> *point) const {
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
    
    
    const float distance(const Point<D> * p1, const Point<D> * p2) {
	float dist = 0;
	for(int d = 0; d < D; d++) {
	    float tmp = abs((*p1)[d] - (*p2)[d]);
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
    void construct(points * data, float * bounds) {
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
    void construct(vector< Point<D> > * data, float * bounds) {
	vector< Point<D>* > pointers;
	for(typename vector< Point<D> >::iterator it = data->begin(); it != data->end(); ++it) {
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
    
    enum Status { RightVisited, LeftVisited, AllVisited, NoneVisited};
    
    float dist;
    Point<D> * nearest;
    float *window;
    void search(const Inner * node, Status status, const Point<D> *query) {
    
	if(node->right && (status != RightVisited || status == NoneVisited)) {
	    if(window[2*node->dimension + 1] > node->split) {
		if(node->right->isLeaf()) {
		    for(points_it it = ((Leaf<D> *)node->right)->bucket.begin(); it != ((Leaf<D> *)node->right)->bucket.end(); ++it) {
			(*it)->setColor(0, 255, 255);
			float tmp = distance(query, *it);
			if(tmp < dist && tmp > 0) { //ie points are not the same!
			    dist = tmp;
			    nearest = *it;
			    for(int d = 0; d < D; d++) {
				window[2*d] = (*query)[d] - dist;
				window[2*d + 1] = (*query)[d] + dist;
			    }
			}
		    }
		}
		else {
		    //Not leaf
		    search((Inner *) node->right, NoneVisited,  query);
		}
	    }
	    else {
		if(node->right->isLeaf())
		    for(points_it it = ((Leaf<D> *)node->right)->bucket.begin(); it != ((Leaf<D> *)node->right)->bucket.end(); ++it) {
			(*it)->setColor(0, 128, 128);
		    }
	    }
	}
	
	if(node->left && (status != LeftVisited || status == NoneVisited)) {
	    if(window[2*node->dimension] <= node->split) {
		if(node->left->isLeaf()) {
		    for(points_it it = ((Leaf<D> *)node->left)->bucket.begin(); it != ((Leaf<D> *)node->left)->bucket.end(); ++it) {
			(*it)->setColor(255, 255, 0);
			float tmp = distance(query, *it);
			if(tmp < dist && tmp > 0) { //ie points are not the same!
			    dist = tmp;
			    nearest = *it;
			    for(int d = 0; d < D; d++) {
				window[2*d] = (*query)[d] - dist;
				window[2*d + 1] = (*query)[d] + dist;
			    }
			}
		    }
		}
		else {
		    //Not leaf
		    search((Inner *) node->left, NoneVisited,  query);
		}
	    }
	    else {
		if(node->left->isLeaf())
		    for(points_it it = ((Leaf<D> *)node->left)->bucket.begin(); it != ((Leaf<D> *)node->left)->bucket.end(); ++it) {
			(*it)->setColor(128, 128, 0);
		    }
	    }
	}
	//on my way down || in root
	if(status == NoneVisited || !node->parent) 
	    return;
	
	if((Inner *) node->parent->right == node)
	    search(node->parent, RightVisited,  query);
	else
	    search(node->parent, LeftVisited, query);
	    
    
    }
    
    
    
    /**
     * Returns the exact nearest neighbor (NN).
     * If there are more NNs, method retuns one random.
     * @param query the point whose NN we search
     * @return nearest neigbor
     */
    Point<D> * nearestNeighbor(const Point<D> *query) {
	Leaf<D> *leaf = findBucket(query);
	dist = numeric_limits<float>::max();
	//Point<D> * nearest;
	
	for(points_it it = leaf->bucket.begin(); it != leaf->bucket.end(); ++it) {
	    (*it)->setColor(0, 255, 0);
	    float tmp = distance(query, *it);
	    if(tmp < dist && tmp > 0) { //ie points are not the same!
		dist = tmp;
		nearest = *it;
	    }
	}
	
	float win[2*D];
	for(int d = 0; d < D; d++) {
	    win[2*d] = (*query)[d] - dist;
	    win[2*d + 1] = (*query)[d] + dist;
	}
	window = &win[0];
	
	//cout << window[0] << " " << window[1]<< " " << window[2]<< " " << window[3] << "\n";
	//PlyHandler::saveWindow<D>(window);
	
	Status status;
	if((Leaf<D> *)leaf->parent->left == leaf) {
	    status = LeftVisited;
	}
	else {
	    status = RightVisited;
	}
	
	if((Leaf<D> *) leaf->parent->right == leaf)
	    search(leaf->parent, RightVisited,  query);
	else 
	    search(leaf->parent, LeftVisited, query);
	
	return nearest;
    }
    
    
    /**
     * Inserts point into the tree
     * @param point point to insert
     */
    void insert(Point<D> *point) { //TODO: insert to empty tree
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
