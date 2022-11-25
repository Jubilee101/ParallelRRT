# include <utility>
# include <string>
# include <vector>
#include "pgm.h"
using namespace std;

struct pathTree{
    
};
typedef pair<double,double> Coordinate;


class nodeList{    
public:
    vector<Coordinate> coordinates;
    vector<Coordinate> parents;
    int size=0;
    nodeList(){
        ;
    }
 //to do 
    void insert(Coordinate point,Coordinate parentPoint);
    Coordinate findNearestNode(Coordinate point,double &min_distance);
    int getSize(){
        return size;
    }
};

class Map
{
public:
    PGM* pgm;
	double resolution;
    Map(double resolution){
        this->resolution=resolution;
    }
    void loadpgm(char* path);
    void savepgm(char* path,nodeList*tree);
	
};
// class pathNode{
// private:
//     pathNode *children;
// 	pathNode *parent;
//     Coordinate coord;
//     double cost;
// public:
//     pathNode();
// };

// struct pathTree
// {
//     pathNode* root;
// 	unsigned size;
// };


class RRT{
private:
    Map *map=nullptr;
    Coordinate startPoint; //initial robot position
    Coordinate endPoint;
public:
    nodeList *tree=nullptr;

    RRT(Map& map,nodeList &nodeList){
        this->map=&map;
        this->tree=&nodeList;
    }
    void setStartPoint(Coordinate startPoint){
        this->startPoint=startPoint;
    }
    void setEndPoint(Coordinate endPoint){
        this->endPoint=endPoint;
    }
    Coordinate findNearestNode(Coordinate point);     
    void start_search();
};