/* 
 * File:   KDTreeNodes.h
 * Author: jaa
 *
 * Created on 10. květen 2013, 2:14
 */

#ifndef KDTREENODES_H
#define	KDTREENODES_H

#include <cstdlib>
#include <vector>


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
    std::vector< Point<D>* > bucket;
    
    Leaf(Inner *parent, std::vector< Point<D>* > bucket) : Node(true, parent), bucket(bucket) {}
    ~Leaf() {}
};

template<const int D = 3>
struct TrackingNode {
private:
    float tracker[D];
    float length;
    
public:
    
    TrackingNode(const TrackingNode<D> &tn) {
	std::copy(tn.getTracker(), tn.getTracker() + D, tracker);
	length = tn.getLength();
    }
    
    TrackingNode() {
	for(int d = 0; d < D; d++) {
	    tracker[d] = 0;
	}
	length = 0;
    }
    ~TrackingNode() {}
    
//    float& operator[](int idx) {
//	return tracker[idx];
//    }
    //read only
    const float& operator[](int idx) const {
	return tracker[idx];
    }
    
    const float* getTracker() const {
	return &tracker[0];
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
    
    float getLength() const {
	return sqrt(length);
    }

};


/** Status of nodes during NN search */
enum Visited {
    RIGHT, LEFT, BOTH, NONE
};

/**
 * Structure in the stack for NN search
 */
template<const int D = 3>
struct ExtendedNode {
    Inner * node;
    Visited status;
    TrackingNode<D> tn;
    
    ExtendedNode() : node(NULL) {}
    ExtendedNode(TrackingNode<D> tn) : node(NULL), tn(tn) {}
    ExtendedNode(Inner * node) : node(node) {}
    ExtendedNode(Inner * node, TrackingNode<D> tn) : node(node), tn(tn) {}
};

#endif	/* KDTREENODES_H */

