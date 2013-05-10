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

/**
 * Parent of nodes, can't be instantiated
 *  
 */
struct Node {
    
    /** Returns true if node is leaf */
    const bool isLeaf() const {
	return leaf;
    }
    
protected:
    /** Constructor for child classes */
    Node(const bool leaf) : leaf(leaf) {}

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
    unsigned char dimension;
    /** value where the dimesion is split*/
    float split;
    
    Node* left;
    Node* right;
    
    Inner() : Node(false), left(NULL), right(NULL) {}
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
    std::vector< const Point<D>* > bucket;
    
    Leaf(std::vector< const Point<D>* > bucket) : Node(true), bucket(bucket) {}
    ~Leaf() {}
};


#endif	/* KDTREENODES_H */

