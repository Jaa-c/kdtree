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

/**
 * Structure in the stack for NN search
 */
template<const int D = 3>
struct ExtendedInner {
    Inner * node;
    //Status status;
    float bounds[2*D]; 



};

#endif	/* KDTREENODES_H */

