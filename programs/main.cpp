#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>
#include <CGAL/intersections.h>
#include <vector>
#include <numeric>
#include <string>
#include <iostream>
#include <time.h>
#include <sstream>
#include <fstream>
#include <chrono>

enum { RANDOM = 1, MIN_AREA = 2, MAX_AREA = 3};

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


// Struct to improve time performance.
struct tuple {
  Point point;
  int position;
};

typedef struct tuple* Tuple;



// Helpful Functions.
bool edge_is_visible_edges(Point p, Segment seg, Polygon polygon);
bool edge_is_visible_points(Point point, Segment segment, PointVector points);
bool edge_is_visible(Point point, Segment segment, Polygon polygon, PointVector points);
bool point_is_on_polygon(Point point, Polygon polygon);
Point point_closest_to_edge(Segment edge, PointVector points, Polygon polygon);
bool remove_point_from_vector(PointVector* points, Point point);

// File Management Functions.
PointVector parse_file(std::string filename);
void print_output(Polygon polygon, PointVector points, std::string output_file, std::string algorithm, int edge_selection, std::string initialization, auto duration);


// Convex Hull.
Polygon convex_hull_algorithm(PointVector initial_points, int edge_selection);
Polygon convex_hull_create(PointVector points);
void add_point_to_polygon(Point point, Segment edge, Polygon* polygon);
void add_point_to_polygon(Polygon* polygon, Tuple tuple);
Segment pick_random_edge(Polygon polygon);
Tuple pick_max_area_edge(Polygon polygon, PointVector points);
Tuple pick_min_area_edge(Polygon polygon, PointVector points);

// Print Functions. 
void print_point(Point point);
void print_segment(Segment seg);
void print_point_vector(PointVector points);
void print_convex_hull(SegmentVector convex_hull);
void print_polygon(Polygon polygon);
void print_polygon_alt(Polygon polygon);



