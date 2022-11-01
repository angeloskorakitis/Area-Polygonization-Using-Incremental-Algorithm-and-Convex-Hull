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
#include <sstream>
#include <fstream>


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
bool edge_is_visible_points_check(Point point, Segment segment, PointVector points);
bool edge_is_visible_check_all(Point point, Segment segment, Polygon polygon, PointVector points);
bool point_is_on_polygon(Point point, Polygon polygon);
Point point_closest_to_edge(Segment edge, PointVector points, Polygon polygon);
Segment edge_closest_to_point(Point point, Polygon polygon);
PointVector parse_file(std::string filename);

void add_point_to_polygon(Point point, Polygon* polygon);
void add_point_to_polygon_in_specific_edge(Point point, Segment edge, Polygon* polygon);
Segment pick_visible_edge(Point point, Polygon polygon);
Segment pick_random_edge(Polygon polygon);
Segment pick_max_area_edge(Point point, Polygon polygon);
Segment pick_min_area_edge(Point point, Polygon polygon);
bool remove_point_from_vector(PointVector* points, Point point);

void print_point(Point point);
void print_segment(Segment seg);
void print_point_vector(PointVector points);
void print_convex_hull(SegmentVector convex_hull);
void print_polygon(Polygon polygon);
void print_polygon_alt(Polygon polygon);


int edge_selection;

// Κανονικά πρέπει να επιστρέφει Polygon. Αλλά προς το παρόν το αφήνω έτσι, γιατί δεν δουλεύει σωστά το ch creation. 
Polygon convex_hull_algorithm(PointVector initial_points);

