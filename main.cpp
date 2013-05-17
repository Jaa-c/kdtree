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
#include <dirent.h>
#include <cstring>
#include <random>
#include "PointCloudGenerator.h"
#include "PlyHandler.h"
#include "KDTree2Ply.h"
#include "KDTree.h"

using namespace std;

/** Data dimension */
#define D 2
/** Folder for output data */
const string output_dir = "./";///home/jaa/.wine/drive_c/data/";

///FORWARD DECLARATIONS:
/** distance calculations for naiveNN function*/
const float distance(const Point<D> * p1, const Point<D>& p2);
/** naive NN, search NN by iteration over all points */
Point<D> * naiveNN(Point<D> * query, vector< Point<D> > *data);
/** tests if kd-tree nearest neigbor returns the smae as naive NN */
void testNNCorrectness(float * bounds);
/** test if sliding midpoint works ok */
void testSlidingMidPoint();
/** compares optimized and simeple NN in kd-tree*/
void compareNNandSimple();
/** does circular query on data and prints data to output folder */
void printCircularQuery(float * bounds);
/** does kNearest query and prits data to output folder */
void printKNearest();
/** prints tree (colored buckets) + splitting lines */
void printBuckets();
/** runs a lot of circular queries for profiling */
void runCircular(float * bounds);
/** runs a lot of knn queries for profiling */
void runKNN(float * bounds);
/** runs a lot of nn queries for profiling */
void runNN(float * bounds);
/** compare time of NN search based on data distribution */
void compareNNonDatasets(string path);
/** counts the number of visited nodes per search */
void countVisitedNodes();
/** prints kNN on real data */
void printKnnOnRealData();


int main(int argc, char *argv[]) {
    
    float bounds[2*5] = {0.f, 10.f, 0.f, 12.f, 0.f, 10.f, 1.f, 3.f, 3.f, 9.f};
    
//    compareNNandSimple();
//    compareNNonDatasets("/home/jaa/Dokumenty/FEL/BP/modely/stodulky/");
//    
      printKNearest();
//    printCircularQuery(bounds);
//    printBuckets();
//    printKnnOnRealData();
//    
//    testNNCorrectness(bounds);
//    testSlidingMidPoint();
//    
//    countVisitedNodes();
//    
//    runNN(bounds);
//    runCircular(bounds);
//    runKNN(bounds);
     
    return 0;
}


void printKnnOnRealData() {
    vector< Point<D> > points;
    string path = "../data/stodulky/";//home/jaa/Dokumenty/FEL/BP/modely/stodulky/";
        
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
	while ((ent = readdir (dir)) != NULL) {
	    if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
		vector< Point<D> > temp = PlyHandler::load<D>(path + ent->d_name);
		points.insert(points.end(), temp.begin(), temp.end());
	    }
	}
	closedir (dir);
    }
    
    const int size = points.size();
    cout << "size: " << size << "\n";
    
    KDTree<D> tree;
    tree.construct(&points);

    vector<Point<D> *> data = tree.kNearestNeighbors(&points[500000], 600);
    
    (points[500000]).setColor(255, 0, 0);
    for(vector< Point<D> *>::iterator it = data.begin(); it != data.end(); ++it) {
	(*it)->setColor(255, 255, 0);
    }
    
    PlyHandler::savePoints<D>(output_dir + "realData.ply", points);
    
}

void countVisitedNodes() {
    const int size = 1000000;
    const int count = 1000;
   
    vector< Point<D> > points = PointCloudGen<D>::genGaussDistr(size);
    KDTree<D> tree;
    tree.construct(&points);
    
    int visited = 0;
    for(int i = 0; i < count; i++) {
	int n = rand() % size;
	Point<D> *p = tree.nearestNeighbor(&points[n]);
	//Point<D> *p = tree.simpleNearestNeighbor(&points[n]);
	visited += tree.getVisitedNodes();
    }
        
    cout << "NN visited node per search: " << visited / (float) count << "\n";
    
}

void compareNNonDatasets(string path) {
    vector< Point<D> > points;
    const int count  = 50000;
        
    struct timeval start, end;
    long seconds, useconds;  
    long time1, time2, time3;
    
    srand((unsigned)std::time(0)); 
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
	while ((ent = readdir (dir)) != NULL) {
	    if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
		vector< Point<D> > temp = PlyHandler::load<D>(path + ent->d_name);
		points.insert(points.end(), temp.begin(), temp.end());
	    }
	}
	closedir (dir);
    }
    
    const int size = points.size();
    cout << "size: " << size << "\n";
    
    KDTree<D> real;
    real.construct(&points);
    
    gettimeofday(&start, NULL);
    for(int i = 0; i < count; i++) {
	int n = rand() % size;
	Point<D> *p = real.nearestNeighbor(&points[n]);
    }
    gettimeofday(&end, NULL);
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    time1 = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    cout << "  real data time: " << time1 << "ms\n";
    

    points = PointCloudGen<D>::genGaussDistr(size);
    KDTree<D> normal;
    normal.construct(&points);
    
    gettimeofday(&start, NULL);
    for(int i = 0; i < count; i++) {
	int n = rand() % size;
	Point<D> *p = normal.nearestNeighbor(&points[n]);
    }
    gettimeofday(&end, NULL);
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    time2 = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    cout << "  normal data time: " << time2 << "ms\n";
        
    
    points = PointCloudGen<D>::genRandPoints(size);
    KDTree<D> random;
    random.construct(&points);
    
    gettimeofday(&start, NULL);
    for(int i = 0; i < count; i++) {
	int n = rand() % size;
	Point<D> *p = random.nearestNeighbor(&points[n]);
    }
    gettimeofday(&end, NULL);
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    time3 = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    cout << "  normal data time: " << time3 << "ms\n";

}