int main(int argc, char* argv[]) {

  std::string input_file = "-1";
  std::string output_file = "-1";
  std::string algorithm = "-1";
  std::string initialization = "-1";
  int edge_selection = -1;

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


  // Turn input file into points vector.
  PointVector points = parse_file(input_file);

  srand(time(0));

  Polygon polygon;

  // Starting timer.
  auto start = std::chrono::high_resolution_clock::now();


  // Calling the algorihm function.
  if(algorithm == "incremental") {
    // Εδώ θα βάλεις τη δική σου συνάρτηση μπροκολοκολίνο.
  }
  else if(algorithm == "convex_hull") {
    polygon = convex_hull_algorithm(points, edge_selection);
  }
  else if(algorithm == "onion") {
    std::cout << "Not implemented." << std::endl;
    return EXIT_FAILURE;
  }
  else {
    std::cout << "Use a valid algorithm name!" << std::endl;
    std::cout << "Usage: ./to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull> -edge_selection <1 or 2 or 3> -initialization <1a or 1b or 2a or 2b | μόνο στον αυξητικό αλγόριθμο> " << std::endl;
    return EXIT_FAILURE;
  }
  
    
  // Stopping timer and calculation duration.
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  // Write output in output file.
  print_output(polygon, points, output_file, algorithm, edge_selection, initialization, duration);

  
  // If the polygon is not simple then failure...
  if(!polygon.is_simple()) {
    std::cout << "THIS POLYGON IS NOT SIMPLE!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
  



































////////////////////////////// Print Functions //////////////////////////////


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




























////////////////////////////// Helpful Functions //////////////////////////////


// Returns true if a point is on the perimeter of a polygon.
bool point_is_on_polygon(Point point, Polygon polygon) {

  if(CGAL::bounded_side_2(polygon.begin(), polygon.end(), point) == CGAL::ON_BOUNDARY) {
    return true;
  }

  return false;
}


// Checking visibility regarding only the polygon edges.
bool edge_is_visible_edges(Point point, Segment segment, Polygon polygon) {


  Point segment_source = segment.source();
  Point segment_target = segment.target();

  // Regarding collinear points as not visible.
  if(CGAL::collinear(point, segment_source, segment_target)) {
    return false;
  }


  Triangle triangle(segment_source, segment_target, point);

  for (EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); ++edge_itr) 
  {
    Point intersection_point;
    Segment intersection_segment;

    Object result = CGAL::intersection(triangle, *edge_itr);

    // The intersection is a point.
    if (CGAL::assign(intersection_point, result)) 
    {
      if(intersection_point == segment_target) continue;
      if(intersection_point == segment_source) continue;
    }
    else
    {
      // The intersection is a segment.
      if (CGAL::assign(intersection_segment, result)) 
      {
        // Not regarding the segment we are checking it's visibility.
        if((intersection_segment == segment) || (intersection_segment == segment.opposite())) continue;
        
        // There is an intersection with another segment, there is no visivbility.
        else if(intersection_segment != segment) return false;
      
      } 
      // No intersection.
      else 
        continue;
    }
  }

  // If we reached here, the segment is visible from point, regarding the segments of the polygon. 
  return true;
}


// Checking visibility regarding only for the inner points.
bool edge_is_visible_points(Point point, Segment segment, PointVector points) {

  Point segment_source = segment.source();
  Point segment_target = segment.target();

  // Regarding collinear points as not visible.
  if(CGAL::collinear(point, segment_source, segment_target)) {
    return false;
  }

  Triangle triangle(segment_source, segment_target, point);

  for (int i = 0; i < points.size(); i++) {

    Point checking_point = points[i];
    Point intersection_point;

    Object result = CGAL::intersection(triangle, checking_point);

    // There is an intersection.
    if (CGAL::assign(intersection_point, result)) {

      // Not regarding the points we are checking.
      if(intersection_point == segment_target) continue;
      if(intersection_point == segment_source) continue;
      if(intersection_point == point) continue;

      // One of the inner points is blocking visibility.
      return false;
    }
    // No intersection.
    else {
      continue;
    }
  }

  // If we reached here, the segment is visible from point, regarding the inner points.
  return true;
}


// Checking visibility regarding both the edges of the polygon and the inner points.
bool edge_is_visible(Point point, Segment segment, Polygon polygon, PointVector points) {
  if(!edge_is_visible_edges(point, segment, polygon)) return false;
  if(!edge_is_visible_points(point, segment, points)) return false;
  return true;
}



// Finds from a set of points the one closest to edge.
// There must be visibility from point to edge.
// If there is no visible point from edge, return point (-1, -1)
Point point_closest_to_edge(Segment edge, PointVector points, Polygon polygon) {

  double min_distance = -1, current_distance;
  Point closest_point(-1,-1);

  // Traverse all points. 
  for(Point point : points) {

    // Must not be on the polygon and must be visible.
    if(point_is_on_polygon(point, polygon)) continue;
    if(!edge_is_visible(point, edge, polygon, points)) continue;


    current_distance = CGAL::squared_distance(point, edge);

    // The first visible and not-on-the-polygon point.     
    if(min_distance == -1) {
      closest_point = point;
      min_distance = current_distance;
    }

    // Searching for the one with the lowest distance...
    if (current_distance < min_distance) {
      closest_point = point;
      min_distance = current_distance;
    }
  }

  return closest_point;
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
























////////////////////////////// File Management Functions //////////////////////////////


// Input file function. Turns the input file into a point vector. 
PointVector parse_file(std::string filename) {
  std::ifstream infile(filename);

  std::string line;
  std::getline(infile, line);
  std::getline(infile, line);

  PointVector points;

  while (std::getline(infile, line))
  {
    std::istringstream iss(line);
    int count, x, y;
    if (!(iss >> count >> x >> y)) { break; } 

        Point point(x, y);
        points.push_back(point);
  }

return points;

}

// Output file function. Writes the results into the output file. 
void print_output(Polygon polygon, PointVector points, std::string filename, std::string algorithm, int edge_selection, std::string initialization, auto duration) {
  std::ofstream outfile(filename);

  if(outfile.is_open()) {
    outfile << "Polygonization" << std::endl;

    for(Point point : points) {
      outfile << point << std::endl;
    }

    for(EdgeIterator edge = polygon.edges_begin(); edge != polygon.edges_end(); edge++) {
      outfile << *edge << std::endl;
    }

    if(algorithm == "incremental") {
      outfile << "Algorithm:" << algorithm << "_edge_selection" << edge_selection << "_initialization" << initialization << std::endl;
    }
    else {
      outfile << "Algorithm:" << algorithm << "_edge_selection" << edge_selection << std::endl;
    }

    double polygon_area = CGAL::abs(polygon.area());
    
    Polygon convex_hull;
    CGAL::convex_hull_2(polygon.begin(), polygon.end(), std::back_inserter(convex_hull));

    double convex_hull_area = CGAL::abs(convex_hull.area());

    double ratio = polygon_area / convex_hull_area;

    outfile << "area: " << polygon_area << std::endl;
    outfile << "ratio: " <<  ratio << std::endl;
    outfile << "construction time: " << duration.count() << std::endl;
  }
}


























////////////////////////////// Pick Edge Functions //////////////////////////////

// Returns random edge of polygon.
Segment pick_random_edge(Polygon polygon) {
  int r = rand() % polygon.size();
  return *(polygon.edges_begin() + r);
}


// Finds the edge of the polygon, that creates the largest (area-wise) triangle with the closest visible point to it.
// Returns the tuple with the edge and the point.
Tuple pick_max_area_edge(Polygon polygon, PointVector points) {
  
  // Max.
  Segment max_segment;
  Point point;
  double max_area = -1;
  int position = 0;

  // Current
  Point current_point;
  double current_area;
  int current_position = 0;

  // Itarate all edges.
  for(EdgeIterator edge = polygon.edges_begin(); edge != polygon.edges_end(); edge++) {

    current_position++;
    
    current_point = point_closest_to_edge(*edge, points, polygon);

    // If the point is (-1, -1), then the current edge is not visible from any inner point.
    Point not_visible(-1, -1);
    if(current_point == not_visible) {
      continue;
    }


    Triangle triangle((*edge).source(), (*edge).target(), current_point);
    // area() function may return negative number.
    current_area = CGAL::abs(triangle.area());


    // Searching for max.
    if(current_area > max_area) {
      max_segment = *edge;
      max_area = current_area;
      point = current_point;
      position = current_position;
    }


  }

  // Create and return tuple. 
  Tuple tuple = new struct tuple;
  tuple->point = point;
  tuple->position = position;

  return tuple;
}


// Returns the edge of the polygon, that creates the smallest (area-wise) triangle with the closest visible point to it.
Tuple pick_min_area_edge(Polygon polygon, PointVector points) {

  // Initiating dull segment to check the first visible edge.
  Point dull1(-1, -1);
  Point dull2(-2, -2);
  Segment dull_segment(dull1, dull2);

  // Min.
  Segment min_segment(dull1, dull2);
  Point point;
  double min_area;
  int position = 0;
  
  // Current.
  Point current_point;
  double current_area;
  int current_position = 0;


  // Iterating all polygon edges, finding the point closest to it. 
  // Checking the triangle areas and finding the smallest one. 
  for(EdgeIterator edge = polygon.edges_begin(); edge != polygon.edges_end(); edge++) {
    current_position++;

    current_point = point_closest_to_edge(*edge, points, polygon);
    

    // If the point is (-1, -1), then the current edge is not visible from any inner point.
    Point not_visible(-1, -1);
    if(current_point == not_visible) {
      continue;
    }


    Triangle triangle((*edge).source(), (*edge).target(), current_point);
    // area() function may return negative number.
    current_area = CGAL::abs(triangle.area());


    // If this is the first visible edge found till now.
    if(min_segment == dull_segment) {
      min_segment = *edge;
      min_area = current_area;
      position = current_position;
      point = current_point;
    }


    // Searching for min...
    if(current_area < min_area) {
      min_segment = *edge;
      min_area = current_area;
      position = current_position;
      point = current_point;
    }

  }



  // Create and return tuple. 
  Tuple tuple = new struct tuple;
  tuple->point = point;
  tuple->position = position;

  return tuple;

}


































////////////////////////////// Convex Hull //////////////////////////////

// Returns the convex hull from a set of points.
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



// Point gets added to polygon. The edge of polygon gets erased and
// two new edges previous_edge.source() to point and previous_edge.target() to point are added.
void add_point_to_polygon(Point point, Segment edge, Polygon* polygon) {

  // If point was added to the polygon on a previous iteration "by accident",
  // Just do nothing.
  if(!point_is_on_polygon(point, *polygon)) {

    int counter = 1;
    for(EdgeIterator edge_itr = (*polygon).edges_begin(); edge_itr != (*polygon).edges_end(); edge_itr++) {
      if((edge == *edge_itr) || (edge.opposite() == *edge_itr)) break;

      counter++;
    }

    // std::cout << "ABOUT TO ADD POINT = " << point << " ON EDGE = " << edge << std::endl;
    (*polygon).insert((*polygon).begin() + counter, point);
  }

}



// Point gets added to polygon. The edge of polygon gets erased and
// two new edges previous_edge.source() to point and previous_edge.target() to point are added.
void add_point_to_polygon(Polygon* polygon, Tuple tuple) {

  // If point was added to the polygon on a previous iteration "by accident",
  // just continue to next inner point.
  if(!point_is_on_polygon(tuple->point, *polygon)) {

    (*polygon).insert((*polygon).begin() + tuple->position, tuple->point);
  }


}


// The convex hull algorithm function. 
Polygon convex_hull_algorithm(PointVector input_points, int edge_selection) {

  // Polygon initally is the convex hull.
  Polygon polygon = convex_hull_create(input_points);

  // Creating inner_points vector. A vector with all inner points. 
  PointVector inner_points;
  for(Point point : input_points) {
    if(!point_is_on_polygon(point, polygon)) {
      inner_points.push_back(point);
    }
  }


  Point not_visible_point(-1, -1);

  // While there are inner points.
  while(inner_points.size() > 0) {

    Tuple tuple;
    Segment edge;
    Point point;
    

    // Pick an edge and find the closest point to it.
    do {
      if(edge_selection == RANDOM) {
        edge = pick_random_edge(polygon);
        point = point_closest_to_edge(edge, inner_points, polygon);
      }
      else if(edge_selection == MIN_AREA) {
        tuple = pick_max_area_edge(polygon, inner_points);
        point = tuple->point;
      }
      else if(edge_selection == MAX_AREA) {
        tuple = pick_min_area_edge(polygon, inner_points);
        point = tuple->point;
      }


    // Keep trying untill you find an edge with a visible point.
    // (There certainly must be one)
    } while(point == not_visible_point); 


    // Add the point to the polygon and remove it from the inner points.
    if(edge_selection == RANDOM) {
      add_point_to_polygon(point, edge, &polygon);
      remove_point_from_vector(&inner_points, point);
    }
    else {
      add_point_to_polygon(&polygon, tuple);
      remove_point_from_vector(&inner_points, tuple->point);

      delete tuple;
    }
  }

  return polygon;
}



