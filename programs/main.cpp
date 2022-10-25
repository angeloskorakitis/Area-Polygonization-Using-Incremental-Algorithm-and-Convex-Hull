#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Simple_cartesian.h>
// #include <CGAL/Cartesian.h>
#include <CGAL/Polygon_2.h>
// #include <CGAL/property_map.h>
// #include <CGAL/draw_polygon_2.h>
// #include <CGAL/intersections.h>
#include <vector>
#include <numeric>
#include <string>

typedef CGAL::Exact_predicates_inexact_constructions_kernel   K;
typedef K::Point_2                                            Point;
typedef CGAL::Segment_2<K>                                    Segment;
typedef CGAL::Object                                          Object;   
typedef CGAL::Polygon_2<K>                                    Polygon;
typedef CGAL::Triangle_2<K>                                   Triangle;
// typedef CGAL::Convex_hull_traits_adapter_2<K,CGAL::Pointer_property_map<Point>::type > Convex_hull_traits_2;
typedef Polygon::Vertex_iterator                              VertexIterator;
typedef Polygon::Edge_const_iterator                          EdgeIterator;
typedef std::vector<Point>                                    PointVector;
typedef std::vector<Segment>                                  SegmentVector;
typedef PointVector::iterator                                 pPointVector;
typedef SegmentVector::iterator                               pSegmentVector;
typedef std::string                                           String;

//
// Περιγραφή του κώδικα
// Από ένα vector που έχουμε τα δεδομένα σημεία ξεκινάμε κάνοντας τα iterate με έναν pointer και βάζοντας αρχικά
// 3 στο πολύγωνο μας. Ξεκιναμε μια επανάληψη όπου για όσο υπάρχουν σημεία που δεν έχουμε εισάγει στην πολυγωνική μας γραμμή
// παίρνουμε το ΚΠ των σημείων μας και κάνουμε iterate τα σημεία του ΚΠ 'προσπαθώντας' να προσθέσουμε σημεία στο πολύγωνο διατηρώντας το απλό
//

bool is_edge_visible(Point ,Segment ,Polygon );
// bool is_edge_visible(Segment ,Segment ,Polygon );
Polygon incremental_algorithm(PointVector );
void print_point(Point);
void print_segment(Segment);
void print_polygon(Polygon);

// Να το δούμε
// class Polygonization
// {
//   private:
//     const PointVector input_points;
//     const String algorithm;
//     const String output_file;
//     const String edge_selection;
//     Polygon polygon;
//   public:
//     Polygonization(std::string input_string);
//     ~Polygonization();
//     String get_algorithm()const;
//     String get_output_file()const;
//     String get_edge_selection()const;
//     Polygon get_polygon();
//     Polygon incremental_algorithm(Polygon);
//     Polygon convex_hull_algorithm(Polygon);
//     void set_polygon(Polygon);
// };


int main(int argc, char *argv[])
{
  // Επεξεργασία Παραμέτρων Εισόδου

  // Το σύνολο των αρχικών σημείων τα οποία θα δίνει ο χρήστης από ένα αρχείο (προσωρινό - να αλλάξει)
  PointVector points;
  points.push_back(Point(0,0));
  points.push_back(Point(10,0));
  // points.push_back(Point(10,10));
  points.push_back(Point(6,5));
  points.push_back(Point(4,1));
  points.push_back(Point(1,1));
  points.push_back(Point(3,1));

  Polygon convex_hull;
  // Πρώτο κάνω sort τα σημεία με βάση μια συντεταγμένη (για την ώρα είναι σορταρισμένα στην x) STD::SORT
  std::sort(points.begin(), points.end());
  // Polygon polygon(vertices.begin(), vertices.begin()+ 3);
  CGAL::convex_hull_2(points.begin(), points.end(), std::back_inserter(convex_hull));
  print_polygon(convex_hull);
  // Πολυγωνοποίηση
  Polygon polygon1 = incremental_algorithm(points);
  print_polygon(polygon1);
  // Εκτύπωση πολυγώνου
  return EXIT_SUCCESS;

}

