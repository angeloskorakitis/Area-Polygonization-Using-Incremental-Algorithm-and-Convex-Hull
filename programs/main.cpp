#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>
// #include <CGAL/draw_polygon_2.h>
#include <CGAL/intersections.h>
#include <vector>
#include <numeric>
#include <string>
#include <iostream>
#include <time.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel   K;
typedef K::Point_2                                            Point;
typedef CGAL::Segment_2<K>                                    Segment;
typedef CGAL::Object                                          Object;   
typedef CGAL::Polygon_2<K>                                    Polygon;
typedef CGAL::Triangle_2<K>                                   Triangle;
typedef CGAL::Convex_hull_traits_adapter_2<K,CGAL::Pointer_property_map<Point>::type > Convex_hull_traits_2;
typedef Polygon::Vertex_iterator                              VertexIterator;
typedef Polygon::Edge_const_iterator                          EdgeIterator;
typedef std::vector<Point>                                    PointVector;
typedef std::vector<Segment>                                  SegmentVector;
typedef PointVector::iterator                                 pPointVector;
typedef SegmentVector::iterator                               pSegmentVector;
typedef std::string                                           String;

Polygon convex_hull_create(PointVector points);
bool segment_intersects_convex_hull(Segment seg, SegmentVector convex_hull, Segment excl);
bool edge_is_visible(Point p, Segment seg, Polygon polygon);
bool point_is_on_polygon(Point point, Polygon polygon);

void add_point_to_polygon(Point point, Polygon* polygon);
Segment pick_visible_edge(Point point, Polygon polygon);
Segment pick_random_edge(Polygon polygon);

void print_point(Point point);
void print_segment(Segment seg);
void print_point_vector(PointVector points);
void print_convex_hull(SegmentVector convex_hull);
void print_polygon(Polygon polygon);

// Κανονικά πρέπει να επιστρέφει Polygon. Αλλά προς το παρόν το αφήνω έτσι, γιατί δεν δουλεύει σωστά το ch creation. 
Polygon convex_hull_algorithm(PointVector initial_points);

int main()
{

  srand(time(0));
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
  // PointVector points = {
  //                       Point(0,0),
  //                       Point(10,10),
  //                       Point(3,7),
  //                       Point(3,2),
  //                       Point(7,2),
  //                       Point(10,0),
  //                       Point(0,10),
  // };
  // PointVector points = {
  //                       Point(0,0),
  //                       Point(2,2),
  //                       Point(4,6),
  //                       Point(4,0),
  //                       Point(2,4),
  //                       Point(0,6),
  // };

  Polygon polygon = convex_hull_algorithm(points);

  print_polygon(polygon);


  return EXIT_SUCCESS;
}
  
// Checks if a given segment seg intersects with the convex_hull. A segment of the hull excl can be excluded from checking.
// true if they intersect, false if they do not intersect.
bool segment_intersects_convex_hull(Segment seg, SegmentVector convex_hull, Segment excl) {
  for(Segment to_check : convex_hull) {

    // excl is excluded from checking.
    if((to_check == excl) || (to_check == excl.opposite() )) continue;

    if(CGAL::do_intersect(seg, to_check)) return true;
  }

  return false;
}





// Όπως αποδείχθηκε, αυτές οι print είναι παντελώς άχρηστες γιατί απ' ό,τι φαίνεται μπορείς να κάνεις απλά cout τα αντικείμενα της CGAL.
// Δεν τις σβήνω, ωστόσο, μου φαίνεται πιο εύκολο για debug να γράφω print_τάδε, παρά std::cout << μπλα μπλα << std::endl.
void print_point(Point point) {
  std::cout << "(" << point.x() << ", " << point.y() << ")" << std::endl;
}

void print_segment(Segment seg) {
  std::cout << "(" << seg.source().x() << ", " << seg.source().y() << ") - (" << seg.target().x() << ", " << seg.target().y() << ")" << std::endl;
}

void print_point_vector(PointVector points) {
  for(Point point : points) {
    print_point(point);
  }
}

void print_convex_hull(SegmentVector convex_hull) {
  for(Segment seg : convex_hull) {
    print_segment(seg);
  }
}

void print_polygon(Polygon polygon) {
  for(EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); edge_itr++) {
    print_segment(*edge_itr);
  }
}



// This is a bit stupid, I'm sure there's a better way.
// Returns true if a point is on the perimeter of a polygon.
bool point_is_on_polygon(Point point, Polygon polygon) {
  // Check all segments of polygon, if point is on one of them then the point is on the polygon.
  for(EdgeIterator edge = polygon.edges_begin(); edge != polygon.edges_end(); edge++) {
    if(CGAL::do_intersect(point, *edge)) return true;
  }

  return false;
}


// This is temporary, till Convex Hull creation works and Polygon is returned. 
Polygon convex_hull_create(PointVector points) {

  std::vector<std::size_t> indices(points.size()), out;
  std::iota(indices.begin(), indices.end(), 0);
  CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
                      Convex_hull_traits_2(CGAL::make_property_map(points)));

  PointVector convex_hull_points;
  for(int i = 0; i < out.size(); i++) {
    convex_hull_points.push_back(points[out[i]]);
  }

  Polygon convex_hull(convex_hull_points.begin(), convex_hull_points.end());

  return convex_hull;
}


