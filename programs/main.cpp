#include "Polygonization_Using_Incremental_Algorithm.hpp"
#include <fstream>

//
// Περιγραφή του κώδικα
// Από ένα vector που έχουμε τα δεδομένα σημεία ξεκινάμε κάνοντας τα iterate με έναν pointer και βάζοντας αρχικά
// 3 στο πολύγωνο μας. Ξεκιναμε μια επανάληψη όπου για όσο υπάρχουν σημεία που δεν έχουμε εισάγει στην πολυγωνική μας γραμμή
// παίρνουμε το ΚΠ των σημείων μας και κάνουμε iterate τα σημεία του ΚΠ 'προσπαθώντας' να προσθέσουμε σημεία στο πολύγωνο διατηρώντας το απλό
//


int main(int argc, char *argv[])
{

// Επεξεργασία Παραμέτρων Εισόδου

// if(argc != 11 || argc != 9)
// {
//   perror("Error Handling - Command Line Arguments.\n");
//   return -1;
// }

// std::ifstream input_file;
// std::ofstream output_file;
// String algorithm, edge_selection, initialiazation;
// if(argv[1] == "-i") input_file.open(argv[2]);
// if(argv[3] == "-o") output_file.open(argv[4]);
// if(argv[5] == "-algorithm") algorithm = argv[6];
// if(argv[7] == "-edge_selection") edge_selection = argv[8];
// if(argc == 11)  if(argv[9] == "-initialization") edge_selection = argv[10];

// if(input_file.fail())
// {
//   perror("Error Handling - File Didn't Open.\n");
//   return -1;
// }

// PointVector input_points;
// String line;
// while(input_file)
// {
//   getline(input_file, line);

//   if( line.at(0) == '#') continue;

//   for(String::iterator it = line.begin(); it != line.end(); ++it)
//   {
//     if(it == line.begin()) continue;
//     else if(it == line.end())
//   }

// }

// input_file.close();

PointVector input_points = {
                        Point(10,0),
                        Point(0,10),
                        Point(1,1),
                        Point(8,11),
                        Point(3,4),
                        Point(0,0),
                        Point(6,6),
                        Point(18,4),
                        Point(16,16),
                        Point(10,10),
                        Point(2,6) 
  };

  // Sorting
  std::sort(input_points.begin(), input_points.end());

  // Poligonization
  Polygon polygon = incremental_algorithm(input_points);

  // Print polygon
  print_polygon(polygon);
  std::cout << CGAL::abs(polygon.area()) << std::endl;

  // If the polygon is not simple then failure...
  if(!polygon.is_simple()) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}


