/* 
 * File:   PointCloudGenerator.h
 * Author: Dan Princ
 *
 */

#ifndef POINTCLOUDGENERATOR_H
#define	POINTCLOUDGENERATOR_H

#include <cstdlib> 
#include <vector>
#include <ctime> 
#include "Point.h"


/**
 * Generates point data
 */
template <const int D = 3>
class PointCloudGen {
       
    static constexpr float pi = 3.1415926f;
    
    static float randomFloat(float a, float b) {
	float random = ((float) rand()) / (float) RAND_MAX;
	return a + random * (b - a);
    }
    
public:
    
    /**
     * Gfenerates set of random points within given bounds
     * @param count number of points
     * @param bounds array with bounds
     * @return 
     */
    static std::vector< Point<D> > genRandPoints(int count, float *bounds) {
	srand((unsigned)std::time(0)); 
	std::vector< Point<D> > points;
	for(int i = 0; i < count; i++) {
	    Point<D> p;
	    for(int d = 0; d < D; d++) {
		int min = bounds[d*2];
		int max = bounds[d*2 + 1];
		p[d] = randomFloat(min, max);
	    }
	    points.push_back(p);
	}
	return points;
    }
    
    /**
     * Gfenerates set of random points in interval <0, 1>) 
     * @param count number of points
     * @return 
     */
    static std::vector< Point<D> > genRandPoints(int count) {
	srand((unsigned)std::time(0)); 
	std::vector< Point<D> > points;
	for(int i = 0; i < count; i++) {
	    Point<D> p;
	    for(int d = 0; d < D; d++) {
		p[d] = randomFloat(0, 1);
	    }
	    points.push_back(p);
	}
	return points;
    }
    
    /**
     * Generates set of random points with normal distribution
     * @param count number of points, always returns even number of points
     * @return 
     */
    static std::vector< Point<D> > genGaussDistr(int count) {
	srand((unsigned)std::time(0)); 
	std::vector< Point<D> > points;
	
	for(int i = 0; i < count; i+=2) {
	    Point<D> p1;
	    Point<D> p2;
	    for(int d = 0; d < D; d++) {
		float u1 = ((float) rand()) / (float) RAND_MAX;
		float u2 = ((float) rand()) / (float) RAND_MAX;

		p1[d] = sqrt(-2 * log(u1)) * sin(2 * pi * u2);
		p2[d] z2 = sqrt(-2 * log(u1)) * cos(2 * pi * u2);
	    }
	    points.push_back(p1);
	    points.push_back(p2);
	}

	return points;
    }
    
};

#endif	/* POINTCLOUDGENERATOR_H */
