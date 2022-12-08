#include "rrt.h"
#include <iostream>
#include <cstring>
#include <time.h>
#include <fstream>
// #define MAP "./office.pgm"
// #define ENDPOINT Coordinate(34.5,14)
// //[8,10]==>[34.5,14]

// #define MAP "./map.pgm"
// #define ENDPOINT Coordinate(15,3)
//[8,10]==>[15,3]


// #define IS_OBSTACLE(pgm,i,j) (i<0 || j<0 || i>=pgm->height || j>=pgm->width || pgm->raster[(i)*pgm->width+(j)]<250)
// #define OFFICE "./office.pgm"

std::vector<MapChoice> choices={MapChoice("./map.pgm",{8,10},Coordinate(15,3),"map"),MapChoice("./office.pgm",{8,10},Coordinate(34.5,14),"office"),MapChoice("./labyrinth.pgm",{39.5,12.5},Coordinate(34.5,11.5),"labyrinth")};
#define RESOLUTION 0.05
#define RADIUS 0.2
bool is_obstacle(PGM* pgm, int i, int j) {
    // printf("height: %d, width: %d\n", pgm->height, pgm->width);
    // printf("i: %d, j: %d, land on: %d\n", i, j, pgm->raster[(i)*pgm->width+(j)]);
    return i<0 || j<0 || i>=pgm->height || j>=pgm->width || pgm->raster[(i)*pgm->width+(j)]<250;
}
int main(int argc, char* argv[]) {
    int iteration=0;
    int threadNum=0;;
    int mapSelection=0;
    if (argc==1) {
		printf("\nFormat: %s [map] [a] [r] [c]\n",argv[0]);
        printf("\n[map] is the map index\n");
		printf("\t1 = map\n");
        printf("\t2 = office\n");
		printf("\n[r] iteration(only used in bash)\n");
        printf("\n[c] is the threads number (only used in bash)\n");
		return 0;
	}
	if(argc>1){
        mapSelection= atoi(argv[1])-1;
    }
	if (argc>2) {
	    iteration = atoi(argv[2]);
	}

	if (argc>3) {
		threadNum = atoi(argv[3]);
        // omp_set_num_threads(threadNum);
	}
    MapChoice mapConfig=choices[mapSelection];
    Config config=Config(iteration,threadNum,mapConfig);
    // int seed = time(NULL);
    int seed = 5000;
    srand(seed);

    Map map=Map(RESOLUTION);
    char* mapName = const_cast<char*>(mapConfig.map.c_str());
    map.loadpgm(mapName);
    PGM *pgm=load_pgm(mapName);
    // fprintf("width:%d,height%d,\n",pgm->width,pgm->height);
    Timer timer=Timer();
    Timer inflatetimer=Timer();

    timer.start();
    inflatetimer.start();
    // std::cout<<"load map successfully!"<<std::endl;
    // inflate_obstacles_parallel(map.pgm, RADIUS/RESOLUTION);
    inflate_obstacles_parallel(map.pgm, RADIUS/RESOLUTION);

    inflatetimer.end();

    // if (is_obstacle(map.pgm,end_x/RESOLUTION,end_y/RESOLUTION)) {
    //     printf("End point lands on obstacle, exit now\n");
    //     return -1;
    // }
    inflatetimer.print_total_duration("inflate obstacles");
    inflatetimer.writeTofile("inflate_obstacles",config);

    nodeList tree=nodeList();
    RRT rrt=RRT(map,tree);


    rrt.setStartPoint(mapConfig.start_point);

    rrt.setEndPoint(mapConfig.end_point);

    rrt.start_search(config);
    // clock_gettime(CLOCK_MONOTONIC, &cur_timer);
    // uint64_t end_time = getTime(&cur_timer);
    // uint64_t duration = end_time - start_time;
    // printf("Time spent: %ld ms\n", duration);
    timer.end();
    timer.print_duration("total");
    timer.writeTofile("total",config);
    // std::cout<<" end rrt start search!"<<std::endl;
    std::ofstream myfile;
    myfile.open ("summary.csv",ios::app);
    myfile<<threadNum<<","<<iteration<<","<<timer.get_duration()<<tree.getSize()<<std::endl;
    myfile.close();

    // std::cout<<"start creating the tree!"<<std::endl;

    
    for(int i=1;i<rrt.tree->getSize();i++){
        int currX = (int)(rrt.tree->coordinates[i].first / RESOLUTION);
        int currY = (int)(rrt.tree->coordinates[i].second / RESOLUTION);
        int parentX=(int)(rrt.tree->parents[i].first / RESOLUTION);
        int parentY=(int)(rrt.tree->parents[i].second/ RESOLUTION);

        draw_line(pgm,currX,currY,parentX,parentY,0);
    }
    if(pgm!=nullptr){
        string filename="./pic/ouput_"+mapConfig.name+".pgm";
        strcpy(pgm->file,filename.c_str());
        // std::cout<<" start saving!"<<std::endl;

        save_pgm(pgm);
        // std::cout<<" end saving search!"<<std::endl;
    }

}