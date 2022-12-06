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
void RRT::start_search(Config config){
    double x_val = this->map->pgm->width * map->resolution;
	double y_val = this->map->pgm->height * map->resolution;
    int max_iter = 100;
    int iter = 0;
    tree->insert(startPoint,{INF,INF});
    //while not reach the end
    Coordinate currentPoint=startPoint;
    Coordinate potentialPoint;
    Timer nearesttimer=Timer();
    Timer detecttimmer=Timer();
    while(currentPoint!=endPoint ){
        double rd_val=getRandomVal();
        int x0,y0,x1,y1;
        Coordinate nearestCoord;
        if(rd_val<0.1){
            // std::cout<<"random seed< 0.1!"<<std::endl;

            potentialPoint=endPoint;
            double min_distance=std::numeric_limits<double>::max();
            nearesttimer.start();
            nearestCoord=tree->findNearestNode(potentialPoint,min_distance);
            nearesttimer.end();
        }else{
            // std::cout<<"random seed> 0.1!"<<std::endl;

            Coordinate randomPoint={x_val*getRandomVal(),y_val*getRandomVal()};
            double min_distance=std::numeric_limits<double>::max();
            // std::cout<<"Start find nearest node !"<<std::endl;
            nearesttimer.start();
            nearestCoord=tree->findNearestNode(randomPoint,min_distance);
            nearesttimer.end();
            potentialPoint={nearestCoord.first+DELTA*(randomPoint.first-nearestCoord.first)/min_distance,nearestCoord.second+DELTA*(randomPoint.second-nearestCoord.second)/min_distance};   
        }
        // std::cout<<"Start obstacle detection!"<<std::endl;

        // obstacle detection 
        detecttimmer.start();
        if (!detect_obstacle(map->pgm,potentialPoint.first/map->resolution,potentialPoint.second/map->resolution,nearestCoord.first/map->resolution,nearestCoord.second/map->resolution,250)) {
            // std::cout<<"iteration:"<<iter<<std::endl;
            // iter++;
            // if (iter >= max_iter) {
            //     break;
            // }
            detecttimmer.end();
            tree->insert(potentialPoint, nearestCoord);
            currentPoint=potentialPoint;
        }else{
            detecttimmer.end();
        }
    }
    nearesttimer.print_total_duration("find nearest");
    detecttimmer.print_total_duration("detect obstacles");
    nearesttimer.writeTofile("find_nearest",config);
    detecttimmer.writeTofile("detect_obstacles",config);
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
struct reduce_struct{
    double min_distance;
    int min_index;
};
struct reduce_struct reduce_min(struct reduce_struct a, struct reduce_struct b){
    return a.min_distance<b.min_distance? a:b;
}

// Coordinate nodeList::findNearestNode(Coordinate point,double &min_distance){
//     vector<double> distances(size,std::numeric_limits<double>::max());
//     int min_index=-1;

//     for(int i =0; i<size;i++){
//         distances[i] = sqrt((coordinates[i].first - point.first) * (coordinates[i].first - point.first) + (coordinates[i].second - point.second) * (coordinates[i].second - point.second));
//         if(distances[i]<min_distance){
//             min_distance=distances[i];
//             min_index=i;
//         }
//     }
//     return coordinates[min_index];
//  }
// Coordinate nodeList::findNearestNode(Coordinate point,double &min_distance){
//     #pragma omp declare reduction(MinStruct: struct reduce_struct: omp_out =reduce_min(omp_out,omp_in)) initializer(omp_priv={std::numeric_limits<double>::max(), -1}) 
//     struct reduce_struct ans={std::numeric_limits<double>::max(),-1};
//     #pragma omp parallel for schedule(static, 1024) reduction(MinStruct:ans)
//     for(int i =0; i<size;i++){
//         double first = coordinates[i].first;
//         double second=coordinates[i].second;
//         double current_distance = sqrt((first - point.first) * (first - point.first) + (second - point.second) * (second - point.second));
//         struct reduce_struct new_val={current_distance,i};
//         ans=reduce_min(ans,new_val);
//     }
//     min_distance=ans.min_distance;
//     return coordinates[ans.min_index];
//  }
 Coordinate nodeList::findNearestNode(Coordinate point,double &min_distance){
    struct reduce_struct ans = {std::numeric_limits<double>::max(),-1};
    #pragma omp parallel
    {
        struct reduce_struct local_red = {std::numeric_limits<double>::max(),-1};
        #pragma omp for nowait
        for(int i = 0; i < size; ++i) {
                double first = coordinates[i].first;
                double second=coordinates[i].second;
                double current_distance = sqrt((first - point.first) * (first - point.first) + (second - point.second) * (second - point.second));
                local_red=reduce_min(local_red,{current_distance,i});
        }

        #pragma omp critical
        {
            ans=reduce_min(ans,local_red);  
        }    
    }
    min_distance=ans.min_distance;
    return coordinates[ans.min_index];
 }
//  Coordinate nodeList::findNearestNode(Coordinate point,double &min_distance){
//     struct reduce_struct ans = {std::numeric_limits<double>::max(),-1};
//     // struct reduce_struct local_red = {std::numeric_limits<double>::max(),-1};
//     struct reduce_struct reds[8];
//     #pragma omp for
//     for (int i = 0; i < 8; i++) {
//         reds[i] = {std::numeric_limits<double>::max(),-1};
//     }
//     #pragma omp for
//     for(int i = 0; i < size; ++i) {
//             double first = coordinates[i].first;
//             double second=coordinates[i].second;
//             double current_distance = sqrt((first - point.first) * (first - point.first) + (second - point.second) * (second - point.second));
//             int id = omp_get_thread_num();
//             reds[id]=reduce_min(reds[id],{current_distance,i});
//     }
//     #pragma omp for
//     for (int i = 0; i < 8; i++) {
//         ans = reduce_min(ans, reds[i]);
//     }
//     min_distance=ans.min_distance;
//     return coordinates[ans.min_index];
//  }
// struct XYD
// {
// 	double x;
// 	double y;
// 	double d;
// };
// XYD xyd_min2(XYD a, XYD b)
// {
// 	return a.d < b.d ? a : b;
// }

// #pragma omp declare reduction(xyd_min : XYD : omp_out=xyd_min2(omp_out,omp_in))\
// 		initializer(omp_priv={0,0,INF})
//  Coordinate nodeList::findNearestNode(Coordinate point,double &min_distance){
//     vector<double> distances(size,0);
//     #pragma omp parallel for
//     for(int i =0; i<size;i++){
//         double first = coordinates[i].first;
//         double second=coordinates[i].second;
//         distances[i]= sqrt((first - point.first) * (first - point.first) + (second - point.second) * (second - point.second));
//     }
//     XYD value = {0,0,INF};
//     #pragma omp parallel for reduction(xyd_min:value)
//     for (unsigned i=0;i<size;i++) {
//         XYD new_value = {coordinates[i].first,coordinates[i].second,distances[i]};
//         value = xyd_min2(value,new_value);
//     }
//     min_distance=value.d;
//     return {value.x,value.y};
//  }
