/////////////////////////////////////////////////////////////////////////////////
//
//
// Implementation of Polygonization Using the Incremental Algorithm 
//
//
/////////////////////////////////////////////////////////////////////////////////

#include "Polygonization_Using_Incremental_Algorithm.hpp"

bool x_increasing(const Point a, const Point b){
    return a.x() <= b.x(); 
}

bool x_decreasing(const Point a, const Point b){
    return a.x() >= b.x(); 
}

bool y_increasing(const Point a, const Point b){
    return a.y() <= b.y(); 
}

bool y_decreasing(const Point a, const Point b){
    return a.y() >= b.y(); 
}

void sort_input_points(PointVector* input_points, String initialization)
{
  if(initialization == "1a")
    std::sort(input_points->begin(), input_points->end(), x_decreasing);
  else if(initialization == "1b")
    std::sort(input_points->begin(), input_points->end(), x_increasing);
  else if(initialization == "2a")
    std::sort(input_points->begin(), input_points->end(), y_decreasing);
  else if(initialization == "2b")
    std::sort(input_points->begin(), input_points->end(), y_increasing);
}

bool is_edge_visible(Point point, Segment segment, Polygon polygon)
{
  Point segment_source = segment.source();
  Point segment_target = segment.target();

  // Special case: If the 3 points are collinear then the segment is not visible
  if(CGAL::collinear(point, segment_source, segment_target))

    return false;

  // Create a triangle using the 3 the segment and the point given
  Triangle triangle(point, segment_source, segment_target);

  // Υπολογίζεις το intersection των παραπάνω segments με το polygon και θες να έχουν μόνο ένα κοινό σημείο δηλ. το σημείο source και target αντίστοιχα
  // Για κάθε segment του CH να υπολογίζω τα σημεία που κάνει intersect
  // Για να ξερω οτι τα δυο segments τεμνουν μονο τη μια κορυφη αρκει να ελεγξω αν τεμνουν μονο (!) τις δυο γειτονικες ακμές του ΚΠ
  //!!! Ενδεχομένως να μπορω να παρω το τριγωνο και να δω αν υπαρχουν γραμμες του πολυγωνου που το τεμνουν !!!
  for (EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); ++edge_itr) 
  {
    Point intersection_point;
    Segment intersection_segment;
    
    Object result = CGAL::intersection(triangle, *edge_itr);
    
    // If the intersection is a point...
    if (CGAL::assign(intersection_point, result)) 
    {
      // If that point is the segment points the continue
      if(intersection_point == segment_target) continue;
      if(intersection_point == segment_source) continue;
      // Else, not visible
      return false;
    }
    else
    {
      // If intersection is a segment...
      if (CGAL::assign(intersection_segment, result)) 
      {
        // If the intersection is the segment then it is visible...continue
        if(intersection_segment == segment || intersection_segment == segment.opposite()) continue;
        
        // If not, the point in not visible
        else if(intersection_segment != segment || intersection_segment != segment.opposite()) return false;
      }
      // If no intersection at all the continue...
      else 
        continue;
    }
  }

  return true;
}


void add_visible_edge(Point point, SegmentVector visible_polygon_edges, int edge_selection, Polygon* polygon)
{

  Segment insert_segment =  pick_edge(point, visible_polygon_edges, edge_selection);

  for(VertexIterator itr = polygon->vertices_begin(); itr != polygon->vertices_end(); ++itr)
  {
    if(insert_segment.target() == *itr){
      polygon->insert(itr, point);
      break;
    }
  }
}


SegmentVector find_red_edges(Point point, Polygon polygon, String initialization)
{
  Polygon convex_hull;
  // Calculate the CH
  CGAL::convex_hull_2(polygon.begin(), polygon.end(), std::back_inserter(convex_hull));

  // if(convex_hull.is_clockwise_oriented())
  //   convex_hull.reverse_orientation();

  SegmentVector red_edges;

  // Find visible edges
  for (EdgeIterator edge_itr = convex_hull.edges_begin(); edge_itr != convex_hull.edges_end(); ++edge_itr) 
  {
    if(is_edge_visible(point, *edge_itr, convex_hull))
    {
      // if(*edge_itr->direction())
      red_edges.push_back(*edge_itr);
    } 
  }

  return red_edges;
}


SegmentVector find_visible_edges(Point point, SegmentVector red_edges, Polygon polygon)
{
  SegmentVector visible_edges;
  
  if(red_edges.size() != 0)
  {      
    pSegmentVector p_red_edges_begin = red_edges.begin();
    // pSegmentVector p_red_edges_end = red_edges.end();

    // Special case: Point intersects a red edge so the only edge visible is that red edge
    if(CGAL::do_intersect(*p_red_edges_begin, point)){
      return red_edges;
    }
    
    // A boolean that indecates if we found the first red edge
    // bool flag = false;
    for(pSegmentVector p_red_edges = red_edges.begin(); p_red_edges!=red_edges.end(); ++p_red_edges)
    {
      EdgeCirculator edge_circ = polygon.edges_circulator();
      while(edge_circ->source()!=p_red_edges_begin->source()) ++edge_circ;

      do{
      // if the edge is visible from the point...
      if(is_edge_visible(point, *edge_circ, polygon))
        visible_edges.push_back(*edge_circ);
      edge_circ++;
      }
      while(edge_circ->source()!=p_red_edges->target());
    }
  }

  return visible_edges;
}


