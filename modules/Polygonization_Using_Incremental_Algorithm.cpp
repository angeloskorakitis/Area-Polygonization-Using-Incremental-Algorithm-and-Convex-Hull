/////////////////////////////////////////////////////////////////////////////////
//
//
// Implementation of Polygonization Using the Incremental Algorithm 
//
//
/////////////////////////////////////////////////////////////////////////////////

#include "Polygonization_Using_Incremental_Algorithm.hpp"

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


SegmentVector find_visible_edges(Point point, SegmentVector red_edges, Polygon polygon)
{
  SegmentVector visible_edges;
  // Αν υπάρχουν κόκκινες ακμές στο ΚΠ δηλ. ορατές από το σημείο...
  if(red_edges.size() != 0)
  {      
    pSegmentVector p_red_edges_begin = red_edges.begin();
    pSegmentVector p_red_edges_end = red_edges.end();

    // Special case: already in the edge
    if(CGAL::do_intersect(*p_red_edges_begin, point)){
      return red_edges;
    }
    
      // Για κάθε κόκκινη πρέπει να βρώ ποιες ακμές είναι ορατές...
      // Κάθε κόκκινη θα έχει τις κορυφές της στην πολυγωνική γραμμή...
      bool flag = false;
      for(EdgeIterator edge_itr = polygon.edges_begin(); edge_itr != polygon.edges_end(); ++edge_itr)
      {
        // Αν η κορυφή του πολυγώνου είναι η κορυφή της κόκκινης ακμής...
        if(edge_itr->source() == p_red_edges_begin->source() || flag == true)
        {
          flag = true;
          // if the edge is visible from the point...
          if(is_edge_visible(point, *edge_itr, polygon))
            visible_edges.push_back(*edge_itr);
        }
        if(edge_itr->target() == p_red_edges_end->target() && flag == true)
          break;
        
      }
    }
  return visible_edges;
}


Polygon incremental_algorithm(PointVector input_points, int edge_selection)
{
  pPointVector p_input_points = input_points.begin();

  // Start with the first 3 points
  Polygon polygon(p_input_points, p_input_points + 3);

  if(polygon.is_clockwise_oriented())
    polygon.orientation();

  // SPECIAL CASE: Check if the first 3 points are collinear...add a 4th
  // Not ready yeeet!
  if(CGAL::collinear(input_points.at(0), input_points.at(1), input_points.at(2))){
    VertexIterator itr = polygon.vertices_begin();
    polygon.insert(itr, input_points.at(3));
    advance(p_input_points, 1);
  } 

  advance(p_input_points, 3);

  // While the number of the input points is different of the size of the polygon, repeat...
  while(input_points.size() != polygon.size())
  {
    // Find the red edges of the CH, i.e the visible from the CH
    SegmentVector red_edges = find_red_edges(*p_input_points, polygon);
    std::cout << "red: \n";
    print_segment_vector(red_edges);

    // Find the visible edges of the Polygon
    SegmentVector visible_edges = find_visible_edges(*p_input_points, red_edges, polygon);
     std::cout << "vis: \n";
    print_segment_vector(visible_edges);

    // For every visible edge, insert a vertex in the polygon according to the edge selection strategy (random, min/ max area)
    add_visible_edge(*p_input_points, visible_edges, edge_selection, &polygon);
    
    // Continue with the next point...
    advance(p_input_points, 1);
  }

  return polygon;
}


//_____________Edge selection functions according to the input <strategy>_________

Segment pick_edge(Point point ,SegmentVector visible_polygon_edges,int edge_selection){
  switch(edge_selection){
  case 1: return pick_random_edge(visible_polygon_edges); break;
  case 2: return pick_min_area_edge(point, visible_polygon_edges); break;
  }
  return pick_max_area_edge(point, visible_polygon_edges);
}

Segment pick_max_area_edge(Point point ,SegmentVector visible_polygon_edges) {
  Segment max_segment;
  double max_area = -1;
  double current_area;

  for(pSegmentVector edge = visible_polygon_edges.begin(); edge != visible_polygon_edges.end(); edge++) {

    Triangle triangle(edge->source(), edge->target(), point);
    // area() function may return negative number.
    current_area = CGAL::abs(triangle.area());

    if(current_area > max_area) {
      max_segment = *edge;
      max_area = current_area;
    }

  }

  return max_segment;
}

Segment pick_min_area_edge(Point point ,SegmentVector visible_polygon_edges) {
  Segment min_segment;
  double min_area = -1;
  double current_area;

  for(pSegmentVector edge = visible_polygon_edges.begin(); edge != visible_polygon_edges.end(); edge++) {

    Triangle triangle(edge->source(), edge->target(), point);
    // area() function may return negative number.
    current_area = CGAL::abs(triangle.area());

    if(edge == visible_polygon_edges.begin()) {
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

Segment pick_random_edge(SegmentVector visible_polygon_edges) {
  int r = rand() % visible_polygon_edges.size();
  return visible_polygon_edges.at(r);
}


//___________________________Printing functions_________________________________


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