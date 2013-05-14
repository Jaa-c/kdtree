/* 
 * File:   Point.h
 * Author: Daniel Princ
 *
 */

#ifndef POINT_H
#define	POINT_H

#include <iostream>

/**
 * Simple point with coordinates and color
 */
template<const int D = 3>
struct Point {
    float coords[D];
    int color[3];
    
    /**
     * Creates black point with no coordinates
     */
    Point() {
	color[0] = color[1] = color[2] = 0;
    }
    
    /**
     * Creates the point with given coordinates
     * @param co array of coordinates
     */
    Point(float* co) {
	for(int i = 0; i < D; i++) {
	    coords[i] = co[i];
	}
	color[0] = color[1] = color[2] = 0;
    }
    
    /**
     * Copy constructor
     * @param point original object
     */
    Point(const Point<D> & point) {
	std::copy(point.coords, point.coords + D, coords);
	std::copy(point.color, point.color + 3, color);
    }
    
    /**
     * Sets point color
     * @param r red
     * @param g green
     * @param b blue
     */
    void setColor(int r, int g, int b) {
	color[0] = r;
	color[1] = g;
	color[2] = b;
    }
    
    Point& operator= (const Point<D> & point) {
	Point tmp(point);
	std::swap(coords, tmp.coords);
	std::swap(color, tmp.color);
	return *this;
    }
    
    /**
     * Access to point coordinates
     * @param idx dimension
     * @return coordinate
     */
    float& operator[](int idx) {
	return coords[idx];
    }
    
    /**
     * Access to point coordinates
     * @param idx dimension
     * @return coordinate
     */
    const float& operator[](int idx) const {
	return coords[idx];
    }
    
    /**
     * Debug
     */
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

