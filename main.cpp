/*  
 * File:   main.cpp
 * Author: Daniel Princ
 *
 */


#include <cstdlib>
#include <iostream>
#include "PointCloudGenerator.h"
#include "PlyHandler.h"
#include "KDTree2Ply.h"
#include "KDTree.h"

using namespace std;

#define D 2

const float distance(const Point<D> * p1, const Point<D>& p2) {
    float dist = 0;
    for(int d = 0; d < D; d++) {
	float tmp = abs((*p1)[d] - (p2)[d]);
	dist += tmp*tmp;
    }
    return sqrt(dist);
}

Point<D> * naiveNN(Point<D> * query, vector< Point<D> > *data) {
    float dist = numeric_limits<float>::max();
    Point<D> * nearest;
    int i = 0;
    for(typename vector< Point<D> >::iterator it = data->begin(); it != data->end(); ++it) {
	float tmp = distance(query, *it);
	if(tmp < dist && tmp > 0) { //ie points are not the same!
	    dist = tmp;
	    nearest = &(*it);
	}
    }
    return nearest;
}

int main(int argc, char *argv[]) {
    
    float bounds[2*D] = {0.f, 10.f, 0.f, 12.f};//, 0.f, 3.f};
//    PointCloudGenerator<D> pcg;
//    vector< Point<D> > points = pcg.generatePoints(100, &bounds[0]);
    
//    PlyHandler::savePoints<D>("data/input.ply", points);
    vector< Point<D> > points = PlyHandler::load<D>("data/input.ply");
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    KDTree2Ply<D>::saveTree2Ply(&kdtree, &bounds[0]);
     
//    Point<2> p;
//    p.coords[0] = 9;
//    p.coords[1] = 0;
//    kdtree.insert(&p);
    
//    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
//	Point<D> q = *it;
//	Point<D> * nn = kdtree.nearestNeighbor(&q);
//	Point<D> * nnn = naiveNN(&q, &points);
//	if(nn->coords != nnn->coords) {
//	    cout << "You got it all wrong!\n";
//	}
//    }    
    
    
    Point<D> * q = &points[14];
    Point<D> * nn = kdtree.nearestNeighbor(q);

    nn->setColor(0, 0, 255);
    q->setColor(255, 0, 0);
    cout << "q:" << *q << "\n";
    cout << "nn: " << *nn  << "\n";
    
        
    KDTree2Ply<D>::saveTree2Ply(&kdtree, &bounds[0], "test-");
    
    return 0;
}

