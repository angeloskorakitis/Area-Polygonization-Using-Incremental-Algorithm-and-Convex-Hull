#include "Polygonization_Using_Incremental_Algorithm.hpp"
#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

//
// Περιγραφή του κώδικα
// Από ένα vector που έχουμε τα δεδομένα σημεία ξεκινάμε κάνοντας τα iterate με έναν pointer και βάζοντας αρχικά
// 3 στο πολύγωνο μας. Ξεκιναμε μια επανάληψη όπου για όσο υπάρχουν σημεία που δεν έχουμε εισάγει στην πολυγωνική μας γραμμή
// παίρνουμε το ΚΠ των σημείων μας και κάνουμε iterate τα σημεία του ΚΠ 'προσπαθώντας' να προσθέσουμε σημεία στο πολύγωνο διατηρώντας το απλό
//


int main(int argc, char *argv[])
{
  // Επεξεργασία Παραμέτρων Εισόδου


  // String input_file = "-1";
  // String output_file = "-1";
  // String algorithm = "-1";
  // String initialization = "-1";
  // int edge_selection = -1;

  // // Arguments count check.
  // if((argc != 9) && (argc != 11)) {
  //   return EXIT_FAILURE;
  // }
  

  // String argument;
  // String data;
  // // Argument pass.
  // for(int i = 1; i < argc; i+=2) {
  //   argument = argv[i];
  //   data = argv[i+1];
  //   std::cout << argument << "\n";
  //   std::cout << data << "\n";
  //   if(argument == "-i")  input_file = data;
  //   else if(argument.compare("-o"))  output_file = data;
  //   else if(argument.compare("-algorithm"))  algorithm = data;
  //   else if(argument.compare("-edge_selection")) edge_selection = std::stoi(data);
  //   else if(argument.compare("-initialization")) initialization = data;
    
  // }


  // // Argument content check.
  // if(input_file == "-1") {
  //   std::cout << "1Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
  //   return EXIT_FAILURE;
  // }
  // if(output_file == "-1") {
  //   std::cout << "2Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
  //   return EXIT_FAILURE;
  // }
  // if(algorithm == "-1") {
  //   std::cout << "3Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
  //   return EXIT_FAILURE;
  // }
  // if(initialization == "-1") {
  //   std::cout << "Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
  //   return EXIT_FAILURE;

  // }
  // if((edge_selection < 1) || (edge_selection > 3)) {
  //   std::cout << "Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
  //   return EXIT_FAILURE;

  // }

  PointVector points = parse_file(argv[1]);
  print_point_vector(points);

  // Sorting
  std::sort(points.begin(), points.end());

  // Poligonization
  Polygon polygon = incremental_algorithm(points, 3);

  // Print polygon
  print_polygon(polygon);
  unsigned int area = CGAL::abs(polygon.area());
  std::cout << area << std::endl;
  
  // If the polygon is not simple then failure...
  if(!polygon.is_simple()) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}