bool edge_is_visible(Point point, Segment segment, Polygon polygon) {

  std::cout << "Checking visibility from point " << point << " to segment " << segment << std::endl;

  Point segment_source = segment.source();
  Point segment_target = segment.target();

  // Τι θα κάνουμε με τα συνευθειακά σημεία; Προς το παρόν τα συμπεριφέρομαι ως not visible.
  if(CGAL::collinear(point, segment_source, segment_target)) {
    std::cout << "COLLINEAR!" << std::endl;
    return false;
  }




  Triangle triangle(segment_source, segment_target, point);

  for (EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); ++edge_itr) 
  {
    Point intersection_point;
    Segment intersection_segment;

    Object result = CGAL::intersection(triangle, *edge_itr);

    // Αν η τομή τους είναι ένα σημείο...
    if (CGAL::assign(intersection_point, result)) 
    {

      std::cout << "INTERSECTION POINT " << intersection_point << std::endl;
      if(intersection_point == segment_target) continue;
      if(intersection_point == segment_source) continue;
      

      // Με γρήγορη σκέψη, δεν μπορεί κάποια ακμή του πολυγώνου να κρύβει ορατότητα
      // κάποιας άλλης ακμής μόνο με ένα σημείο της, αλλά μόνο με segment της.
      // Οπότε δεν έχει νόημα ελέγξω κάτι άλλο.


      // ΩΣΤΟΣΟΟΟ. ΓΙΑ ΤΟΝ ΑΛΓΟΡΙΘΜΟ ΠΟΥ ΠΑΩ ΝΑ ΥΛΟΠΟΙΗΣΩ.
      // ΑΝ ΒΡΕΘΩ ΣΕ ΑΥΤΟ ΑΚΡΙΒΩΣ ΤΟ ΣΗΜΕΙΟ ΤΟΥ ΚΩΔΙΚΑ.
      // Αυτή σημαίνει ότι το intersection point είναι ένα από τα άλλα inner σημεία.
      // Θα είχε ενδιαφέρον αν συζητήσουμε τι γίνεται σε αυτήν την περίπτωση. 

      // Επίσης, τι γίνεται αν πολλά inner σημεία βρίσκονται μέσα στο τρίγωνο; 
      // Και δεν έχω ούτε intersection point ούτε intersection segment
      // αλλά έχω intersection σύνολο σημείων...;

    }
    else
    {
      // Αν η τομή τους είναι ένα ευθύγραμμο τμήμα τότε...
      if (CGAL::assign(intersection_segment, result)) 
      {
        std::cout << "INTERSECTION SEGMENT " << intersection_segment << std::endl;
        // Αν η τομή τους είναι segment που είναι το segment τότε είναι ορατό...άρα συνεχίζουμε
        if((intersection_segment == segment) || (intersection_segment == segment.opposite())) continue;
        
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

Segment pick_visible_edge(Point point, Polygon polygon) {
  // Εδώ θα πρέπει να γίνεται επιλογή ανάλογα με το τι έχει δώσει ο χρήστης, μεγιστοποίηση/ελαχιστοποίηση εμβαδού κλπ. 

  // Επιλέγω μία τυχαία ορατή ακμή. 
  Segment edge;
  while(!edge_is_visible(point, edge = pick_random_edge(polygon), polygon));

  return edge;
  // Γίνεται να φτάσω στο τέλος χωρίς να έχω ορατή ακμή; Οέο;
}


void add_point_to_polygon(Point point, Polygon* polygon) {

  // If point was added to the polygon on a previous iteration "by accident",
  // just continue to next inner point.
  if(!point_is_on_polygon(point, *polygon)) {


    Segment edge = pick_visible_edge(point, *polygon);

    int counter = 1;
    for(EdgeIterator edge_itr = (*polygon).edges_begin(); edge_itr != (*polygon).edges_end(); edge_itr++) {
      if((edge == *edge_itr) || (edge.opposite() == *edge_itr)) break;

      counter++;
    }

    std::cout << "ΚΡΙΣΙΜΟ ΣΗΜΕΙΟ. " << counter << " ~~~ " << edge << std::endl;
    // Δεν δουλεύει ακόμα!
    (*polygon).insert((*polygon).begin() + counter, point);
  }


}


Segment pick_random_edge(Polygon polygon) {
  int r = rand() % polygon.size();
  return *(polygon.edges_begin() + r);
}




// Should return Polygon. 
Polygon convex_hull_algorithm(PointVector input_points) {

  // Polygon initally is the convex hull.
  Polygon polygon = convex_hull_create(input_points);

  // Creating inner_points vector. A vector with all inner points. 
  PointVector inner_points;
  for(Point point : input_points) {
    if(!point_is_on_polygon(point, polygon)) {
      inner_points.push_back(point);
    }
  }

  // Juuuust checkin.
  std::cout << "Inner points are: " << std::endl;
  print_point_vector(inner_points);

  for(Point point : inner_points) {
    add_point_to_polygon(point, &polygon);
  }


  return polygon;
}
