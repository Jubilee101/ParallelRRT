# include <utility>
# include <string>
# include <vector>
#include <iostream>
#include <fstream>
#include <omp.h>

#include <time.h>

#include "pgm.h"
using namespace std;


typedef pair<double,double> Coordinate;
class MapChoice{
    public:
    string map;
    Coordinate start_point;
    Coordinate end_point;
    string name;
    MapChoice(){
        map="";
        start_point={0,0};
        end_point={0,0};
        name="";
    }
    MapChoice(string m,Coordinate s,Coordinate c,string n){
        map=m;
        start_point=s;
        end_point=c;
        name=n;
    }
    
};

class Config{
    public:
    // int selection=0;
    int iteration=0;
    int threadNum=0;;
    MapChoice mapChoice;
    Config(int i,int t,MapChoice mp){
        // selection=s;
        iteration=i;
        threadNum=t;
        mapChoice=mp;
    }
};
class Timer {
private:
    struct timespec start_timer;
    struct timespec end_timer;
    uint64_t pre_duration;
    uint64_t total_duration;
    int call_count;
    uint64_t getTime(struct timespec* time) {
        return (uint64_t)time->tv_sec * 1000000 + time->tv_nsec / 1000;
    }
public:
    Timer() {
        start_timer = {0, 0};
        end_timer = {0, 0};
        pre_duration = 0;
        total_duration = 0;
        call_count=0;
    }
    void start() {
        clock_gettime(CLOCK_MONOTONIC, &start_timer);
        call_count+=1;
    }
    void end() {
        clock_gettime(CLOCK_MONOTONIC, &end_timer);
        pre_duration = get_duration();
        total_duration += pre_duration;
    }
    uint64_t get_duration() {
        uint64_t start_time = getTime(&start_timer);
        uint64_t end_time = getTime(&end_timer);
        return end_time - start_time;
    }
    void print_duration(string phase){
        cout<<"Time spent ["<<phase<<"]: "<< pre_duration << " us"<<endl;
    }
    void print_total_duration(string phase) {
        cout<<"Time spent in total ["<<phase<<"]: "<< total_duration << " us"<<endl;
    }
    void writeTofile(string phase,Config config){
        std::ofstream myfile;
        myfile.open ("./exp_results/"+phase+"_"+config.mapChoice.name+".csv",ios::app);
        // myfile<<"Phase, CPUs, ALG, ITERATION, TIME, CALLCOUNT, SINGLETIME"<<std::endl;

        myfile<<phase<<","<<config.threadNum<<","<<config.iteration<<","<<total_duration<<","<<call_count<<","<<(double)total_duration/(double)call_count<<std::endl;
        myfile.close();
    }
};

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
    void start_search(Config config);
};