// // Ελέγχει αν ένα segment είναι ορατό από ένα σημείο
bool is_edge_visible(Point point, Segment segment, Polygon convex_hull)
{
  Point source = segment.source();
  Point target = segment.target();

  // Δημιουργώ το τρίγωνο με κορυφές Source Target Point
  Triangle triangle(point, source, target);

  // Υπολογίζεις το intersection των παραπάνω segments με το polygon και θες να έχουν μόνο ένα κοινό σημείο δηλ. το σημείο source και target αντίστοιχα
  // Για κάθε segment του CH να υπολογίζω τα σημεία που κάνει intersect
  // Για να ξερω οτι τα δυο segments τεμνουν μονο τη μια κορυφη αρκει να ελεγξω αν τεμνουν μονο (!) τις δυο γειτονικες ακμές του ΚΠ
  //!!! Ενδεχομένως να μπορω να παρω το τριγωνο και να δω αν υπαρχουν γραμμες του πολυγωνου που το τεμνουν !!!
  for (EdgeIterator edge_itr = convex_hull.edges_begin(); edge_itr != convex_hull.edges_end(); ++edge_itr) 
  {
    Point intersection_point;
    Segment intersection_segment;

    Object result = CGAL::intersection(triangle, *edge_itr);
    
    // Αν η τομή τους είναι ένα σημείο...
    if (CGAL::assign(intersection_point, result)) 
    {
      if(intersection_point == target) continue;
      if(intersection_point == source) continue;
      // !!! Δεν είμαι σίγουρος γι αυτό...να το σκεφτώ περισσότερο !!!
      else if(intersection_point != target) return false;
      else if(intersection_point != source) return false;

    }
    else
    {
      // Αν η τομή τους είναι ένα ευθύγραμμο τμήμα τότε...
      if (CGAL::assign(intersection_segment, result)) 
      {
        // Αν η τομή τους είναι segment που είναι το segment τότε είναι ορατό...άρα συνεχίζουμε
        if(intersection_segment == segment) continue;
        
        // Αν η τομή τους είναι segment που δεν είναι το segment τότε δεν είναι ορατό...άρα επιστροφή false
        else if(intersection_segment != segment) return false;
      
      // Αν δεν υπάρχει τομή τότε...συνέχισε    
      } 
      else 
        continue;
    }
  }

  return true;
}

// ΟΧΙ ΟΛΟΚΛΗΡΩΜΕΝΗ !!!
// Ελέγχει αν ένα segment είναι ορατό από ένα σημείο
// Ιδέα: Θα μπορούσαμε να χρησιμοποιησουμε την προηγούμενη συνάρτηση στα άκρα του segment
// bool is_edge_visible(Segment ch_segment, Segment polygon_segment, Polygon convex_hull)
// {
//   Point polygon_source = polygon_segment.source();
//   Point polygon_target = polygon_segment.target();

//   Point ch_source = ch_segment.source();
//   Point ch_target = ch_segment.target();

//   // Δημιουργώ το τρίγωνο με κορυφές Source Target Point
//   // if(polygon_source == ch_source) triangle( polygon_source, polygon_target, ch_target);
//   // if(polygon_source == ch_target) triangle( polygon_source, polygon_target, ch_source);
//   //...........................................................................................
//   // Υπολογίζεις το intersection των παραπάνω segments με το polygon και θες να έχουν μόνο ένα κοινό σημείο δηλ. το σημείο source και target αντίστοιχα
//   // Για κάθε segment του CH να υπολογίζω τα σημεία που κάνει intersect
//   // Για να ξερω οτι τα δυο segments τεμνουν μονο τη μια κορυφη αρκει να ελεγξω αν τεμνουν μονο (!) τις δυο γειτονικες ακμές του ΚΠ
//   //!!! Ενδεχομένως να μπορω να παρω το τριγωνο και να δω αν υπαρχουν γραμμες του πολυγωνου που το τεμνουν !!!
//   Triangle triangle( polygon_source, polygon_target, ch_target);

//   for (EdgeIterator edge_itr = convex_hull.edges_begin(); edge_itr != convex_hull.edges_end(); ++edge_itr) 
//   {
//     Point intersection_point;
//     Segment intersection_segment;

