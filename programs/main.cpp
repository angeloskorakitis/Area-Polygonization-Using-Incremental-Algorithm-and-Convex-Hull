#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>
#include <CGAL/draw_polygon_2.h>
#include <CGAL/intersections.h>
#include <vector>
#include <numeric>

typedef CGAL::Exact_predicates_inexact_constructions_kernel   K;
typedef K::Point_2                                            Point;
typedef CGAL::Segment_2<K>                                    Segment;
typedef CGAL::Object                                          Object;   
typedef CGAL::Polygon_2<K>                                    Polygon;
// typedef CGAL::Convex_hull_traits_adapter_2<K,CGAL::Pointer_property_map<Point>::type > Convex_hull_traits_2;
typedef Polygon::Vertex_iterator                              VertexIterator;
typedef Polygon::Edge_const_iterator                          EdgeIterator;
typedef std::vector<Point>                                    Vector;
//
// Περιγραφή του κώδικα
// Από ένα vector που έχουμε τα δεδομένα σημεία ξεκινάμε κάνοντας τα iterate με έναν pointer και βάζοντας αρχικά
// 3 στο πολύγωνο μας. Ξεκιναμε μια επανάληψη όπου για όσο υπάρχουν σημεία που δεν έχουμε εισάγει στην πολυγωνική μας γραμμή
// παίρνουμε το ΚΠ των σημείων μας και κάνουμε iterate τα σημεία του ΚΠ 'προσπαθώντας' να προσθέσουμε σημεία στο πολύγωνο διατηρώντας το απλό
//
bool is_edge_visible(Point point, Segment segment, Polygon convex_hull);

int main(int argc, char *argv[])
{
  // Το σύνολο των αρχικών σημείων τα οποία θα δίνει ο χρήστης από ένα αρχείο (προσωρινό - να αλλάξει)
  Vector vertices = { Point(1,3),
                                  Point(2,2),
                                  Point(2,4),
                                  Point(2,1),
                                  Point(3,2),
                                  Point(3,4),
                                  Point(5,3)};
  
  Vector::iterator p_vertices = vertices.begin();

  // Πρώτο κάνω sort τα σημεία με βάση μια συντεταγμένη (για την ώρα είναι σορταρισμένα στην x)

  // Επιλέγω αρχικό πολύγωνο - τρίγωνο, τα τρια σημεία με το μικρότερο x
  Polygon polygon(p_vertices, p_vertices+ 3);

  // Ο δείκτης στο points να δείχνει 3 θέσεις μετά
  advance(p_vertices, 3);

  Polygon convex_hull;
  // Οσο ο αριθμός των κορυφών του πολυγώνου είναι διαφορετικός από τον αριθμό των σημείων, επανάλαβε... 
  while(vertices.size() != polygon.size())
  {
    
    // Υπολογισμός του ΚΠ
    CGAL::convex_hull_2(polygon.edges_begin(), polygon.edges_end(), convex_hull);
    
    // Για κάθε ακμή του ΚΠ, επανάλαβε...
    for (EdgeIterator edge_itr = convex_hull.edges_begin(); edge_itr != convex_hull.edges_end(); ++edge_itr) 
    {
      if(is_edge_visible(*p_vertices, *edge_itr, convex_hull)){

      } 
    }

  return EXIT_SUCCESS;
}

// Ελέγχει αν ένα segment είναι ορατό από ένα σημείο
bool is_edge_visible(Point point, Segment segment, Polygon convex_hull) {
  Point source = segment.source();
  Point target = segment.target();

  Segment segment_source(point, source), segment_target(point, target);

  // Υπολογίζεις το intersection των παραπάνω segments με το polygon και θες να έχουν μόνο ένα κοινό σημείο δηλ. το σημείο source και target αντίστοιχα
  // Για κάθε segment του CH να υπολογίζω τα σημεία που κάνει intersect
  // Για να ξερω οτι τα δυο segments τεμνουν μονο τη μια κορυφη αρκει να ελεγξω αν τεμνουν μονο (!) τις δυο γειτονικες ακμές του ΚΠ
  //!!! Ενδεχομένως να μπορω να παρω το τριγωνο και να δω αν υπαρχουν γραμμες του πολυγωνου που το τεμνουν !!!
  for (EdgeIterator edge_itr = convex_hull.edges_begin(); edge_itr != convex_hull.edges_end(); ++edge_itr) 
    {
    int counter;
    Object result;
    Point intersection_point;
    Segment intersection_segment;

    result = CGAL::intersection(*edge_itr, segment_source);
    if (CGAL::assign(intersection_point, result)) {
      
      if(result == source) continue;
      if(result == target) continue;

    } 
    else {
        // Αν ολόκληρο το segment τέμνει
        if (CGAL::assign(iseg, result)) {
            return false;
        }
        
        continue;
    
    }
    

  return false;
}