Polygon incremental_algorithm(PointVector input_points, int edge_selection, String initialization)
{
  // Sorting the points according to <initialization>
  sort_input_points(&input_points, initialization);

  // print_point_vector(input_points);

  pPointVector p_input_points = input_points.begin();

  // Start with the first 3 points
  Polygon polygon(p_input_points, p_input_points + 3);

  // We need CCW orientation for the polygon points
  if(polygon.is_clockwise_oriented())
    polygon.reverse_orientation();
    

  // Special case: Check if the first 3 points are collinear...add a 4th
  if(CGAL::collinear(input_points.at(0), input_points.at(1), input_points.at(2)))
  {
    VertexIterator itr = polygon.vertices_begin();
    polygon.insert(itr, input_points.at(3));
    advance(p_input_points, 1);
  } 

  advance(p_input_points, 3);

  // While the number of the input points is different of the size of the polygon, repeat...
  while(input_points.size() != polygon.size())
  {
    // std::cout << polygon.is_simple() << "\n";
    // print_polygon(polygon); 
    // Find the red edges of the CH, i.e the visible from the CH
    SegmentVector red_edges = find_red_edges(*p_input_points, polygon, initialization);
    // std::cout << "red\n";
    // print_segment_vector(red_edges);
    // Find the visible edges of the Polygon
    SegmentVector visible_edges = find_visible_edges(*p_input_points, red_edges, polygon);
    // std::cout << "vis\n";
    // print_segment_vector(visible_edges);

    // For every visible edge, insert a vertex in the polygon according to the edge selection strategy (random, min/ max area)
    add_visible_edge(*p_input_points, visible_edges, edge_selection, &polygon);
    
    // Continue with the next point...
    advance(p_input_points, 1);
  }

  return polygon;
}


//_____________Edge selection functions according to the input strategy <edge_selection>_________

Segment pick_edge(Point point ,SegmentVector visible_polygon_edges,int edge_selection){
  switch(edge_selection)
  {
  case 1: return pick_random_edge(visible_polygon_edges); break;
  case 2: return pick_min_area_edge(point, visible_polygon_edges); break;
  }
  return pick_max_area_edge(point, visible_polygon_edges);
}

Segment pick_max_area_edge(Point point ,SegmentVector visible_polygon_edges) {
  Segment max_segment;
  unsigned int max_area = -1;
  unsigned int current_area;

  for(pSegmentVector edge_itr = visible_polygon_edges.begin(); edge_itr != visible_polygon_edges.end(); ++edge_itr) {

    Triangle triangle(edge_itr->source(), edge_itr->target(), point);
    current_area = CGAL::abs(triangle.area());

    if(edge_itr == visible_polygon_edges.begin()) {
      max_area = current_area;
      max_segment = *edge_itr;
    }

    if(current_area > max_area) {
      max_segment = *edge_itr;
      max_area = current_area;
    }

  }

  return max_segment;
}

Segment pick_min_area_edge(Point point ,SegmentVector visible_polygon_edges) {
  Segment min_segment;
  unsigned int min_area = -1;
  unsigned int current_area;

  for(pSegmentVector edge_itr = visible_polygon_edges.begin(); edge_itr != visible_polygon_edges.end(); ++edge_itr) {

    Triangle triangle(edge_itr->source(), edge_itr->target(), point);
    current_area = CGAL::abs(triangle.area());

    if(edge_itr == visible_polygon_edges.begin()) {
      min_area = current_area;
      min_segment = *edge_itr;
    }

    if(current_area < min_area) {
      min_segment = *edge_itr;
      min_area = current_area;
    }

  }

  return min_segment;
}

Segment pick_random_edge(SegmentVector visible_polygon_edges) {
  int r = rand() % visible_polygon_edges.size();
  return visible_polygon_edges.at(r);
}


//___________________________Printing functions_________________________________


void print_point(Point point) {
  std::cout << point.x() << ", " << point.y() << std::endl;
}


void print_segment(Segment segment) {
  std::cout << segment.source().x() << "," << segment.source().y() <<  " - " << segment.target().x() << "," << segment.target().y() << std::endl;
}


void print_polygon(Polygon polygon) {
  for (EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); ++edge_itr) 
    print_segment(*edge_itr);
}


void print_point_vector(PointVector points) {
  for(Point point : points) {
    print_point(point);
  }
}


void print_segment_vector(SegmentVector segments) {
  for(Segment segment : segments) {
    print_segment(segment);
  }
}


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
    if (!(iss >> count >> x >> y)) { break; } // error

        Point point(x, y);
        points.push_back(point);
  }

return points;
}


void print_output(Polygon polygon, PointVector points, std::string filename, std::string algorithm, int edge_selection, std::string initialization, std::chrono::duration<double> duration) {
  std::ofstream outfile(filename);
  try{
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

      unsigned int polygon_area = CGAL::abs(polygon.area());
      
      Polygon convex_hull;
      CGAL::convex_hull_2(polygon.begin(), polygon.end(), std::back_inserter(convex_hull));

      unsigned int convex_hull_area = CGAL::abs(convex_hull.area());

      double ratio = (double)polygon_area / (double)convex_hull_area;

      outfile << "area: " << polygon_area << std::endl;
      outfile << "ratio: " <<  ratio << std::endl;
      outfile << "construction time: " << duration.count() << std::endl;

      outfile.close();

    }
    else throw "Output file is not open.\n";
  }
  catch(String error_msg)
  {
    std::cerr << error_msg << std::endl;
  }  

  
}