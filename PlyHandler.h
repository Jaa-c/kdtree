/* 
 * File:   LoadPly.h
 * Author: Dan Princ
 *
 */

#ifndef PLYHANDLER_H
#define	PLYHANDLER_H

#include <cstdlib> 
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
//trim
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "Point.h"

using namespace std;


class PlyHandler {
    
    // trim from start
    static inline std::string &ltrim(std::string &s) {
	    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	    return s;
    }

    // trim from end
    static inline std::string &rtrim(std::string &s) {
	    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	    return s;
    }

    // trim from both ends
    static inline std::string &trim(std::string &s) {
	    return ltrim(rtrim(s));
    }

public:
    static vector< Point<3> > load(string file) {
	vector< Point<3> > data;
	ifstream infile(file.c_str());
	string line;
	if(getline(infile, line)) {
	    if(trim(line) != "ply") {
		return data;
	    }
	}
	else return data;
	
	if(getline(infile, line)) {
	    if(trim(line) != "format ascii 1.0") 
		return data;
	}
	else return data;
	
	
	int vertices = -1;
	while (getline(infile, line)) {
	    istringstream iss(line);
	    string s;
	    if(!(iss >> s)) break;
	    s = trim(s);
	    
	    if(vertices == -1) {
		if(s != "element") continue;
		if(!(iss >> s)) break;
		if(s != "vertex") continue;
		if(!(iss >> vertices)) break;
	    }
	    else {
		if(s == "property") continue;
		if(s == "end_header") break;
	    }
	}
	
	for(int i = 0; i < vertices; i++) {
	    getline(infile, line);
	    istringstream iss(trim(line));
	    Point<3> p;
	    float x;
	    if (!(iss >> p[0] >> p[1] >> p[2])) {
		cout << "a";
		break;
	    }
	    if (!(iss >> x >> x >> x)) {
		cout << "b";
		break;
	    }
	    if (!(iss >> p.color[0] >> p.color[1] >> p.color[2])) {
		cout << "c";
		break;
	    }
	    data.push_back(p);
	}
	infile.close();
	
	cout << "loaded " << data.size() << " points from " << file << "\n";
	
	return data;
    }
    
    template<const int D>
    static void save(string file, vector< Point<D> > data) {
	if(D > 3 || D < 2) 
	    return;
	
	cout << "saving " << data.size() << " points to " << file << "\n";
	
	ofstream myfile;
	myfile.open(file.c_str());
	
	myfile << "ply\nformat ascii 1.0\n";
	myfile << "element vertex " << data.size() << "\n";
	myfile << "property float x\n";
	myfile << "property float y\n";
	myfile << "property float z\n";
	//if(D == 3) myfile << "property float z\n";
	
	
	myfile << "property uchar r\n";
	myfile << "property uchar g\n";
	myfile << "property uchar b\n";
	myfile << "end_header\n";
	
	for(typename vector< Point<D> >::iterator it = data.begin(); it != data.end(); ++it) {
	    Point<D> p = *it;
	    myfile << p[0] << " " << p[1];
	    if(D == 3) myfile << " " << p[2];
	    else myfile << " 0";
	    
	    myfile << " " << p.color[0] << " "<< p.color[1] << " "<< p.color[2] << "\n";
	}
	
	myfile.close();
    }

};



#endif	/* PLYHANDLER_H */

