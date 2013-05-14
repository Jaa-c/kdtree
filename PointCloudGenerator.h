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
class PointCloudGen {
       
    static constexpr float pi = 3.1415926f;
    
    static float randomFloat(float a, float b) {
	float random = ((float) rand()) / (float) RAND_MAX;
	return a + random * (b - a);
    }
    
public:
        
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
    
    static std::vector< Point<D> > genRandPoints(int count) {
	srand((unsigned)std::time(0)); 
	std::vector< Point<D> > points;
	for(int i = 0; i < count; i++) {
	    Point<D> p;
	    for(int d = 0; d < D; d++) {
		int min = 0;
		int max = 1;
		p[d] = randomFloat(min, max);
	    }
	    points.push_back(p);
	}
	return points;
    }
    
    static std::vector< Point<D> > genGaussDistr(int count) {
	srand((unsigned)std::time(0)); 
//	const int mu = 0; //the mean
//	const int sigma = 1; //enter the standard deviation you want or need
	std::vector< Point<D> > points;
	
	for(int i = 0; i < count; i+=2) {
	    Point<D> p1;
	    Point<D> p2;
	    for(int d = 0; d < D; d++) {
		float u1 = ((float) rand()) / (float) RAND_MAX;
		float u2 = ((float) rand()) / (float) RAND_MAX;

		float z1 = sqrt(-2 * log(u1)) * sin(2 * pi * u2);
		float z2 = sqrt(-2 * log(u1)) * cos(2 * pi * u2);
		
		//float x1 = mu + z1 * sigma;
		//float x2 = mu + z2 * sigma;
		
		p1[d] = z1;
		p2[d] = z2;
	    }
	    points.push_back(p1);
	    points.push_back(p2);
	}

	return points;
    }
    

};


#endif	/* POINTCLOUDGENERATOR_H */