//     Object result = CGAL::intersection(triangle, *edge_itr);

//     // Αν η τομή τους είναι ένα σημείο...
//     if (CGAL::assign(intersection_point, result)) 
//     {
//       if(intersection_point == ch_target) continue;
//       if(intersection_point == ch_source) continue;
//       // !!! Δεν είμαι σίγουρος γι αυτό...να το σκεφτώ περισσότερο !!!
//       else if(intersection_point != ch_target) return false;
//       else if(intersection_point != ch_source) return false;

//     }
//     else
//     {
//       // Αν η τομή τους είναι ένα ευθύγραμμο τμήμα τότε...
//       if (CGAL::assign(intersection_segment, result)) 
//       {
//         // Αν η τομή τους είναι segment που είναι το segment τότε είναι ορατό...άρα συνεχίζουμε
//         if(intersection_segment == ch_segment) continue;
        
//         // Αν η τομή τους είναι segment που δεν είναι το segment τότε δεν είναι ορατό...άρα επιστροφή false
//         else if(intersection_segment != ch_segment) return false;
      
//       // Αν δεν υπάρχει τομή τότε...συνέχισε    
//       } 
//       else 
//         continue;
//     }
//   }

//   return 0;
// }

// ΟΧΙ ΟΛΟΚΛΗΡΩΜΕΝΗ !!!
// Η υλοποίηση του αυξητικού αλγορίθμου
Polygon incremental_algorithm(PointVector input_points)
{
  pPointVector p_input_points = input_points.begin();
  Polygon polygon(p_input_points, p_input_points+ 3);

   // Ο δείκτης στο points να δείχνει 3 θέσεις μετά
  advance(p_input_points, 3);

  Polygon convex_hull;

  // Οσο ο αριθμός των κορυφών του πολυγώνου είναι διαφορετικός από τον αριθμό των σημείων, επανάλαβε... 
  while(input_points.size() != polygon.size())
  {
    // Υπολογισμός του ΚΠ
    CGAL::convex_hull_2(polygon.begin(), polygon.end(), std::back_inserter(convex_hull));
  

    // Vector που θα περιέχει τις κόκκινες ακμές
    SegmentVector red_edges;

    // Για κάθε ακμή του ΚΠ, επανάλαβε...
    for (EdgeIterator edge_itr = convex_hull.edges_begin(); edge_itr != convex_hull.edges_end(); ++edge_itr) 
    {
      if(is_edge_visible(*p_input_points, *edge_itr, convex_hull) == true) red_edges.push_back(*edge_itr);
    }

    pSegmentVector p_red_edges;

    // // Αν υπάρχουν κόκκινες ακμές στο ΚΠ δηλ. ορατές από το σημείο...
    // if(red_edges.size() != 0)
    // {
    //   // Για κάθε μία βρίσκω τις ορατές ακμές 
    //   for(p_red_edges = red_edges.begin(); p_red_edges != red_edges.end(); ++p_red_edges)
    //   {
    //     // Έστω ότι η επιλογή ορατή ακμής γίνεται τυχαία...(δηλαδή επιλέγω την πρώτη ακμή)
    //     // Για κάθε κόκκινη πρέπει να βρώ ποιες γραμμές είναι ορατές...
    //   }
    // }

    // Για κάθε ορατή ακμή του ΚΠ θέλω να βρώ τις ορατές ακμές στο πολύγωνο και ανάλογα με το strategy (τυχαία επιλογή, μέγιστο, ελάχστο εμβαδόν) να τις επιλέξω...
    
    // 22
    advance(p_input_points, 1);

  }
  return polygon;
}

void print_point(Point point) {
  std::cout << "(" << point.x() << ", " << point.y() << ")" << std::endl;
}

void print_segment(Segment seg) {
  std::cout << "(" << seg.source().x() << ", " << seg.source().y() << ") - (" << seg.target().x() << ", " << seg.target().y() << ")" << std::endl;
}

void print_polygon(Polygon polygon) {
  for (EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); ++edge_itr) 
    print_segment(*edge_itr);
}