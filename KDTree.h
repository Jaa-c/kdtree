/* 
 * File:   KDTree.h
 * Author: Daniel Princ
 *
 */

#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <stack>
#include <queue>
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
    typedef pair<Inner *, Visited> exInner; //DEPRECATED
    
    /** Size of the bucket*/
    const static int bucketSize = 8;
    
    /** root of the tree */
    Inner * root;
    
    /** number of points inside the tree */
    int size;
    
    /** bounding box of the tree, format: xmin, xmax, ymin, ymax, ...*/
    float boundingBox[2*D];
    
    struct Constr {
	points data;
	float bounds[2*D];
	Inner *parent;
	
	Constr(points data, float * bounds, Inner *parent) 
		: data(data), parent(parent) {
	    
	    std::copy(bounds, bounds + 2*D, &this->bounds[0]);
	}
    };
    
    
    /**
     * Recursive construction of the tree
     * @param data Set on unordered points
     * @param bounds current bounds in the tree
     * @param parent parent node
     */
    void construct(points * adata, float * abounds, Inner* aparent)  {
	
	stack<Constr> stack;
	stack.push(Constr(*adata, abounds, aparent));
	
	while(!stack.empty()) {
	    	    
	    Constr curr = stack.top();
	    stack.pop();
	    points * data = &curr.data;
	    float* bounds = &curr.bounds[0];
	    Inner *parent = curr.parent;
	
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
	    float lmax = 0, rmin = numeric_limits<float>::max();

	    for(points_it it = data->begin(); it != data->end(); ++it) {
		Point<D> *p = (*it);
		if((*p)[dim] <= split) { //NOTE: points exactly on split line belong to left node!
		    left.push_back(*it);
		    if((*p)[dim] > lmax)
			lmax = (*p)[dim];
		}
		if((*p)[dim] > split) {
		    right.push_back(*it);
		    if((*p)[dim] < rmin)
			rmin = (*p)[dim];
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
		    //construct(&left, &b[0], node);
		    stack.push(Constr(left, &b[0], node));

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
		    //construct(&right, &b[0], node);
		    stack.push(Constr(right, &b[0], node));
		}
		else {
		    Leaf<D> * leaf = new Leaf<D>(parent, right);
		    parent->right = leaf;
		}
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
		if(!node->left) {
		    if(node->right->isLeaf())
			return (Leaf<D> *) node->right;
		    else {
			node = (Inner *) node->right;
			continue;
		    }
		}
		if(node->left->isLeaf())
		    return (Leaf<D> *) node->left;
		node = (Inner *) node->left;
	    }
	    else {
		if(!node->right) {
		    if(node->left->isLeaf())
			return (Leaf<D> *) node->left;
		    else {
			node = (Inner *) node->left;
			continue;
		    }
		}
		if(node->right->isLeaf())
		    return (Leaf<D> *) node->right;
		node = (Inner *) node->right;
	    }
	}
    }
    
    /**
     * Calculates distance
     * @param p1
     * @param p2
     * @param sqrtb if false, the returned distance is squared
     * @return 
     */
    inline const float distance(const Point<D> * p1, const Point<D> * p2, bool sqrtb = false) {
	float dist = 0;
	for(int d = 0; d < D; d++) {
	    float tmp = fabs((*p1)[d] - (*p2)[d]);
	    dist += tmp*tmp;
	}
	if(sqrtb)
	    return sqrt(dist);
	else
	    return dist;
    }
    
    /**
     * Gets distance from given point to defined hyper rectangle
     * @param point query point
     * @param min min coords of a hyper reectangle
     * @param max max coords of a hyper reectangle
     * @return squared distance
     */
    inline const float minBoundsDistance(const Point<D> * point, const float * min, const float * max) {
	float dist = 0;
	for(int d = 0; d < D; d++) {
	    if ((*point)[d] < min[d]) {
		const float tmp = min[d] - (*point)[d];
		dist += tmp*tmp;
	    } 
	    else if ((*point)[d] > max[d]) {
		const float tmp = max[d] - (*point)[d];
		dist += tmp*tmp;
	    }
	}
	return dist;
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
     * @param[in] data vector of pointers to unordered points
     * @param[in] bounds array with bounds of the coordinates \
     *		  expects array like this - 2D: [xmin, xmax, ymin, ymax]
     */
    void construct(points * data, float * bounds) {
	copy(bounds, bounds + 2*D, boundingBox);
	size = data->size();
	construct(data, bounds, root);
    }
    
    /**
     *  Builds the KD-Tree on a given set of points with unknown bounds
     * 
     * @param[in] data vector of pointers to unordered points
     */
    void construct(points * data) {
	float bounds[2*D];
	for(int d = 0; d < D; d++) {
	    bounds[2*d] = numeric_limits<float>::max();
	    bounds[2*d + 1] = numeric_limits<float>::min();
	}
	
	for(points_it it = data->begin(); it != data->end(); ++it) {
	    Point<D> *p = *it;
	    for(int d = 0; d < D; d++) {
		if((*p)[d] < bounds[2*d]) bounds[2*d] = (*p)[d];
		if((*p)[d] > bounds[2*d + 1]) bounds[2*d + 1] = (*p)[d];
	    }
	}
	
	copy(bounds, bounds + 2*D, boundingBox);
	size = data->size();
	construct(data, &bounds[0], root);
    }
    
    /**
     *  Builds the KD-Tree on a given set of points
     * 
     * @param[in] data vector of unordered points
     * @param[in] bounds array with bounds of the coordinates \
     *		  expects array like this - 2D: [xmin, xmax, ymin, ymax]
     */
    void construct(vector< Point<D> > * data, float * bounds = NULL) {
	vector< Point<D>* > pointers;
	for(typename vector< Point<D> >::iterator it = data->begin(); it != data->end(); ++it) {
	    pointers.push_back(&(*it));
	}
	if(bounds)
	    construct(&pointers, bounds);
	else
	    construct(&pointers);
    }
    
    /**
     * Returns the root of the tree
     * @return pointer to the root node
     */
    const Inner *getRoot() const {
	return root;
    }
    
    /**
     * Bounding box of the tree
     * @return array of size 2D, format: xmin, xmax, ymin, ymax, ...
     */
    const float *getBoundingBox() const {
	return &boundingBox[0];
    }
    
    
    /**
     * Returns exact k-nearest neighbors (kNN).
     * @param query the point whose kNN we search
     * @param k the number of points we look for
     * @return vector op kNN
     */
    vector< Point<D> * > kNearestNeighbor(const Point<D> *query, const int k) {
	Leaf<D> *leaf = findBucket(query);
	
	float dist = 0;
	for(int d = 0; d < D; d++) {
	    float tmp = leaf->max[d] - leaf->min[d];
	    dist += tmp * tmp;
	}
	
	if(dist == 0) { //bucket has only 1 point, so lets just find NN
	    dist = distance(leaf->bucket[0], nearestNeighbor(query));
	}
	
	float r = sqrt(dist) / 2.0f;
	
	int diff = k - leaf->bucket.size();
	if(diff > 0) {
	    float df = pow(diff, 1 / (float) D);
	    r *= df; // works pretty well
	}
	
	vector< Point<D> * > knn;
	
	//TODO: this is certainly not the most efficient solution
	for(int i = 100; i > 1; i--) { 
	     knn = circularQuery(query, r);
	    if(knn.size() > k + 1 || knn.size() == size) {
		break;
	    }
	    else {
		r *= 1 + (1 / (float) D) + (1 / (float) i);
	    }
	}
	
	sort(knn.begin(), knn.end(), 
	    [query, this](const Point<D> * a, const Point<D> * b) -> bool { 
		return distance(a, query) < distance(b, query); 
	    });
	    
	vector< Point<D> * > result;
	
	int size =  (k + 1 < knn.size()) ? k + 1 : knn.size();
	
	result.insert(result.end(), knn.begin() + 1, knn.begin() + size);
	
	return result;
    }
    
    /**
     * Returns all points in a hypersphere around given point
     * 
     * Basicaly similar implementation to NN, except there is a fixed radius
     * 
     * @param query center of the sphere
     * @param radius radius of the sphere
     * @return list of points inside
     */
    vector< Point<D> * > circularQuery(const Point<D> *query, const int radius) {
	Leaf<D> *leaf = findBucket(query);
	vector< Point<D> * > data;
	float r = radius * radius;
	
	//find nearest point in the bucket
	for(points_it it = leaf->bucket.begin(); it != leaf->bucket.end(); ++it) {
	    float tmp = distance(query, *it);
	    if(tmp < r) { //ie points are not the same!
		data.push_back(*it);
	    }
	}
	
	ExtendedNode<D> firstNode(leaf->parent);
	if((Leaf<D> *)leaf->parent->left == leaf)
	    firstNode.status = LEFT;
	else
	    firstNode.status = RIGHT;
	
	stack< ExtendedNode<D> > stack; //avoid recursion
	stack.push(firstNode);
	
	// check possible nodes for NN
	while(!stack.empty()) { 
	    ExtendedNode<D> exNode = stack.top();
	    stack.pop();
	    	    
	    Node * nleft = NULL;
	    Node * nright = NULL;
	    float ldiff, rdiff, ladd, radd;
	    
	    /// if right child exist && it has not been searchd yet
	    if(exNode.node->right && (exNode.status != RIGHT || exNode.status == NONE)) {
		radd = exNode.node->split - (*query)[exNode.node->dimension];
		if(radd > 0) // only if I'm "crossing line from left to right"
		    rdiff = exNode.tn.getUpdatedLength(exNode.node->dimension, radd);
		else
		    rdiff = exNode.tn.getLengthSquare();
		
		if(rdiff < r) { //if there possibly can be nearer point than current nearest
		    nright = exNode.node->right;
		}
	    }
	    /// if left child exist && it has not been searchd yet
	    if(exNode.node->left && (exNode.status != LEFT || exNode.status == NONE)) {
		ladd = (*query)[exNode.node->dimension] - exNode.node->split;
		if(ladd > 0)
		    ldiff = exNode.tn.getUpdatedLength(exNode.node->dimension, ladd);
		else
		    ldiff = exNode.tn.getLengthSquare();
		
		if(ldiff < r) {
		    nleft = exNode.node->left;
		}
	    }
	    
	    //TODO: I should stop if I'm to high in the tree
	    //on my way up && not in root
	    if(exNode.status != NONE && exNode.node->parent) {
		ExtendedNode<D> add(exNode.node->parent);
		add.tn = exNode.tn;
		if((Inner *) exNode.node->parent->right == exNode.node) 
		    add.status = RIGHT;
		else
		    add.status = LEFT;

		stack.push(add);
	    }
	    
	    // this iterates over 2 children
	    // a bit mess, but there are too many variables and it does not look
	    // nice as a method.
	    for(int c = 0; c <= 1; c++) { 
		Node * node;
		float add;
		//path ordering, choose the worse first so
		//the better will be first to pop of the stack
		if(c == 0) { 
		    node = (ldiff >= rdiff) ? nleft : nright;
		    add = (ldiff >= rdiff) ? ladd : radd;
		}
		else { //in second iteration, choose the better (=the other node)
		    node = (ldiff < rdiff) ? nleft : nright;
		    add = (ldiff < rdiff) ? ladd : radd;
		}
		
		if(node) {
		    if(node->isLeaf()) { // if node is leaf we search the bucket
			Leaf<D> * leaf = (Leaf<D> *) node;
			///BOB test
			if(minBoundsDistance(query, leaf->min, leaf->max) < r) {
			    points *bucket = &leaf->bucket;
			    for(points_it it = bucket->begin(); it != bucket->end(); ++it) {
				float tmp = distance(query, *it);
				if(tmp < r) { //ie points are not the same!
				    data.push_back(*it);
				}
			    }
			}
		    }
		    else { //Not leaf, add Node to the stack with correct tracking node
			ExtendedNode<D> newN((Inner *) node);
			newN.tn = exNode.tn;
			if(add > 0)
			    newN.tn.set(exNode.node->dimension, add);
			newN.status = NONE; 
			if(newN.tn.getLengthSquare() < r) { //check if the dist hasn't changed
			    stack.push(newN);
			}
		    }
		}
	    }
	}
    
	return data;
    }
    
    
    /**
     * Returns the exact nearest neighbor (NN).
     * If there are more NNs, method retuns one random.
     * @param query the point whose NN we search
     * @return nearest neigbor
     */
    Point<D> * nearestNeighbor(const Point<D> *query) {
	Leaf<D> *leaf = findBucket(query);
	/** squared distance of the current nearest neigbor */
	float dist = numeric_limits<float>::max();
	/** current best NN */
	Point<D> * nearest;
	
	//find nearest point in the bucket
	for(points_it it = leaf->bucket.begin(); it != leaf->bucket.end(); ++it) {
	    (*it)->setColor(0, 255, 0);
	    float tmp = distance(query, *it);
	    if(tmp < dist && tmp > 0) { //ie points are not the same!
		dist = tmp;
		nearest = *it;
	    }
	}
		
	ExtendedNode<D> firstNode(leaf->parent);
	if((Leaf<D> *)leaf->parent->left == leaf)
	    firstNode.status = LEFT;
	else
	    firstNode.status = RIGHT;
	
	stack< ExtendedNode<D> > stack; //avoid recursion
	stack.push(firstNode);
	
	// check possible nodes for NN
	while(!stack.empty()) { 
	    ExtendedNode<D> exNode = stack.top();
	    stack.pop();
	    	    
	    Node * nleft = NULL;
	    Node * nright = NULL;
	    float ldiff, rdiff, ladd, radd;
	    
	    /// if right child exist && it has not been searchd yet
	    if(exNode.node->right && (exNode.status != RIGHT || exNode.status == NONE)) {
		radd = exNode.node->split - (*query)[exNode.node->dimension];
		if(radd > 0) // only if I'm "crossing line from left to right"
		    rdiff = exNode.tn.getUpdatedLength(exNode.node->dimension, radd);
		else
		    rdiff = exNode.tn.getLengthSquare();
		
		if(rdiff < dist) { //if there possibly can be nearer point than current nearest
		    nright = exNode.node->right;
		}
	    }
	    /// if left child exist && it has not been searchd yet
	    if(exNode.node->left && (exNode.status != LEFT || exNode.status == NONE)) {
		ladd = (*query)[exNode.node->dimension] - exNode.node->split;
		if(ladd > 0)
		    ldiff = exNode.tn.getUpdatedLength(exNode.node->dimension, ladd);
		else
		    ldiff = exNode.tn.getLengthSquare();
		
		if(ldiff < dist) {
		    nleft = exNode.node->left;
		}
	    }
	    
	    //TODO: I should stop if I'm to high in the tree
	    //on my way up && not in root
	    if(exNode.status != NONE && exNode.node->parent) {
		ExtendedNode<D> add(exNode.node->parent);
		add.tn = exNode.tn;
		if((Inner *) exNode.node->parent->right == exNode.node) 
		    add.status = RIGHT;
		else
		    add.status = LEFT;

		stack.push(add);
	    }
	    
	    // this iterates over 2 children
	    // a bit mess, but there are too many variables and it does not look
	    // nice as a method.
	    for(int c = 0; c <= 1; c++) { 
		Node * node;
		float add;
		//path ordering, choose the worse first so
		//the better will be first to pop of the stack
		if(c == 0) { 
		    node = (ldiff >= rdiff) ? nleft : nright;
		    add = (ldiff >= rdiff) ? ladd : radd;
		}
		else { //in second iteration, choose the better (=the other node)
		    node = (ldiff < rdiff) ? nleft : nright;
		    add = (ldiff < rdiff) ? ladd : radd;
		}
		
		if(node) {
		    if(node->isLeaf()) { // if node is leaf we search the bucket
			Leaf<D> * leaf = (Leaf<D> *) node;
			///BOB test
			if(minBoundsDistance(query, leaf->min, leaf->max) < dist) {
			    points *bucket = &leaf->bucket;
			    for(points_it it = bucket->begin(); it != bucket->end(); ++it) {
				(*it)->setColor(255, 255, 0); //debug
				float tmp = distance(query, *it);
				if(tmp < dist && tmp > 0) { //ie points are not the same!
				    dist = tmp;
				    nearest = *it;
				}
			    }
			}
		    }
		    else { //Not leaf, add Node to the stack with correct tracking node
			ExtendedNode<D> newN((Inner *) node);
			newN.tn = exNode.tn;
			if(add > 0)
			    newN.tn.set(exNode.node->dimension, add);
			newN.status = NONE; 
			if(newN.tn.getLengthSquare() < dist) { //check if the dist hasn't changed
			    stack.push(newN);
			}
		    }
		}
	    }
	}
	
	return nearest;
    }
    
    
    /**
     * Inserts point into the tree
     * @param point point to insert
     */
    void insert(Point<D> *point) { //TODO: insert to empty tree
	size++;
	Leaf<D> * leaf = findBucket(point);
	if(leaf->bucket.size() < bucketSize) {
	    leaf->add(point);
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
    
    
    /**
     * !! This is just to compare the performance with the better version !!
     * 
     * Returns the exact nearest neighbor (NN).
     * If there are more NNs, method retuns one random.
     * @param query the point whose NN we search
     * @return nearest neigbor
     */
    Point<D> * simpleNearestNeighbor(const Point<D> *query) {
	Leaf<D> *leaf = findBucket(query);
	float dist = numeric_limits<float>::max();
	Point<D> * nearest;
	
	//find nearest point in the bucket
	for(points_it it = leaf->bucket.begin(); it != leaf->bucket.end(); ++it) {
	    (*it)->setColor(0, 255, 0);
	    float tmp = distance(query, *it, true);
	    if(tmp < dist && tmp > 0) { //ie points are not the same!
		dist = tmp;
		nearest = *it;
	    }
	}
	
	//create initial window
	float window[2*D];
	for(int d = 0; d < D; d++) {
	    window[2*d] = (*query)[d] - dist;
	    window[2*d + 1] = (*query)[d] + dist;
	}	
	//cout << window[0] << " " << window[1]<< " " << window[2]<< " " << window[3] << "\n";
	//PlyHandler::saveWindow<D>(window);
	
	exInner n;
	n.first = leaf->parent;
	if((Leaf<D> *)leaf->parent->left == leaf)
	    n.second = LEFT;
	else
	    n.second = RIGHT;
	
	stack<exInner> stack; //avoid recursion
	stack.push(n);
	
	while(!stack.empty()) { //check possible nodes
	    exInner exNode = stack.top();
	    stack.pop();
	    Inner* node = exNode.first;
	    Visited status = exNode.second;
	    
	    Node *nodes[2]; //left and right child
	    nodes[0] = nodes[1] = NULL;
	    
	    if(node->right && (status != RIGHT || status == NONE)) {
		if(window[2*node->dimension + 1] > node->split) {
		    nodes[0] = node->right;
		}
	    }

	    if(node->left && (status != LEFT || status == NONE)) {
		if(window[2*node->dimension] <= node->split) {
		    nodes[1] = node->left;
		}
	    }
	    
	    for(int i = 0; i < 2; i++) {
		if(nodes[i]) { //check node 
		    if((nodes[i])->isLeaf()) {
			points bucket = ((Leaf<D> *)(nodes[i]))->bucket;
			for(points_it it = bucket.begin(); it != bucket.end(); ++it) {
			    //(*it)->setColor(255, 255, 0);
			    float tmp = distance(query, *it, true);
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
			//Not leaf, add Node to the stack
			exInner add((Inner *) nodes[i], NONE);
			stack.push(add);
		    }
		}
	    }
	    
	    //on my way up && not in root
	    if(status != NONE && node->parent) {
		exInner add;
		add.first = (Inner *) node->parent;
		if((Inner *) node->parent->right == node) 
		    add.second = RIGHT;
		else
		    add.second = LEFT;

		stack.push(add);	
	    }
	}
	
	return nearest;
    }
    
};

#endif	/* KDTREE_H */
