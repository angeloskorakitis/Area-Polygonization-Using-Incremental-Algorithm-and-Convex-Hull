#include <CGAL/convex_hull_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Simple_Cartesian.h>
#include <CGAL/Polygon_2.h>
// #include <CGAL/property_map.h>
// #include <CGAL/draw_polygon_2.h>
// #include <CGAL/intersections.h>
#include <vector>
#include <numeric>
#include <string>

typedef CGAL::Epick   K;
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
Polygon incremental_algorithm(PointVector );
void print_point(Point);
void print_segment(Segment);
void print_polygon(Polygon);
void add_visible_edge(Point , SegmentVector, Polygon* );

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
  // for(pPointVector itr = points.begin(); itr != points.end(); ++itr) print_point(*itr);
  //   std::cout << "POINTS!" << std::endl;
  // Polygon convex_hull;
  // Πρώτο κάνω sort τα σημεία με βάση μια συντεταγμένη (για την ώρα είναι σορταρισμένα στην x) STD::SORT
  std::sort(points.begin(), points.end());
  // Polygon polygon(vertices.begin(), vertices.begin()+ 3);
  // CGAL::convex_hull_2(points.begin(), points.end(), std::back_inserter(convex_hull));
  // print_polygon(convex_hull);
  // Πολυγωνοποίηση
  Polygon polygon1 = incremental_algorithm(points);
  print_polygon(polygon1);
  if(!polygon1.is_simple()) return EXIT_FAILURE;
  // Εκτύπωση πολυγώνου
  return EXIT_SUCCESS;

}

// // Ελέγχει αν ένα segment είναι ορατό από ένα σημείο
bool is_edge_visible(Point point, Segment segment, Polygon convex_hull)
{
  Point segment_source = segment.source();
  Point segment_target = segment.target();

// Τι θα κάνουμε με τα συνευθειακά σημεία; Προς το παρόν τα συμπεριφέρομαι ως not visible.
  if(CGAL::collinear(point, segment_source, segment_target)) {
    // std::cout << "COLLINEAR!" << std::endl;
    return false;
  }

  // Δημιουργώ το τρίγωνο με κορυφές Source Target Point
  Triangle triangle(point, segment_source, segment_target);

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
      if(intersection_point == segment_target) continue;
      if(intersection_point == segment_source) continue;
      // !!! Δεν είμαι σίγουρος γι αυτό...να το σκεφτώ περισσότερο !!!
      // else if(intersection_point != target) return false;
      // else if(intersection_point != source) return false;

    }
    else
    {
      // Αν η τομή τους είναι ένα ευθύγραμμο τμήμα τότε...
      if (CGAL::assign(intersection_segment, result)) 
      {
        // Αν η τομή τους είναι segment που είναι το segment τότε είναι ορατό...άρα συνεχίζουμε
        if(intersection_segment == segment || intersection_segment == segment.opposite()) continue;
        
        // Αν η τομή τους είναι segment που δεν είναι το segment τότε δεν είναι ορατό...άρα επιστροφή false
        else if(intersection_segment != segment || intersection_segment == segment.opposite()) return false;
      
      // Αν δεν υπάρχει τομή τότε...συνέχισε    
      } 
      else 
        continue;
    }
  }

  return true;
}

// _____________________________________________________________________
void add_visible_edge(Point point, SegmentVector visible_polygon_edges, Polygon* polygon)
{
  Segment insert_segment =  visible_polygon_edges.back();
  for(VertexIterator itr = polygon->vertices_begin(); itr != polygon->vertices_end(); ++itr)
  {
    if(insert_segment.target() == *itr){
      polygon->insert(itr, point);
      break;
    }
  }
}

//______________________________________________________________________

// ΟΧΙ ΟΛΟΚΛΗΡΩΜΕΝΗ !!!
// Η υλοποίηση του αυξητικού αλγορίθμου
Polygon incremental_algorithm(PointVector input_points)
{
  pPointVector p_input_points = input_points.begin();
  Polygon polygon(p_input_points, p_input_points + 3);
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

    // Βρες τις κόκκινες ακμές...
    for (EdgeIterator edge_itr = convex_hull.edges_begin(); edge_itr != convex_hull.edges_end(); ++edge_itr) 
    {
      if(is_edge_visible(*p_input_points, *edge_itr, convex_hull))
      {
        red_edges.push_back(*edge_itr);
      } 
    }

    pSegmentVector p_red_edges;
    SegmentVector visible_polygon_edges;
    // Αν υπάρχουν κόκκινες ακμές στο ΚΠ δηλ. ορατές από το σημείο...
    if(red_edges.size() != 0)
    {
      
      // std::cout << red_edges.size();
      // Για κάθε μία κόκκινη ακμή βρίσκω τις ορατές ακμές στο πολύγωνο...
      for(p_red_edges = red_edges.begin(); p_red_edges != red_edges.end(); ++p_red_edges)
      {
        // Για κάθε κόκκινη πρέπει να βρώ ποιες ακμές είναι ορατές...
        // Κάθε κόκκινη θα έχει τις κορυφές της στην πολυγωνική γραμμή...
        bool flag = false;
        for(EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); ++edge_itr)
        {
          // Αν η κορυφή του πολυγώνου είναι η κορυφή της κόκκινης ακμής...
          if((*edge_itr).source() == (*p_red_edges).source()){
            flag = true;
            visible_polygon_edges.push_back(*edge_itr);
          }
          if(flag == true){
            visible_polygon_edges.push_back(*edge_itr);
          }
          if((*edge_itr).target() == (*p_red_edges).target()){
            visible_polygon_edges.push_back(*edge_itr);
            // std::cout << visible_polygon_edges.size();
            break;
          }
        }
      }
    }
    // Δεν δουλεύει ακομα...
    // Για κάθε ορατή ακμή του ΚΠ θέλω να βρώ τις ορατές ακμές στο πολύγωνο και ανάλογα με το strategy (τυχαία επιλογή, μέγιστο, ελάχστο εμβαδόν) να τις επιλέξω...
    add_visible_edge(*p_input_points, visible_polygon_edges, &polygon);
    // Συνεχίζουμε στο επόμενο σημείο
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