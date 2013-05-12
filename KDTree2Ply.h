/* 
 * File:   KDTree2Ply.h
 * Author: Daniel Princ
 *
 * Created on 10. květen 2013, 3:24
 */

#ifndef KDTREE2PLY_H
#define	KDTREE2PLY_H

#include <cstdlib>
#include <vector>
#include "Point.h"
#include "KDTreeNodes.h"
#include "KDTree.h"

using namespace std;

template<const int D = 2>
class KDTree2Ply {
    
    typedef vector< Point<D>* > points;
    typedef typename vector< Point<D>* >::const_iterator points_it;
    
public:
    
    /**
     * Saves points, every bucket in different color
     * makes sense only in 2D
     */
    static void saveTree2Ply(const KDTree<D> *tree, const float * bounds, string name = "", bool paint = false) {
	srand((unsigned)std::time(0)); 
	vector< Point<D> > data = debugBuckets(tree->getRoot(), paint);
	PlyHandler::savePoints<D>("data/" + name + "points.ply", data);
	
	if(D != 2) return;
	data = debugTree(tree->getRoot(), bounds);
	PlyHandler::saveLines<D>("data/" + name + "lines.ply", data);
    }
    
private:
    static vector< Point<D> > debugBuckets(const Inner* node, bool paint) {
	vector< Point<D> > data;
	if(node->left) {
	    
	    if(!node->left->isLeaf()) {
		vector< Point<D> > d = debugBuckets((Inner *) node->left, paint);
		data.insert(data.end(), d.begin(), d.end());
	    }
	    else {
		Leaf<D> * l = (Leaf<D> *) node->left;
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		for(points_it it = l->bucket.begin(); it != l->bucket.end(); ++it) {
		    Point<D> p(*(*it));
		    if(paint) {
			p.color[0] = r;
			p.color[1] = g;
			p.color[2] = b;
		    }
		    data.push_back(p);
		}
	    }
	}
	if(node->right) {
	    if(!node->right->isLeaf()) {
		vector< Point<D> > d = debugBuckets((Inner *) node->right, paint);
		data.insert(data.end(), d.begin(), d.end());
	    }
	    else {
		Leaf<D> * l = (Leaf<D> *) node->right;
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		for(points_it it = l->bucket.begin(); it != l->bucket.end(); ++it) {
		    Point<D> p(*(*it));
		    if(paint) {
			p.color[0] = r;
			p.color[1] = g;
			p.color[2] = b;
		    }
		    data.push_back(p);
		}
	    }
	}
	return data;
    }
    
    static vector< Point<D> > debugTree(const Inner* node, const float* bound) {
	vector< Point<D> > data;
	if(D != 2) return data;
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
	if(node->left && !node->left->isLeaf()) {
	    float b[4];
	    std::copy(bound, bound + 4, &b[0]);
	    b[2*node->dimension + 1] = node->split;
	    vector< Point<D> > d = debugTree((Inner *) node->left, &b[0]);
	    data.insert(data.end(), d.begin(), d.end());
	}
	
	if(node->right && !node->right->isLeaf()) {
	    float b[4];
	    std::copy(bound, bound + 4, &b[0]);
	    b[2*node->dimension] = node->split;
	    vector< Point<D> > d = debugTree((Inner *) node->right, &b[0]);
	    data.insert(data.end(), d.begin(), d.end());
	}
	return data;
    }
};

#endif	/* KDTREE2PLY_H */

