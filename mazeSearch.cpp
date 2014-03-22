///////////////////////////////////////////////////////////////////////////////
////                                                                           //
//// Program Name : mazeSearch.cpp                                             //
//// Course CRN   : CS440 - Artificial Intelligence                            //
//// Instructor   : Prof. Svetlana Lazebnik                                    // 
//// Author       : Bo-Yu, Chiang                                              //
//// NetID / UIN  : bchiang3 / 677629729                                       //
//// Email        : jyt0532@gmail.com                                          //
//// Affiliation  : CS Dept., University of Illinois at Urbana-Champaign       //
//// Latest update: 09/23/2013                                                 //
////                                                                           //
//// Copyright (c) 2013, Bo-Yu, Chiang                                         //
////                                                                           //
//// This program is free software: you can redistribute it and/or modify      //
//// it under the terms of the GNU General Public License as published by      //
//// the Free Software Foundation, either version 3 of the License, or         //
//// (at your option) any later version.                                       //
////                                                                           //
//// This program is distributed in the hope that it will be useful,           //
//// but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
//// GNU General Public License for more details.                              //
////                                                                           //
//// You should have received a copy of the GNU General Public License         //
//// along with this program.  If not, see <http://www.gnu.org/licenses/>.     //
////                                                                           //
////                                                                           //
////                                                                           //
/////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <queue>
#include <stack>
#include <cstring>
#include <cmath>
#include <cfloat>

