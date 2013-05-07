/* 
 * File:   Point.h
 * Author: jaa
 *
 */

#ifndef POINT_H
#define	POINT_H

#include <iostream>

template<const int D = 3>
struct Point {
    float coords[D];
    int color[3];
    
    Point() {
	color[0] = color[1] = color[2] = 0;
    }
    
    float& operator[](int idx) {
	return coords[idx];
    }
    
    const float& operator[](int idx) const {
	return coords[idx];
    }
    
    friend std::ostream& operator<<(std::ostream& out, const Point& p) // output
    {
	out << "Point[" << D << "]: coord ";
	for(int i = 0; i < D; i++) {
	    out << p[i] << ", ";
	}
	out << "color " << p.color[0] << ", " << p.color[1] << ", " << p.color[2];
	return out;
    }
};

#endif	/* POINT_H */

