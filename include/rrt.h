# include <utility>
# include <string>
# include <vector>
using namespace std;

struct pathTree{
    
};
typedef pair<double,double> Coordinate;

class Map
{
private:
	PGM* pgm;
	double resolution;
public:
    Map(double resolution){
        this->resolution=resolution;
    }
    void loadpgm(char* path);
    void savepgm(char* path);
	
};
class pathNode{
private:
    pathNode *children;
	pathNode *parent;
    Coordinate coord;
    double cost;
public:
    pathNode();
};

struct pathTree
{
    pathNode* root;
	unsigned size;
};
class nodeList{
private:
    vector<Coordinate> coordinates;
    vector<Coordinate> parents;
    int size=0
public:
 //to do 
    void insert(Coordinate point,Coordinate parentPoint);
    Coordinate findNearestNode(Coordinate point);
};

class RRT{
private:
    nodeList *tree=nullptr;
    Map *map=nullptr;
    Coordinate startPoint; //initial robot position
    Coordinate endPoint;
public:
    RRT(Map& map){
        this->map=map;
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