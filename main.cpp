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

int main(int argc, char *argv[]) {
    
    float bounds[2*D] = {0.f, 10.f, 0.f, 12.f};
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generatePoints(100, &bounds[0]);
    
    //PlyHandler::savePoints<D>("data/out.ply", points);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    KDTree2Ply::saveTree2Ply(&kdtree, &bounds[0]);
    
    return 0;
}

