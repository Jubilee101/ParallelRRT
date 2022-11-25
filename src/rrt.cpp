#include "rrt.h"
#include "pgm.h"
#include <limits>
#include <cstring>
#include <iostream>
#include <math.h>
#define INF 99999

#define DELTA 0.15
void Map::loadpgm(char* path){
    this->pgm = load_pgm(path);
}
double getRandomVal(){
    return (double) rand() / (double) (RAND_MAX);
}
// void Map:: savepgm(char *path,nodeList*tree){
//     for(int i=0;i<tree->getSize();i++){
//         int currX = (int)(tree->coordinates[i].first / resolution);
//         int currY = (int)(tree->coordinates[i].second / resolution);
//         int parentX=(int)(tree->parents[i].first / resolution);
//         int parentY=(int)(tree->parents[i].second/ resolution);

//         draw_line(this->pgm,currX,currY,parentX,parentY,0);
//     }
// 	strcpy(this->pgm->file,path);
// 	save_pgm(this->pgm);
// }
void RRT::start_search(){
    double x_val = this->map->pgm->width * map->resolution;
	double y_val = this->map->pgm->height * map->resolution;
    int max_iter = 100;
    int iter = 0;
    tree->insert(startPoint,{INF,INF});
    //while not reach the end
    Coordinate currentPoint=startPoint;
    Coordinate potentialPoint;
    while(currentPoint!=endPoint ){
        double rd_val=getRandomVal();
        int x0,y0,x1,y1;
        Coordinate nearestCoord;
        if(rd_val<0.1){
            // std::cout<<"random seed< 0.1!"<<std::endl;

            potentialPoint=endPoint;
            double min_distance=std::numeric_limits<double>::max();
            nearestCoord=tree->findNearestNode(potentialPoint,min_distance);
            
        }else{
            // std::cout<<"random seed> 0.1!"<<std::endl;

            Coordinate randomPoint={x_val*getRandomVal(),y_val*getRandomVal()};
            double min_distance=std::numeric_limits<double>::max();
            // std::cout<<"Start find nearest node !"<<std::endl;
            nearestCoord=tree->findNearestNode(randomPoint,min_distance);
            potentialPoint={nearestCoord.first+DELTA*(randomPoint.first-nearestCoord.first)/min_distance,nearestCoord.second+DELTA*(randomPoint.second-nearestCoord.second)/min_distance};   
        }
        // std::cout<<"Start obstacle detection!"<<std::endl;

        // obstacle detection 
        if (!detect_obstacle(map->pgm,potentialPoint.first/map->resolution,potentialPoint.second/map->resolution,nearestCoord.first/map->resolution,nearestCoord.second/map->resolution,250)) {
            // std::cout<<"iteration:"<<iter<<std::endl;
            // iter++;
            // if (iter >= max_iter) {
            //     break;
            // }
            tree->insert(potentialPoint, nearestCoord);
            currentPoint=potentialPoint;
        }
    }
}

// pathNode::pathNode() {
    
// }
void nodeList::insert(Coordinate point,Coordinate parentPoint){
    coordinates.push_back(point);
    parents.push_back(parentPoint);
    size+=1;
    // std::cout<<"====Total["<<size<<"]"<<std::endl;
    // for(int i=0;i<size;i++){
    //     std::cout<<coordinates[i].first<<","<<coordinates[i].second<<")"<<std::endl;
    // }
}
Coordinate nodeList::findNearestNode(Coordinate point,double &min_distance){

    vector<double> distances(size,std::numeric_limits<double>::max());
    int min_index=-1;
    // std::cout<<"Start searching  here!"<<std::endl;
    // int min_distance=std::numeric_limits<double>::max();
    for(int i =0; i<size;i++){
        distances[i] = sqrt((coordinates[i].first - point.first) * (coordinates[i].first - point.first) + (coordinates[i].second - point.second) * (coordinates[i].second - point.second));
        if(distances[i]<min_distance){
            min_distance=distances[i];
            min_index=i;
        }
    }
    return coordinates[min_index];
 }