void testSlidingMidPoint() {
    float bounds[2*2] = {0.f, 10.f, 0.f, 12.f};
    float bounds1[2*2] = {0.f, 1.f, 0.f, 1.2f};
    
    vector< Point<D> > points = PointCloudGen<D>::genRandPoints(40, &bounds1[0]);
    
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    KDTree2Ply<D>::saveTree2Ply(&kdtree, bounds, output_dir + "tree-", true);
}

void runCircular(float * bounds) {
    vector< Point<D> > points = PointCloudGen<D>::genRandPoints(10000, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	kdtree.circularQuery(&q, 3.0f);
    }
}

void runKNN(float * bounds) {
    
    vector< Point<D> > points = PointCloudGen<D>::genRandPoints(10000, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    const int nns = 50;
    
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	vector< Point<D> *> data = kdtree.kNearestNeighbors(&q, nns);
	if(data.size() != nns) {
	    cerr << "error!, returned " << data.size() << " instead of " << nns << " points\n";
	}
    }
}

void runNN(float * bounds) {
    
    vector< Point<D> > points = PointCloudGen<D>::genRandPoints(100000, &bounds[0]);
    
    KDTree<D> kdtree;
    kdtree.construct(&points, &bounds[0]);
    
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> q = *it;
	kdtree.nearestNeighbor(&q);
    }
}

void printBuckets() {
    
    //vector< Point<D> > points = PointCloudGen<D>::generateRandomPoints(100, &bounds[0]);
    vector< Point<D> > points = PointCloudGen<D>::genGaussDistr(150);
    
    KDTree<D> kdtree;
    kdtree.construct(&points);
        
    KDTree2Ply<D>::saveTree2Ply(&kdtree, kdtree.getBoundingBox(), output_dir + "tree-", true);

}

void compareNNandSimple() {
    
    vector< Point<D> > points = PointCloudGen<D>::genRandPoints(300000);
    
    KDTree<D> kdtree;
    kdtree.construct(&points);
    
    struct timeval start, end;
    long seconds, useconds;  
    long time1, time2, time3;
    
    gettimeofday(&start, NULL);
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> *p = kdtree.nearestNeighbor(&(*it));
	p->setColor(255,0,0);
    }
    gettimeofday(&end, NULL);
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    time1 = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    cout << "  nearestNeighbor time: " << time1 << "ms\n";
    
    gettimeofday(&start, NULL);
    for(vector< Point<D> >::iterator it = points.begin(); it != points.end(); ++it) {
	Point<D> *p = kdtree.simpleNearestNeighbor(&(*it));
	p->setColor(255,0,0);
    }
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    time2 = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    cout << "  simpleNearestNeighbor time: " << time2 << "ms\n";
    
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
    cout << "> nearestNeighbor method is " << (time2/ (double) time1) << "x faster than simple and "
	    << (time3/ (double) time1) << "x faster than naive.\n";

}

void printCircularQuery(float * bounds) {
    
    vector< Point<D> > points = PointCloudGen<D>::genRandPoints(100000, &bounds[0]);
    
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


void printKNearest() {
    cout << "running kNN query of 1300 points on random data, size 100000\n";
    vector< Point<D> > points = PointCloudGen<D>::genGaussDistr(100000);
    
    KDTree<D> kdtree;
    kdtree.construct(&points);

    Point<D> * p =  &points[13];
    const int k = 1300;
    vector< Point<D> * > result = kdtree.kNearestNeighbors(p, k);
    cout << "found " << result.size() << " points in the query\n";
    
    for(vector< Point<D> *>::iterator it = result.begin(); it != result.end(); ++it) {
	(*it)->setColor(255, 255, 0);
    }
    p->setColor(255, 0, 0);
    
    PlyHandler::savePoints<D>(output_dir + "kNN.ply", points);
}

void testNNCorrectness(float * bounds) {
    
    vector< Point<D> > points = PointCloudGen<D>::genRandPoints(10000, &bounds[0]);;
    
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