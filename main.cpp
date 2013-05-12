/*  
 * File:   main.cpp
 * Author: Daniel Princ
 *
 */

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <math.h>
#include "PointCloudGenerator.h"
#include "PlyHandler.h"
#include "KDTree2Ply.h"
#include "KDTree.h"

using namespace std;

#define D 2

const string output_dir = "/home/jaa/.wine/drive_c/data/";

const float distance(const Point<D> * p1, const Point<D>& p2);

/** Naive NN, search NN by iteration over all points */
Point<D> * naiveNN(Point<D> * query, vector< Point<D> > *data);

/** Tests if kd-tree nearest neigbor returns the smae as naive NN */
void testNNCorrectness(float * bounds);

/** Compares optimizde and simeple NN in kd-tree*/
void compareNNandSimple(float * bounds);

/** Does circular query on data and prints data to output folder */
void printCircularQuery(float * bounds);

/** Does kNearest query and prits data to output folder */
void printKNearest(float * bounds);

/** prints tree (colored buckets) + splitting lines */
void printBuckets(float * bounds);

int main(int argc, char *argv[]) {
    
    float bounds[2*5] = {0.f, 10.f, 0.f, 12.f, 0.f, 10.f, 1.f, 3.f, 3.f, 9.f};
    
    printKNearest(bounds);
    
    //testCircularQuery(bounds);
    
    //compareNNandSimple(bounds);
    
    //testNNCorrectness(bounds);
	    
    return 0;
    
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generatePoints(10000, &bounds[0]);
    PlyHandler::savePoints<D>("data/input.ply", points);
    
//    vector< Point<D> > points = PlyHandler::load<D>("data/input.ply");
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    KDTree2Ply<D>::saveTree2Ply(&kdtree, &bounds[0]);
     
//    Point<2> p;
//    p.coords[0] = 9;
//    p.coords[1] = 0;
//    kdtree.insert(&p);
    
    
    Point<D> * q = &points[87];
    Point<D> * nn = kdtree.nearestNeighbor(q);
    Point<D> * nnn = naiveNN(q, &points);

    nn->setColor(0, 0, 255);
    q->setColor(255, 0, 0);
    cout << "q:" << *q << "\n";
    cout << "nn: " << *nn  << "\n";
    cout << "nnn: " << *nnn  << "\n";
    
    KDTree2Ply<D>::saveTree2Ply(&kdtree, &bounds[0], "test-");
    
}

void printBuckets(float * bounds) {
    
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generatePoints(100, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    KDTree2Ply<D>::saveTree2Ply(&kdtree, &bounds[0], output_dir + "tree-");

}

void compareNNandSimple(float * bounds) {
    
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generatePoints(50000, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    long start = clock();
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	kdtree.nearestNeighbor(&q);
    }
    float time = (clock() - start) / (float) CLOCKS_PER_SEC;
    cout << "  nearestNeighbor time: " << time << "s\n";
    
    start = clock();
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	kdtree.simpleNearestNeighbor(&q);
    }
    float time2 = (clock() - start) / (float) CLOCKS_PER_SEC;
    cout << "  simpleNearestNeighbor time: " << time2 << "s\n";
    
    cout << "> first method is " << (time2/time) << "x faster.\n";

}

void printCircularQuery(float * bounds) {
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generatePoints(1000, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);

    Point<D> * p =  &points[13];
    const float radius = 3.0;
    vector< Point<D> * > result = kdtree.circularQuery(p, radius);
    cout << "found " << result.size() << " points in the query\n";
    
    for(vector< Point<D> *>::iterator it = result.begin(); it != result.end(); ++it) {
	(*it)->setColor(255, 255, 0);
    }
    p->setColor(255, 0, 0);
    
    PlyHandler::savePoints<D>(output_dir + "circularQuery.ply", points);
    
}


void printKNearest(float * bounds) {
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generatePoints(1000, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);

    Point<D> * p =  &points[13];
    const int k = 13;
    vector< Point<D> * > result = kdtree.kNearestNeighbor(p, k);
    cout << "found " << result.size() << " points in the query\n";
    
    for(vector< Point<D> *>::iterator it = result.begin(); it != result.end(); ++it) {
	(*it)->setColor(255, 255, 0);
    }
    p->setColor(255, 0, 0);
    
    PlyHandler::savePoints<D>(output_dir + "kNN.ply", points);
}

void testNNCorrectness(float * bounds) {
    
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generatePoints(5000, &bounds[0]);;
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    bool ok = true;
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	Point<D> * nn = kdtree.nearestNeighbor(&q);
	Point<D> * nnn = naiveNN(&q, &points);
	if(nn->coords != nnn->coords) {
	   cout << "You got it all wrong!\n";
	   ok = false;
	}
    } 
    
    if(ok) cout << "everything seems to be OK!\n";
    else   cout << "there are some errors :(\n";
    cout << "testNNCorrectness - DONE!\n";
}

const float distance(const Point<D> * p1, const Point<D>& p2) {
    float dist = 0;
    for(int d = 0; d < D; d++) {
	float tmp = fabs((*p1)[d] - (p2)[d]);
	dist += tmp*tmp;
    }
    return sqrt(dist);
}

Point<D> * naiveNN(Point<D> * query, vector< Point<D> > *data) {
    float dist = numeric_limits<float>::max();
    Point<D> * nearest;
    for(typename vector< Point<D> >::iterator it = data->begin(); it != data->end(); ++it) {
	float tmp = distance(query, *it);
	if(tmp < dist && tmp > 0) { //ie points are not the same!
	    dist = tmp;
	    nearest = &(*it);
	}
    }
    return nearest;
}