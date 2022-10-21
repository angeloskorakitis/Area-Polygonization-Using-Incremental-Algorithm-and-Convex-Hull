#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>
#include <CGAL/draw_polygon_2.h>
#include <vector>
#include <numeric>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;
typedef CGAL::Segment_2<K> Edge;
typedef CGAL::Polygon_2<K> Polygon;
typedef CGAL::Convex_hull_traits_adapter_2<K,CGAL::Pointer_property_map<Point>::type > Convex_hull_traits_2;

//
// Περιγραφή του κώδικα
// Από ένα vector που έχουμε τα δεδομένα σημεία ξεκινάμε κάνοντας τα iterate με έναν pointer και βάζοντας αρχικά
// 3 στο πολύγωνο μας. Ξεκιναμε μια επανάληψη όπου για όσο υπάρχουν σημεία που δεν έχουμε εισάγει στην πολυγωνική μας γραμμή
// παίρνουμε το ΚΠ των σημείων μας και κάνουμε iterate τα σημεία του ΚΠ 'προσπαθώντας' να προσθέσουμε σημεία στο πολύγωνο διατηρώντας το απλό
//


int main()
{
  // Το σύνολο των αρχικών σημείων τα οποία θα δίνει ο χρήστης από ένα αρχείο (προσωρινό - να αλλάξει)
  std::vector<Point> vector_points = { Point(1,3),
                                  Point(2,2),
                                  Point(2,4),
                                  Point(2,1),
                                  Point(3,2),
                                  Point(3,4),
                                  Point(5,3)};
  
  std::vector<Point>::iterator p_vector_points = vector_points.begin();

  // Πρώτο κάνω sort τα σημεία με βάση μια συντεταγμένη (για την ώρα είναι σορταρισμένα στην x)

  // Επιλέγω αρχικό πολύγωνο - τρίγωνο, τα τρια σημεία με το μικρότερο x
  Polygon temp_polygon(p_vector_points, p_vector_points+ 3);

  // Ο δείκτης στο points να δείχνει 3 θέσεις μετά
  advance(p_vector_points, 3);

  std::vector<Point> temp_convex_hull;
  // Οσο ο αριθμός των κορυφών του πολυγώνου είναι διαφορετικός από τον αριθμό των σημείων, επανάλαβε... 
  while(vector_points.size() != temp_polygon.size()){
    
    // Υπολογισμός του ΚΠ
    CGAL::convex_hull_2(temp_polygon.edges_begin(), temp_polygon.edges_end(), temp_convex_hull);
    
    std::vector<Point>::iterator iter_convex_hull;
    // Για κάθε ακμή του ΚΠ, επανάλαβε...
    for(iter_convex_hull = temp_convex_hull.begin(); iter_convex_hull != temp_convex_hull.end(); ++iter_convex_hull){
      // Κοκκινες, πρόσθεση σημείου...κλπ
    }

  }
  return EXIT_SUCCESS;
}