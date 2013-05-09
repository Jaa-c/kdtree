/* 
 * File:   main.cpp
 * Author: Daniel Princ
 *
 */

#include <cstdlib>
#include <iostream>
#include "PointCloudGenerator.h"
#include "PlyHandler.h"
#include "KDTree.h"

using namespace std;

#define D 2

int main(int argc, char *argv[]) {
    float bounds[2*D] = {0.f, 5.f, 0.f, 10.f};
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generatePoints(100, &bounds[0]);
    vector< const Point<D>* > pointers;
    
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	pointers.push_back(&(*it));
    }
    
    PlyHandler::save<D>("data/out.ply", points);
    
    KDTree<D> kdtree;
    kdtree.construct(&pointers, &bounds[0]);
    kdtree.debugTree();
    
    return 0;
}

