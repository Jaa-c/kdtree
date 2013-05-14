/* 
 * File:   KDTreeNodes.h
 * Author: Dan Princ
 * 
 * Contains various structures used in the KDTree
 *
 */

#ifndef KDTREENODES_H
#define	KDTREENODES_H

#include <cstdlib>
#include <vector>
#include <limits>
#include <math.h>


struct Inner;

/**
 * Parent of nodes, can't be instantiated
 *  
 */
struct Node {
    
    /** Pointer to parent node */
    Inner * parent;
    
    /** Returns true if node is leaf */
    const bool isLeaf() const {
	return leaf;
    }
    
    virtual ~Node() {}
    
protected:
    /** Constructor for child classes */
    Node(const bool leaf, Inner* parent) : leaf(leaf), parent(parent) {}

private:
    const bool leaf;
};

/**
 * Inner node in the KDTree, 
 * has pointers to children and
 * information about dimension split
 * 
 */
struct Inner : Node {
    /** index of dimension that is split by this node */
    unsigned int dimension;
    /** value where the dimesion is split*/
    float split;
    
    Node* left;
    Node* right;
    
    Inner(Inner *parent) : Node(false, parent), left(NULL), right(NULL) {}
    ~Inner() {
	if(left) delete left;
	if(right) delete right;
    }
};

/**
 * Leaf (bucket) in the tree.
 * Contains only list of pointers to points
 * 
 */
template<const int D = 3>
struct Leaf : Node {
    std::vector< Point<D> * > bucket;
    /** lower bound, BOB test */
    float min[D]; 
    /** upper bound, BOB test */
    float max[D]; 
    
    Leaf(Inner *parent, std::vector< Point<D>* > bucket) : Node(true, parent), bucket(bucket) {
	for(int d = 0; d < D; d++) {
	    min[d] = std::numeric_limits<float>::max();
	    max[d] = 0;
	}
	for(typename std::vector< Point<D> * >::iterator it = bucket.begin(); it!= bucket.end(); ++it) {
	    Point<D> * p = *it;
	    for(int d = 0; d < D; d++) {
		if((*p)[d] > max[d]) max[d] = (*p)[d];
		if((*p)[d] < min[d]) min[d] = (*p)[d];
	    }
	}
    }
    ~Leaf() {}
    
    void add(Point<D> * p) {
	bucket.push_back(p);
	for(int d = 0; d < D; d++) {
	    if((*p)[d] > max[d]) max[d] = (*p)[d];
	    if((*p)[d] < min[d]) min[d] = (*p)[d];
	}
    }
    
    const float getDensity() const {
	float vol = 1;
	for(int d = 0; d < D; d++) {
	    vol *= max[d] - min[d];
	}
	return vol / (float)bucket.size();
    }

};

/**
 * Object that keeps track of the distance coverd
 * from NN query.
 */
template<const int D = 3>
struct TrackingNode {
private:
    float tracker[D];
    float length;
    
public:
    /**
     * Constructior
     */
    TrackingNode() {
	for(int d = 0; d < D; d++) {
	    tracker[d] = 0;
	}
	length = 0;
    }
    ~TrackingNode() {}
    
    //read only
    const float& operator[](int idx) const {
	return tracker[idx];
    }
    
    /**
     * Sets value in given dimension to given length
     * @param d dimension
     * @param val length
     */
    void set(int d, float val) {
	length -= tracker[d];
	tracker[d] = val*val;
	length += tracker[d];
    }
    
    /**
     * Similar to set, except it only returns length and doesn't
     * modify the object. It'm using it to avoid creatin new objects
     * on stack if it's not necessary.
     * @param d dimension
     * @param val value to change
     * @return squared length
     */
    float getUpdatedLength(int d, float val) const {
	float temp =  length - tracker[d];
	temp += val * val;
	return temp;
    }
  
    /**
     * Returns the length from the query
     * @return length
     */
    float getLength() const {
	return sqrt(length);
    }
    
    /**
     * Returns the sqared length from the query
     * @return length square
     */
    float getLengthSquare() const {
	return length;
    }

};

/** Status of nodes during NN search */
enum Visited {
    /** right child has been visited*/
    RIGHT, 
    /** left child has been visited*/
    LEFT, 
    /** both children have been visited*/
    BOTH, 
    /** none child has been visited*/
    NONE
};

/**
 * Structure on the stack for NN search
 */
template<const int D = 3>
struct ExtendedNode {
    Inner * node;
    Visited status;
    TrackingNode<D> tn;
    
    ExtendedNode() : node(NULL) {}
    ExtendedNode(Inner * node) : node(node) {}
};

/**
 * Structure on the stack tree construction
 */
template<const int D = 3>
struct Constr {
    std::vector< Point<D> *> data;
    float bounds[2*D];
    Inner *parent;

    Constr(std::vector< Point<D> *> data, float * bounds, Inner *parent) 
	    : data(data), parent(parent) {

	std::copy(bounds, bounds + 2*D, &this->bounds[0]);
    }
};

#endif	/* KDTREENODES_H */

