/*  
 * File:   main.cpp
 * Author: Daniel Princ
 *
 */

#include <cstdlib>
#include <iostream>
#include <sys/time.h>
#include <math.h>
#include <sys/resource.h>
#include "PointCloudGenerator.h"
#include "PlyHandler.h"
#include "KDTree2Ply.h"
#include "KDTree.h"

using namespace std;

#define D 3

const string output_dir = "/home/jaa/.wine/drive_c/data/";

const float distance(const Point<D> * p1, const Point<D>& p2);

/** Naive NN, search NN by iteration over all points */
Point<D> * naiveNN(Point<D> * query, vector< Point<D> > *data);

/** Tests if kd-tree nearest neigbor returns the smae as naive NN */
void testNNCorrectness(float * bounds);

/** test if sliding midpoint works ok */
void testSlidingMidPoint();

/** Compares optimized and simeple NN in kd-tree*/
void compareNNandSimple();

/** Does circular query on data and prints data to output folder */
void printCircularQuery(float * bounds);

/** Does kNearest query and prits data to output folder */
void printKNearest(float * bounds);

/** prints tree (colored buckets) + splitting lines */
void printBuckets(float * bounds);

/** runs a lot of circular queries for profiling */
void runCircular(float * bounds);

/** runs a lot of knn queries for profiling */
void runKNN(float * bounds);

/** runs a lot of nn queries for profiling */
void runNN(float * bounds);


int main(int argc, char *argv[]) {
        
    float bounds[2*5] = {0.f, 10.f, 0.f, 12.f, 0.f, 10.f, 1.f, 3.f, 3.f, 9.f};

    
    compareNNandSimple();
    
    
    //runNN(bounds);
    //runCircular(bounds);
    //runKNN(bounds);
    
    //printKNearest(bounds);
    //printBuckets(bounds);
    //printCircularQuery(bounds);
    
    //testNNCorrectness(bounds);
    //testSlidingMidPoint();
    
    
	    
    return 0;
    
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generateNormalPoints(10000);
    PlyHandler::savePoints<D>( output_dir + "gauss.ply", points);
    
//    vector< Point<D> > points = PlyHandler::load<D>("data/input.ply");
    
}

void testSlidingMidPoint() {
    float bounds[2*2] = {0.f, 10.f, 0.f, 12.f};
    float bounds1[2*2] = {0.f, 1.f, 0.f, 1.2f};
    
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generateRandomPoints(40, &bounds1[0]);
    
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    KDTree2Ply<D>::saveTree2Ply(&kdtree, bounds, output_dir + "tree-", true);
}

void runCircular(float * bounds) {
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generateRandomPoints(10000, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	kdtree.circularQuery(&q, 3.0f);
    }
}

void runKNN(float * bounds) {
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generateRandomPoints(10000, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    const int nns = 50;
    
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	vector< Point<D> *> data = kdtree.kNearestNeighbor(&q, nns);
	if(data.size() != nns) {
	    cerr << "error!, returned " << data.size() << " instead of " << nns << " points\n";
	}
    }
}

void runNN(float * bounds) {
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generateRandomPoints(100, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	kdtree.nearestNeighbor(&q);
    }
}

void printBuckets(float * bounds) {
    
    PointCloudGenerator<D> pcg;
    //vector< Point<D> > points = pcg.generateRandomPoints(100, &bounds[0]);
    vector< Point<D> > points = pcg.generateNormalPoints(100);
    
    KDTree<D> kdtree;
    kdtree.construct(&points);
        
    KDTree2Ply<D>::saveTree2Ply(&kdtree, kdtree.getBoundingBox(), output_dir + "tree-", true);

}

void compareNNandSimple() {
    
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generateRandomPoints(10000000);
    
    KDTree<D> kdtree;
    kdtree.construct(&points);
    
    struct timeval start, end;
    long seconds, useconds;  
    long time1, time2, time3;
    
    gettimeofday(&start, NULL);
    for(vector< Point<D> >::iterator it = points.begin(); it != points.begin() + 10000; ++it) {
	Point<D> *p = kdtree.nearestNeighbor(&(*it));
	p->setColor(255,0,0);
    }
    gettimeofday(&end, NULL);
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    time1 = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    cout << "  nearestNeighbor time: " << time1 << "ms\n";
    
//    gettimeofday(&start, NULL);
//    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
//	Point<D> *p = kdtree.simpleNearestNeighbor(&(*it));
//	p->setColor(255,0,0);
//    }
//    gettimeofday(&end, NULL);
//    seconds  = end.tv_sec  - start.tv_sec;
//    useconds = end.tv_usec - start.tv_usec;
//    time2 = ((seconds) * 1000 + useconds/1000.0) + 0.5;
//    cout << "  simpleNearestNeighbor time: " << time2 << "ms\n";
    
//    gettimeofday(&start, NULL);
//    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
//	Point<D> *p = naiveNN(&(*it), &points);
//	p->setColor(255,0,0); //the compiler (or whatever) discards this cycle if I dont use the point
//    }
//    gettimeofday(&end, NULL);
//    seconds  = end.tv_sec  - start.tv_sec;
//    useconds = end.tv_usec - start.tv_usec;
//    time3 = ((seconds) * 1000 + useconds/1000.0) + 0.5;
//    cout << "  naiveNN time: " << time3 << "ms\n";
//    
//    cout << "> nearestNeighbor method is " << (time2/ (double) time1) << "x faster than simple and "
//	    << (time3/ (double) time1) << "x faster than naive.\n";

}

void printCircularQuery(float * bounds) {
    PointCloudGenerator<D> pcg;
    vector< Point<D> > points = pcg.generateRandomPoints(1000, &bounds[0]);
    
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
    vector< Point<D> > points = pcg.generateRandomPoints(1000, &bounds[0]);
    
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
    vector< Point<D> > points = pcg.generateRandomPoints(10000, &bounds[0]);;
    
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
    return (dist);
}

Point<D> * naiveNN(Point<D> *query, vector< Point<D> > *data) {
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