#define MAX_SIZE 128
int to[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
using namespace std;
struct Grid{
  int x;
  int y;
  int end_x;
  int end_y;
  double already_cost;
};
class compareHeuristic{
  public: 
    bool operator()(Grid& g1, Grid& g2)
    {
      int Manhattan_of_g1 = abs(g1.end_x - g1.x) + abs(g1.end_y - g1.y);
      int Manhattan_of_g2 = abs(g2.end_x - g2.x) + abs(g2.end_y - g2.y);
      if(Manhattan_of_g1 > Manhattan_of_g2)
        return true;
      else
        return false;
    }
};
class compareHeuristicForAStar{
  public: 
    bool operator()(Grid& g1, Grid& g2)
    {
      double Manhattan_of_g1 = abs(g1.end_x - g1.x) + abs(g1.end_y - g1.y);
      double Manhattan_of_g2 = abs(g2.end_x - g2.x) + abs(g2.end_y - g2.y);
      if(Manhattan_of_g1 + g1.already_cost > Manhattan_of_g2 + g2.already_cost)
        return true;
      else
        return false;
    }
};
class compareCurrentPathCost_c1{
  public: 
    bool operator()(Grid& g1, Grid& g2)
    {
      if(g1.already_cost > g2.already_cost)
        return true;
      else
        return false;
    }
};

void get_input_maze(int &Row, int &Column, char maze[MAX_SIZE][MAX_SIZE], char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  Row = 0;
  Column = 0;
  while(fgets(maze[Row], MAX_SIZE, fp)!=NULL){ 
    Row++;
    Column = strlen(maze[0])-2; 
  }
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      printf("%c", maze[i][j]);
    }
    printf("\n");
  }
}
void get_start_and_end_point(char maze[MAX_SIZE][MAX_SIZE], int &Row, int &Column
    , int &start_x, int &start_y, int &end_x, int &end_y){
  int i, j;
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      if(maze[i][j]=='P'){
        start_x = i;
        start_y = j;
      }
      if(maze[i][j]=='.'){
        end_x = i;
        end_y = j;
      }
    }
  }

}
int is_space(char maze[MAX_SIZE][MAX_SIZE], int &Row, int &Column, int &next_x, int &next_y){
  if(maze[next_x][next_y] == '%' || next_x == Row || next_y == Column 
      || next_x < 0 || next_y < 0){
    return 0;
  }
  else
    return 1;
}
int get_num_of_nodes_expanded(int depth[MAX_SIZE][MAX_SIZE], int &Row, int &Column){
  int i,j;
  int num = 0;
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      if(depth[i][j]!=-1)
        num++;
    }
  }
  return num;
}
int get_max_depth(int depth[MAX_SIZE][MAX_SIZE], int &Row, int Column){
  int i,j;
  int max = 0;
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      if(depth[i][j] > max)
        max = depth[i][j];
    }
  }
  return max;

}
void print_solution_map(char maze[MAX_SIZE][MAX_SIZE], int parent_node[MAX_SIZE][MAX_SIZE]
    , int &Row, int &Column , int &start_x, int &start_y, int &end_x, int &end_y){
  int current_x = end_x;
  int current_y = end_y;
  int i, j, tmp_current_x;
  char solution_maze[MAX_SIZE][MAX_SIZE];
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      if(maze[i][j] == 'P'){
        solution_maze[i][j] = '.';
        continue;
      }
      solution_maze[i][j] = maze[i][j];
    }
  }
  while(current_x != start_x || current_y != start_y){
    solution_maze[current_x][current_y] = '.';
    tmp_current_x = current_x;
    current_x = current_x - to[(parent_node[current_x][current_y])][0];
    current_y = current_y - to[(parent_node[tmp_current_x][current_y])][1];
  }
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      printf("%c", solution_maze[i][j]);
    }
    printf("\n");
  }

}
void dfs(char maze[MAX_SIZE][MAX_SIZE], int &Row, int &Column
    , int &start_x, int &start_y, int &end_x, int &end_y){
  int depth[MAX_SIZE][MAX_SIZE];
  int parent_node[MAX_SIZE][MAX_SIZE];
  double distance[MAX_SIZE][MAX_SIZE];
  int num_of_expansion;
  int max_depth;
  int max_size_of_frontier = 0;
  int i, j;
  int current_x, current_y, next_x, next_y;
  //initialize
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      depth[i][j] = -1;
      distance[i][j] = FLT_MAX;
      parent_node[i][j] = -1;
    }
  }
  //insert start_node and set start_node attribute
  stack<int> stack_x, stack_y;

  stack_x.push(start_x);
  stack_y.push(start_y);
  distance[start_x][start_y] = 0;
  depth[start_x][start_y] = 0;
  //if queue is not empty, pop
  while(!stack_x.empty()){
    max_size_of_frontier = max(max_size_of_frontier, (int)stack_x.size());
    current_x = stack_x.top();
    current_y = stack_y.top();
    stack_x.pop();
    stack_y.pop();

    if(current_x == end_x && current_y == end_y){
      break;
    }
    //if this node is not the goal, expand it
    for(i = 0; i < 4; i++){
      next_x = current_x + to[i][0];
      next_y = current_y + to[i][1];

      if(is_space(maze, Row, Column, next_x, next_y) && depth[next_x][next_y]==-1)
      {
        depth[next_x][next_y] = depth[current_x][current_y] + 1;
        distance[next_x][next_y] = distance[current_x][current_y] + 1;

        parent_node[next_x][next_y] = i;
        stack_x.push(next_x);
        stack_y.push(next_y);
      }
    }

  }
  num_of_expansion = get_num_of_nodes_expanded(depth, Row, Column);
  max_depth = get_max_depth(depth, Row, Column);
  print_solution_map(maze, parent_node, Row, Column, start_x, start_y, end_x, end_y);
  printf("Path cost: %lf\n", distance[end_x][end_y]);
  printf("Number of nodes expanded: %d\n", num_of_expansion);
  printf("Maximun tree depth searched: %d\n", max_depth);
  printf("Maximun size of frontier: %d\n", max_size_of_frontier);


}
void bfs(char maze[MAX_SIZE][MAX_SIZE], int &Row, int &Column
    , int &start_x, int &start_y, int &end_x, int &end_y){
  int depth[MAX_SIZE][MAX_SIZE];
  int parent_node[MAX_SIZE][MAX_SIZE];
  double distance[MAX_SIZE][MAX_SIZE];
  int num_of_expansion;
  int max_depth;
  int max_size_of_frontier = 0;
  int i, j;
  int current_x, current_y, next_x, next_y;
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      depth[i][j] = -1;
      distance[i][j] = FLT_MAX;
      parent_node[i][j] = -1;
    }
  }
  queue<int> queue_x, queue_y;

  queue_x.push(start_x);
  queue_y.push(start_y);
  distance[start_x][start_y] = 0;
  depth[start_x][start_y] = 0;
  while(!queue_x.empty()){
    max_size_of_frontier = max(max_size_of_frontier, (int)queue_x.size());
    current_x = queue_x.front();
    current_y = queue_y.front();
    queue_x.pop();
    queue_y.pop();

    if(current_x == end_x && current_y == end_y){
      break;
    }
    for(i = 0; i < 4; i++){
      next_x = current_x + to[i][0];
      next_y = current_y + to[i][1];

      if(is_space(maze, Row, Column, next_x, next_y) && depth[next_x][next_y]==-1)
      {
        depth[next_x][next_y] = depth[current_x][current_y] + 1;
        distance[next_x][next_y] = distance[current_x][current_y] + 1;

        parent_node[next_x][next_y] = i;
        queue_x.push(next_x);
        queue_y.push(next_y);
      }
    }

  }
  num_of_expansion = get_num_of_nodes_expanded(depth, Row, Column);
  max_depth = get_max_depth(depth, Row, Column);
  print_solution_map(maze, parent_node, Row, Column, start_x, start_y, end_x, end_y);
  printf("Path cost: %lf\n", distance[end_x][end_y]);
  printf("Number of nodes expanded: %d\n", num_of_expansion);
  printf("Maximun tree depth searched: %d\n", max_depth);
  printf("Maximun size of frontier: %d\n", max_size_of_frontier);


}
void gbfs(char maze[MAX_SIZE][MAX_SIZE], int &Row, int &Column
    , int &start_x, int &start_y, int &end_x, int &end_y){
  int depth[MAX_SIZE][MAX_SIZE];
  int parent_node[MAX_SIZE][MAX_SIZE];
  double distance[MAX_SIZE][MAX_SIZE];
  int num_of_expansion;
  int max_depth;
  int max_size_of_frontier = 0;
  int i, j;
  int current_x, current_y, next_x, next_y;
  Grid current_grid;
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      depth[i][j] = -1;
      distance[i][j] = FLT_MAX;
      parent_node[i][j] = -1;
    }
  }
  priority_queue<Grid, vector<Grid>, compareHeuristic> pq;
  Grid tmp = {start_x, start_y, end_x, end_y, 0};
  pq.push(tmp);
  distance[start_x][start_y] = 0;
  depth[start_x][start_y] = 0;
  while(!pq.empty()){
    max_size_of_frontier = max(max_size_of_frontier, (int)pq.size());
    current_grid = pq.top();
    current_x = current_grid.x;
    current_y = current_grid.y;
    pq.pop();
    if(current_x == end_x && current_y == end_y){
      break;
    }
    for(i = 0; i < 4; i++){
      next_x = current_x + to[i][0];
      next_y = current_y + to[i][1];
      if(is_space(maze, Row, Column, next_x, next_y) && depth[next_x][next_y]==-1)
      {
        depth[next_x][next_y] = depth[current_x][current_y] + 1;
        distance[next_x][next_y] = distance[current_x][current_y] + 1;
        parent_node[next_x][next_y] = i;
        Grid tmp = {next_x, next_y, end_x, end_y, 0};
        pq.push(tmp);
      }
    }

  }
  num_of_expansion = get_num_of_nodes_expanded(depth, Row, Column);
  max_depth = get_max_depth(depth, Row, Column);
  print_solution_map(maze, parent_node, Row, Column, start_x, start_y, end_x, end_y);
  printf("Path cost: %lf\n", distance[end_x][end_y]);
  printf("Number of nodes expanded: %d\n", num_of_expansion);
  printf("Maximun tree depth searched: %d\n", max_depth);
  printf("Maximun size of frontier: %d\n", max_size_of_frontier);
}
void astar(char maze[MAX_SIZE][MAX_SIZE], int &Row, int &Column
    , int &start_x, int &start_y, int &end_x, int &end_y){
  int depth[MAX_SIZE][MAX_SIZE];
  int parent_node[MAX_SIZE][MAX_SIZE];
  double distance[MAX_SIZE][MAX_SIZE];
  int num_of_expansion;
  int max_depth;
  int max_size_of_frontier = 0;
  int i, j;
  int current_x, current_y, next_x, next_y;
  Grid current_grid;
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      depth[i][j] = -1;
      distance[i][j] = FLT_MAX;
      parent_node[i][j] = -1;
    }
  }
  priority_queue<Grid, vector<Grid>, compareHeuristicForAStar> pq;
  Grid tmp = {start_x, start_y, end_x, end_y, 0};
  pq.push(tmp);
  distance[start_x][start_y] = 0;
  depth[start_x][start_y] = 0;
  while(!pq.empty()){
    max_size_of_frontier = max(max_size_of_frontier, (int)pq.size());
    current_grid = pq.top();
    current_x = current_grid.x;
    current_y = current_grid.y;
    pq.pop();
    if(current_x == end_x && current_y == end_y){
      break;
    }
    for(i = 0; i < 4; i++){
      next_x = current_x + to[i][0];
      next_y = current_y + to[i][1];
      if(is_space(maze, Row, Column, next_x, next_y) && depth[next_x][next_y]==-1)
      {
        depth[next_x][next_y] = depth[current_x][current_y] + 1;
        distance[next_x][next_y] = distance[current_x][current_y] + 1;
        parent_node[next_x][next_y] = i;
        Grid tmp = {next_x, next_y, end_x, end_y, distance[next_x][next_y]};
        pq.push(tmp);
      }
    }

  }
  num_of_expansion = get_num_of_nodes_expanded(depth, Row, Column);
  max_depth = get_max_depth(depth, Row, Column);
  print_solution_map(maze, parent_node, Row, Column, start_x, start_y, end_x, end_y);
  printf("Path cost: %lf\n", distance[end_x][end_y]);
  printf("Number of nodes expanded: %d\n", num_of_expansion);
  printf("Maximun tree depth searched: %d\n", max_depth);
  printf("Maximun size of frontier: %d\n", max_size_of_frontier);


}
void uniform_c1(char maze[MAX_SIZE][MAX_SIZE], int &Row, int &Column
    , int &start_x, int &start_y, int &end_x, int &end_y){
  int depth[MAX_SIZE][MAX_SIZE];
  int parent_node[MAX_SIZE][MAX_SIZE];
  double distance[MAX_SIZE][MAX_SIZE];
  int num_of_expansion;
  int max_depth;
  int max_size_of_frontier = 0;
  int i, j;
  int current_x, current_y, next_x, next_y;
  int tmp_x, tmp_y;
  int already_in_queue[MAX_SIZE][MAX_SIZE];

  double cost;
  Grid current_grid;
  Grid tmp_grid;
  //initiailize
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      depth[i][j] = -1;
      distance[i][j] = FLT_MAX;
      parent_node[i][j] = -1;
      already_in_queue[i][j] = 0;
    }
  }
  //insert start_node and set start_node attribute
  priority_queue<Grid, vector<Grid>, compareCurrentPathCost_c1> pq;
  Grid tmp = {start_x, start_y, end_x, end_y, 0};
  pq.push(tmp);
  already_in_queue[start_x][start_y] = 1;
  distance[start_x][start_y] = 0;
  depth[start_x][start_y] = 0;
  //if queue is not empty, pop
  while(!pq.empty()){
    max_size_of_frontier = max(max_size_of_frontier, (int)pq.size());
    current_grid = pq.top();
    current_x = current_grid.x;
    current_y = current_grid.y;
    pq.pop();
    if(current_x == end_x && current_y == end_y){
      break;
    }
    //if this node is not the goal, expand it
    for(i = 0; i < 4; i++){
      next_x = current_x + to[i][0];
      next_y = current_y + to[i][1];
      if(i == 1 || i == 3){//GO Up and Down
        cost = 1.0; 
      }else{
        cost = pow(0.5, next_y);
      }
      if(is_space(maze, Row, Column, next_x, next_y) 
          && (distance[current_x][current_y] + cost < distance[next_x][next_y]))
      {
        depth[next_x][next_y] = depth[current_x][current_y] + 1;
        distance[next_x][next_y] = distance[current_x][current_y] + cost;
        parent_node[next_x][next_y] = i;
        
        if(already_in_queue[next_x][next_y] ==0 ){
          Grid tmp = {next_x, next_y, end_x, end_y, distance[next_x][next_y]};
          pq.push(tmp);
          already_in_queue[next_x][next_y] = 1;
        }else{
          priority_queue<Grid, vector<Grid>, compareCurrentPathCost_c1> pq_tmp;
          while(!pq.empty()){
            tmp_grid = pq.top();
            if(tmp_grid.x == next_x && tmp_grid.y == next_y){
              tmp_grid.already_cost = distance[next_x][next_y];
            }
            pq.pop();
            pq_tmp.push(tmp_grid);
          }
          while(!pq_tmp.empty()){
            tmp_grid = pq_tmp.top();
            pq_tmp.pop();
            pq.push(tmp_grid);
          }
        }
        
      }
    }

  }
  num_of_expansion = get_num_of_nodes_expanded(depth, Row, Column);
  max_depth = get_max_depth(depth, Row, Column);
  print_solution_map(maze, parent_node, Row, Column, start_x, start_y, end_x, end_y);
  printf("Path cost: %lf\n", distance[end_x][end_y]);
  printf("Number of nodes expanded: %d\n", num_of_expansion);
  printf("Maximun tree depth searched: %d\n", max_depth);
  printf("Maximun size of frontier: %d\n", max_size_of_frontier);


}
void uniform_c2(char maze[MAX_SIZE][MAX_SIZE], int &Row, int &Column
    , int &start_x, int &start_y, int &end_x, int &end_y){
  int depth[MAX_SIZE][MAX_SIZE];
  int parent_node[MAX_SIZE][MAX_SIZE];
  double distance[MAX_SIZE][MAX_SIZE];
  int num_of_expansion;
  int max_depth;
  int max_size_of_frontier = 0;
  int i, j;
  int current_x, current_y, next_x, next_y;
  int tmp_x, tmp_y;
  int already_in_queue[MAX_SIZE][MAX_SIZE];

  double cost;
  Grid current_grid;
  Grid tmp_grid;
  for(i = 0; i < Row; i++){
    for(j = 0; j < Column; j++){
      depth[i][j] = -1;
      distance[i][j] = FLT_MAX;
      parent_node[i][j] = -1;
      already_in_queue[i][j] = 0;
    }
  }
  priority_queue<Grid, vector<Grid>, compareCurrentPathCost_c1> pq;
  Grid tmp = {start_x, start_y, end_x, end_y, 0};
  pq.push(tmp);
  already_in_queue[start_x][start_y] = 1;
  distance[start_x][start_y] = 0;
  depth[start_x][start_y] = 0;
  while(!pq.empty()){
    max_size_of_frontier = max(max_size_of_frontier, (int)pq.size());
    current_grid = pq.top();
    current_x = current_grid.x;
    current_y = current_grid.y;
    pq.pop();
    if(current_x == end_x && current_y == end_y){
      break;
    }
    for(i = 0; i < 4; i++){
      next_x = current_x + to[i][0];
      next_y = current_y + to[i][1];
      if(i == 1 || i == 3){//GO Up and Down
        cost = 1.0; 
      }else{
        cost = pow(2.0, next_y);
      }
      if(is_space(maze, Row, Column, next_x, next_y) 
          && (distance[current_x][current_y] + cost < distance[next_x][next_y]))
      {
        depth[next_x][next_y] = depth[current_x][current_y] + 1;
        distance[next_x][next_y] = distance[current_x][current_y] + cost;
        parent_node[next_x][next_y] = i;
        
        if(already_in_queue[next_x][next_y] ==0 ){
          Grid tmp = {next_x, next_y, end_x, end_y, distance[next_x][next_y]};
          pq.push(tmp);
          already_in_queue[next_x][next_y] = 1;
        }else{
          priority_queue<Grid, vector<Grid>, compareCurrentPathCost_c1> pq_tmp;
          while(!pq.empty()){
            tmp_grid = pq.top();
            if(tmp_grid.x == next_x && tmp_grid.y == next_y){
              tmp_grid.already_cost = distance[next_x][next_y];
            }
            pq.pop();
            pq_tmp.push(tmp_grid);
          }
          while(!pq_tmp.empty()){
            tmp_grid = pq_tmp.top();
            pq_tmp.pop();
            pq.push(tmp_grid);
          }
        }
        
      }
    }

  }
  num_of_expansion = get_num_of_nodes_expanded(depth, Row, Column);
  max_depth = get_max_depth(depth, Row, Column);
  print_solution_map(maze, parent_node, Row, Column, start_x, start_y, end_x, end_y);
  printf("Path cost: %lf\n", distance[end_x][end_y]);
  printf("Number of nodes expanded: %d\n", num_of_expansion);
  printf("Maximun tree depth searched: %d\n", max_depth);
  printf("Maximun size of frontier: %d\n", max_size_of_frontier);
}

int main(int argc, char *argv[]){
  if(argc ==3){
    int Row, Column;
    char maze[MAX_SIZE][MAX_SIZE];
    int start_x, start_y, end_x, end_y;
    get_input_maze(Row, Column, maze, argv[2]);
    get_start_and_end_point(maze, Row, Column, start_x, start_y, end_x, end_y);
    if(strcmp(argv[1], "dfs")==0){
      dfs(maze, Row, Column, start_x, start_y, end_x, end_y);
    }else if(strcmp(argv[1], "bfs")==0){
      bfs(maze, Row, Column, start_x, start_y, end_x, end_y);
    }else if(strcmp(argv[1], "gbfs")==0){
      gbfs(maze, Row, Column, start_x, start_y, end_x, end_y);
    }else if(strcmp(argv[1], "astar")==0){
      astar(maze, Row, Column, start_x, start_y, end_x, end_y);
    }else if(strcmp(argv[1], "uniform_c1")==0){
      uniform_c1(maze, Row, Column, start_x, start_y, end_x, end_y);
    }else if(strcmp(argv[1], "uniform_c2")==0){
      uniform_c2(maze, Row, Column, start_x, start_y, end_x, end_y);
    }

  }
} 