int main(int argc, char* argv[]) {

  std::string input_file = "-1";
  std::string output_file = "-1";
  std::string algorithm = "-1";
  std::string initialization = "-1";

  // Arguments count check.
  if((argc != 9) && (argc != 11)) {
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
  if(input_file == "-1") {
    std::cout << "Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
    return EXIT_FAILURE;
  }
  if(output_file == "-1") {
    std::cout << "Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
    return EXIT_FAILURE;
  }
  if(algorithm == "-1") {
    std::cout << "Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
    return EXIT_FAILURE;
  }
  if(initialization == "-1") {
    if(algorithm == "incremental") {
      std::cout << "Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
      return EXIT_FAILURE;
    }
  }
  if((edge_selection < 1) || (edge_selection > 3)) {
    std::cout << "Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
    return EXIT_FAILURE;
  }



  PointVector points = parse_file(input_file);

  print_point_vector(points);


  srand(time(0));


  // PointVector points = {
  //                       Point(10,0),
  //                       Point(0,10),
  //                       Point(1,1),
  //                       Point(8,11),
  //                       Point(3,4),
  //                       Point(0,0),
  //                       Point(6,6),
  //                       Point(18,4),
  //                       Point(16,16),
  //                       Point(10,10),
  //                       Point(2,6) 
  // };
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

  print_polygon_alt(polygon);





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

void print_polygon_alt(Polygon polygon) {
  for(VertexIterator itr = polygon.begin(); itr != polygon.end(); itr++) {
    print_point(*itr);
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


// Checking visibility regarding only the polygon edges.
bool edge_is_visible(Point point, Segment segment, Polygon polygon) {

  // std::cout << "Checking visibility from point " << point << " to segment " << segment << std::endl;

  Point segment_source = segment.source();
  Point segment_target = segment.target();

  // Τι θα κάνουμε με τα συνευθειακά σημεία; Προς το παρόν τα συμπεριφέρομαι ως not visible.
  if(CGAL::collinear(point, segment_source, segment_target)) {
    // std::cout << "COLLINEAR!" << std::endl;
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

      // std::cout << "INTERSECTION POINT " << intersection_point << std::endl;
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
        // std::cout << "INTERSECTION SEGMENT " << intersection_segment << std::endl;
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


// Checking visibility regarding only for the inner points.
bool edge_is_visible_points_check(Point point, Segment segment, PointVector points) {

  // std::cout << "Checking visibility from point " << point << " to segment " << segment << std::endl;
  Point segment_source = segment.source();
  Point segment_target = segment.target();

  // Τι θα κάνουμε με τα συνευθειακά σημεία; Προς το παρόν τα συμπεριφέρομαι ως not visible.
  if(CGAL::collinear(point, segment_source, segment_target)) {
    // std::cout << "COLLINEAR!" << std::endl;
    return false;
  }

  Triangle triangle(segment_source, segment_target, point);

  for (int i = 0; i < points.size(); i++) {

    Point checking_point = points[i];
    Point intersection_point;

    Object result = CGAL::intersection(triangle, checking_point);

    // Αν η τομή τους είναι ένα σημείο...
    if (CGAL::assign(intersection_point, result)) {

      if(intersection_point == segment_target) continue;
      if(intersection_point == segment_source) continue;
      if(intersection_point == point) continue;

      return false;
    }
    else {
      continue;
    }
  }

  return true;
}

// Checking visibility regarding both the edges of the polygon and the inner points.
bool edge_is_visible_check_all(Point point, Segment segment, Polygon polygon, PointVector points) {
  if(!edge_is_visible(point, segment, polygon)) return false;
  if(!edge_is_visible_points_check(point, segment, points)) return false;
  return true;
}


Segment pick_visible_edge(Point point, Polygon polygon) {
  // Εδώ θα πρέπει να γίνεται επιλογή ανάλογα με το τι έχει δώσει ο χρήστης, μεγιστοποίηση/ελαχιστοποίηση εμβαδού κλπ. 
  Segment edge;

  // // Επιλέγω μία τυχαία ορατή ακμή. 
  // while(!edge_is_visible(point, edge = pick_random_edge(polygon), polygon));

  // Επιλέγω την ορατή ακμή που φτιάχνει το μεγαλύτερο τρίγωνο με το σημείο.
  edge = pick_max_area_edge(point, polygon);

  // // Επιλέγω την ορατή ακμή που φτιάχνει το μικρότερο τρίγωνο με το σημείο. 
  // edge = pick_min_area_edge(point, polygon);

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
    (*polygon).insert((*polygon).begin() + counter, point);
  }


}


// Point gets added to polygon. The edge of polygon gets erased and
// two new edges previous_edge.source() to point and previous_edge.target() to point are added.
void add_point_to_polygon_in_specific_edge(Point point, Segment edge, Polygon* polygon) {

  // If point was added to the polygon on a previous iteration "by accident",
  // just continue to next inner point.
  if(!point_is_on_polygon(point, *polygon)) {

    int counter = 1;
    for(EdgeIterator edge_itr = (*polygon).edges_begin(); edge_itr != (*polygon).edges_end(); edge_itr++) {
      if((edge == *edge_itr) || (edge.opposite() == *edge_itr)) break;

      counter++;
    }

    std::cout << "ΚΡΙΣΙΜΟ ΣΗΜΕΙΟ 2. " << counter << " ~~~ " << edge << std::endl;
    // Δεν δουλεύει ακόμα!
    (*polygon).insert((*polygon).begin() + counter, point);
  }


}

// Returns random edge of polygon.
Segment pick_random_edge(Polygon polygon) {
  int r = rand() % polygon.size();
  return *(polygon.edges_begin() + r);
}


// Returns the -visible from point- edge of the polygon, that creates the largest (area-wise) triangle with the given point.
Segment pick_max_area_edge(Point point, Polygon polygon) {
  Segment max_segment;
  double max_area = -1;
  double current_area;

  for(EdgeIterator edge = polygon.edges_begin(); edge != polygon.edges_end(); edge++) {


    // If the edge is not visible, skip the edge. 
   if(!edge_is_visible(point, *edge, polygon)) continue;


    Triangle triangle((*edge).source(), (*edge).target(), point);
    // area() function may return negative number.
    current_area = CGAL::abs(triangle.area());

    if(current_area > max_area) {
      max_segment = *edge;
      max_area = current_area;
    }

  }


  return max_segment;
}


// Returns the -visible from point- edge of the polygon, that creates the smallest (area-wise) triangle with the given point.
Segment pick_min_area_edge(Point point, Polygon polygon) {
  Segment min_segment;
  double min_area;
  double current_area;

  for(EdgeIterator edge = polygon.edges_begin(); edge != polygon.edges_end(); edge++) {

    // If the edge is not visible, skip the edge. 
    if(!edge_is_visible(point, *edge, polygon)) continue;

    Triangle triangle((*edge).source(), (*edge).target(), point);
    // area() function may return negative number.
    current_area = CGAL::abs(triangle.area());

    if(edge == polygon.edges_begin()) {
      min_area = current_area;
      min_segment = *edge;
    }

    if(current_area < min_area) {
      min_segment = *edge;
      min_area = current_area;
    }

  }

  return min_segment;
}


// Finds from a set of points the one closest to edge.
// There must be visibility from point to edge.
// If there is no visible point from edge, return point (-1, -1)
Point point_closest_to_edge(Segment edge, PointVector points, Polygon polygon) {

  double min_distance = -1, current_distance;
  Point closest_point(-1,-1);

  for(Point point : points) {


    if(point_is_on_polygon(point, polygon)) continue;
    if(!edge_is_visible_check_all(point, edge, polygon, points)) continue;

    current_distance = CGAL::squared_distance(point, edge);
    
    if(min_distance == -1) {
      closest_point = point;
      min_distance = current_distance;
    }
    if (current_distance < min_distance) {
      closest_point = point;
      min_distance = current_distance;
    }
  }

  return closest_point;
}


// Finds from a polygon the edge closest to point.
// There must be visibility from point to edge.
Segment edge_closest_to_point(Point point, Polygon polygon, PointVector points) {
  double min_distance, current_distance;
  Segment closest_edge;

  for(EdgeIterator edge = polygon.edges_begin(); edge != polygon.edges_end(); edge++) {

    if(!edge_is_visible_check_all(point, *edge, polygon, points)) continue;

    current_distance = CGAL::squared_distance(point, *edge);

    if(edge == polygon.edges_begin()) {
      closest_edge = *edge;
      min_distance = current_distance;
    }
    else if (current_distance < min_distance) {
      closest_edge = *edge;
      min_distance = current_distance;
    }
  }

  return closest_edge;

}



// Removes point from a set of points. 
// True on success, false otherwise. 
bool remove_point_from_vector(PointVector* points, Point point) {
  for(int i = 0; i < (*points).size(); i++) {
    if((*points)[i] == point) {
      (*points)[i] = (*points)[(*points).size() - 1];
      (*points).pop_back();
      return true;
    }
  }

  return false;
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


  Point wrong_point(-1, -1);

  // While there are inner points.
  while(inner_points.size() > 0) {

    Segment edge;
    Point point;
    
    // Pick a random edge and find the closest point to it.
    do {
      edge = pick_random_edge(polygon);
      point = point_closest_to_edge(edge, inner_points, polygon);
    } while(point == wrong_point); 

    add_point_to_polygon_in_specific_edge(point, edge, &polygon);
    remove_point_from_vector(&inner_points, point);
         
  }

  return polygon;
}


PointVector parse_file(std::string filename) {
  std::ifstream infile(filename);

  std::string line;
  std::getline(infile, line);
  std::cout << line;
  std::getline(infile, line);
  std::cout << line;

PointVector points;

  while (std::getline(infile, line))
  {
    std::istringstream iss(line);
    int count, x, y;
    if (!(iss >> count >> x >> y)) { break; } // error

        std::cout << "Count = " << count << " x = " << x << " y = " << y << std::endl;
        Point point(x, y);
        points.push_back(point);
  }

return points;

}