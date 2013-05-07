/* 
 * File:   main.cpp
 * Author: Daniel Princ
 *
 */

#include <cstdlib>
#include <iostream>
#include "PointCloudGenerator.h"
#include "PlyHandler.h"

using namespace std;

int main(int argc, char *argv[]) {
    float bounds[4] = {0.f, 5.f, 0.f, 10.f};
    PointCloudGenerator<2> pcg;
    vector< Point<2> > points = pcg.generatePoints(100, &bounds[0]);
    
    
    
    PlyHandler::save<2>("data/out.ply", points);
    
    
    return 0;
}

