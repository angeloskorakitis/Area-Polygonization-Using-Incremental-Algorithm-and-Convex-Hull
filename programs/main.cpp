#include <CGAL/convex_hull_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Simple_Cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <vector>
#include <numeric>
#include <string>

typedef CGAL::Epick   K;
typedef K::Point_2                                            Point;
typedef CGAL::Segment_2<K>                                    Segment;
typedef CGAL::Object                                          Object;   
typedef CGAL::Polygon_2<K>                                    Polygon;
typedef CGAL::Triangle_2<K>                                   Triangle;
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
SegmentVector find_red_edges(Point, Polygon );
SegmentVector find_visible_edges(SegmentVector, Polygon);
void add_visible_edge(Point , SegmentVector, Polygon* );
Polygon incremental_algorithm(PointVector );
void print_point(Point);
void print_segment(Segment);
void print_polygon(Polygon);


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
  std::sort(points.begin(), points.end());

  // Poligonization
  Polygon polygon = incremental_algorithm(points);

  // Print polygon
  print_polygon(polygon);

  // If the polygon is not simple then failure...
  if(!polygon.is_simple()) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}


bool is_edge_visible(Point point, Segment segment, Polygon convex_hull)
{
  Point segment_source = segment.source();
  Point segment_target = segment.target();

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


void add_visible_edge(Point point, SegmentVector visible_polygon_edges, Polygon* polygon)
{
  //
  //
  // Παίρνει την πρώτη γραμμή και την εισάγει...πρέπει να το υλοποιήσω σύμφωνα με τον σχεδιασμό
  //
  //
  Segment insert_segment =  visible_polygon_edges.back();
  for(VertexIterator itr = polygon->vertices_begin(); itr != polygon->vertices_end(); ++itr)
  {
    if(insert_segment.target() == *itr){
      polygon->insert(itr, point);
      break;
    }
  }
}

// Returns a vector of Segments with the red edges of the CH
SegmentVector find_red_edges(Point point, Polygon polygon)
{
  Polygon convex_hull;
  // Υπολογισμός του ΚΠ
  CGAL::convex_hull_2(polygon.begin(), polygon.end(), std::back_inserter(convex_hull));

  // Vector που θα περιέχει τις κόκκινες ακμές
  SegmentVector red_edges;

  // Βρες τις κόκκινες ακμές...
  for (EdgeIterator edge_itr = convex_hull.edges_begin(); edge_itr != convex_hull.edges_end(); ++edge_itr) 
  {
    if(is_edge_visible(point, *edge_itr, convex_hull))
    {
      red_edges.push_back(*edge_itr);
    } 
  }

  return red_edges;
}

// Returns a vector of Segments with the visible edges of the Polygon
SegmentVector find_visible_edges(SegmentVector red_edges, Polygon polygon)
{
  SegmentVector visible_edges;
  // Αν υπάρχουν κόκκινες ακμές στο ΚΠ δηλ. ορατές από το σημείο...
  if(red_edges.size() != 0)
  {      
    // Για κάθε μία κόκκινη ακμή βρίσκω τις ορατές ακμές στο πολύγωνο...
    for(pSegmentVector p_red_edges = red_edges.begin(); p_red_edges != red_edges.end(); ++p_red_edges)
    {
      // Για κάθε κόκκινη πρέπει να βρώ ποιες ακμές είναι ορατές...
      // Κάθε κόκκινη θα έχει τις κορυφές της στην πολυγωνική γραμμή...
      bool flag = false;
      for(EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); ++edge_itr)
      {
        // Αν η κορυφή του πολυγώνου είναι η κορυφή της κόκκινης ακμής...
        if((*edge_itr).source() == (*p_red_edges).source()){
          flag = true;
          visible_edges.push_back(*edge_itr);
        }
        if(flag == true){
          visible_edges.push_back(*edge_itr);
        }
        if((*edge_itr).target() == (*p_red_edges).target()){
          visible_edges.push_back(*edge_itr);
          break;
        }
      }
    }
  }
  return visible_edges;
}


// Implementation of the incremental algorithm
Polygon incremental_algorithm(PointVector input_points)
{
  pPointVector p_input_points = input_points.begin();

  // Start with the first 3 points
  Polygon polygon(p_input_points, p_input_points + 3);

  // SPECIAL CASE: Check if the first 3 points are collinear...add a 4th

  advance(p_input_points, 3);

  // While the number of the input points is different of the size of the polygon, repeat...
  while(input_points.size() != polygon.size())
  {
    // Find the red edges of the CH, i.e the visible from the CH
    SegmentVector red_edges = find_red_edges(*p_input_points, polygon);

    // Find the visible edges of the Polygon
    SegmentVector visible_edges = find_visible_edges(red_edges, polygon);
    
    // For every visible edge, insert a vertex in the polygon according to the strategy (random, min/ max area)
    add_visible_edge(*p_input_points, visible_edges, &polygon);
    
    // Continue with the next point...
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