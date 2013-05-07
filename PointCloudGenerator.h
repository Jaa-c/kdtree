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

template <const int D = 3>
class PointCloudGenerator {
    std::vector< Point<D> > points;
    
    float randomFloat(float a, float b) {
	float random = ((float) rand()) / (float) RAND_MAX;
	return a + random * (b - a);
    }
    
public:
    
    PointCloudGenerator() {
	 srand((unsigned)std::time(0)); 
    }
    
    ~PointCloudGenerator() {}
    
    std::vector< Point<D> > generatePoints(int count, float *bounds) {
	for(int i = 0; i < count; i++) {
	    Point<D> p;
	    for(int d = 0; d < D; d++) {
		int min = bounds[d*D];
		int max = bounds[d*D + 1];
		p[d] = randomFloat(min, max);
	    }
	    points.push_back(p);
	}
	return points;
    }
    

};


#endif	/* POINTCLOUDGENERATOR_H */
