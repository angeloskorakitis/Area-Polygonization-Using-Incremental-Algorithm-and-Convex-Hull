#include "Polygonization_Using_Incremental_Algorithm.hpp"
#include "Polygonization_Using_Convex_Hull_Algorithm.hpp"
#include <iostream>
#include <ctime>

#pragma one;

//
// Περιγραφή του κώδικα
// Από ένα vector που έχουμε τα δεδομένα σημεία ξεκινάμε κάνοντας τα iterate με έναν pointer και βάζοντας αρχικά
// 3 στο πολύγωνο μας. Ξεκιναμε μια επανάληψη όπου για όσο υπάρχουν σημεία που δεν έχουμε εισάγει στην πολυγωνική μας γραμμή
// παίρνουμε το ΚΠ των σημείων μας και κάνουμε iterate τα σημεία του ΚΠ 'προσπαθώντας' να προσθέσουμε σημεία στο πολύγωνο διατηρώντας το απλό
//


int main(int argc, char *argv[])
{

  // Input parameters.
  String error_msg = "Unable to run the program.\nUsage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> \n";
  std::string input_file = "-1";
  std::string output_file = "-1";
  std::string algorithm = "-1";
  std::string initialization = "-1";
  int edge_selection = 0;

  // Arguments count check.
  if((argc != 9) && (argc != 11)) 
  {
    std::cout << error_msg;
    return EXIT_FAILURE;
  }

  // Argument pass.
  for(int i = 1; i < argc; i+=2) {
    if(strcmp(argv[i], "-i") == 0) {
      input_file = argv[i+1];
    }
    else if(strcmp(argv[i], "-o") == 0) {
      output_file = argv[i+1];
    }
    else if(strcmp(argv[i], "-algorithm") == 0) {
      algorithm = argv[i+1];
    }
    else if(strcmp(argv[i], "-edge_selection") == 0) {
      edge_selection = atoi(argv[i+1]);
    }
    else if(strcmp(argv[i], "-initialization") == 0) {
      initialization = argv[i+1];
    }
  }

  // Argument content check.
  try
  {
    if(input_file == "-1")  throw error_msg;
    if(output_file == "-1") throw error_msg;
    if(algorithm == "-1")   throw error_msg;
    // Incremental algorithm needs initialization according to documentation
    if(initialization == "-1") if(algorithm == "incremental") throw error_msg;
    if((edge_selection < 1) || (edge_selection > 3)) throw error_msg;
  }
  catch(String error_msg)
  {
      std::cerr << error_msg;
      return EXIT_FAILURE;
  }

  // Turn input file into points vector
  PointVector points = parse_file(input_file);

  // Starting timer
  auto start = std::chrono::high_resolution_clock::now();

  srand(time(NULL));

  Polygon polygon;
  
  // Calling the algorihm function.
  if(algorithm == "incremental") {
    polygon = incremental_algorithm(points, edge_selection, initialization);
  }
  else if(algorithm == "convex_hull") {
    polygon = convex_hull_algorithm(points, edge_selection);
  }
  else {
    std::cout << "Use a valid algorithm name!" << std::endl;
    std::cerr << error_msg;
    return EXIT_FAILURE;
  }
  
  // Stoping timer
  auto stop = std::chrono::high_resolution_clock::now();

  // Running time
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  // Print output
  print_output(polygon, points, output_file, algorithm, edge_selection, initialization, duration);

  // If the polygon is not simple then failure
  if(!polygon.is_simple()) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}


