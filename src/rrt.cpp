#include "rrt.h"
#include "pgm.h"
#include <limits>
#define DELTA 0.15
void Map::loadpgm(char* path){
    this->pgm = load_pgm(path);
}
//to do
double getRandomVal(){
    return (double) rand() / (double) (RAND_MAX);
}
void Map:: save_pgm(char *path){
    // for (int i=1;i<NODES;i++) {
	// 	int x0 = (int)round(tree.nodes[i].x / RESOLUTION);
	// 	int y0 = (int)round(tree.nodes[i].y / RESOLUTION);
	// 	int x1 = (int)round(tree.parents[i].x / RESOLUTION);
	// 	int y1 = (int)round(tree.parents[i].y  / RESOLUTION);
		draw_line(this->pgm,x0,y0,x1,y1,0);
	// }
	strcpy(this->pgm->file,path);
	save_pgm(this->pgm);
}
void RRT::start_search(){
    double x_val = this->map.pgm->width * map.resolution;
	double y_val = this->map.pgm->height * map.resolution;
    tree->insert(startPoint,nullptr);
    //while not reach the end
    Coordinate currentPoint=startPoint;
    while(currentPoint!=endPoint ){
        double rd_val=getRandomVal();
        int x0,y0,x1,y1;
        if(rd_val<0.1){
            currentPoint=endPoint;
            
        }else{
            Coordinate randomPoint={x_val*getRandomVal(),y_val*getRandomVal()};
            int min_distance=std::numeric_limits<double>::max();
            Coordinate nearestCoord=tree->findNearestNode(randomPoint,min_distance);
            tree->insert(nearestCoord.first+DELTA*(randomPoint.first-nearestCoord.first)/min_distance,nearestCoord.second+DELTA*(randomPoint.second-nearestCoord.second)/min_distance);
            

        }
        //obstacle detection 
    }
}

pathNode::pathNode() {
    
}
void NodeList::insert(Coordinate point,Coordinate parentPoint){
    coordinates.append(point);
    parents.append(parentPoint);
    size+=1;
}
Coordinate nodeList::findNearestNode(Coordinate point,int &min_distance){

    vector<double> distance(size,std::numeric_limits<double>::max());
    int min_index=-1;
    // int min_distance=std::numeric_limits<double>::max();
    for(int i =0; i<size;i++){
        distances[i] = (coordinates[i].first - point.first) * (coordinates[i].fisrt - random.fisrt)  + (coordinates[i].second - point.second) * (coordinates[i].second - point.second);
        if(distance[i]<min_distance){
            min_distance=distances[i];
            min_index=i;
        }
    }
    return coordinates[i];
 }
