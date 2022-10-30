#include "Polygonization_Using_Incremental_Algorithm.hpp"
#include <vector>
#include <numeric>
#include <string>


//
// Περιγραφή του κώδικα
// Από ένα vector που έχουμε τα δεδομένα σημεία ξεκινάμε κάνοντας τα iterate με έναν pointer και βάζοντας αρχικά
// 3 στο πολύγωνο μας. Ξεκιναμε μια επανάληψη όπου για όσο υπάρχουν σημεία που δεν έχουμε εισάγει στην πολυγωνική μας γραμμή
// παίρνουμε το ΚΠ των σημείων μας και κάνουμε iterate τα σημεία του ΚΠ 'προσπαθώντας' να προσθέσουμε σημεία στο πολύγωνο διατηρώντας το απλό
//


int main(int argc, char *argv[])
{
  // Επεξεργασία Παραμέτρων Εισόδου

// PointVector points = {
//                         Point(0,0),
//                         Point(10,10),
//                         Point(3,7),
//                         Point(3,2),
//                         Point(7,2),
//                         Point(10,0),
//                         Point(0,10)
//   };
  PointVector points = {
                        Point(0,0),
                        Point(0,5),
                        Point(0,10),
                        Point(10,0),
                        Point(0,10),
                        Point(1,1),
                        Point(8,11),
                        Point(3,4),
                        Point(6,6),
                        Point(18,4),
                        Point(16,16),
                        Point(2,6) 
  };

  // Sorting
  std::sort(points.begin(), points.end());

  // Poligonization
  Polygon polygon = incremental_algorithm(points);

  // Print polygon
  print_polygon(polygon);
  std::cout << CGAL::abs(polygon.area()) << std::endl;

  // If the polygon is not simple then failure...
  if(!polygon.is_simple()) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}


