/////////////////////////////////////////////////////////////////////////////////
//
//
// Implementation of Polygonization Using the Incremental Algorithm 
//
//
/////////////////////////////////////////////////////////////////////////////////


#include "Polygonization_Using_Incremental_Algorithm.hpp"


//____________________________Compare functions for input points <initialization>_________________________________


bool compare_x_increasing(const Point a, const Point b){
    return a.x() <= b.x(); 
}


bool compare_x_decreasing(const Point a, const Point b){
    return a.x() >= b.x(); 
}


bool compare_y_increasing(const Point a, const Point b){
    return a.y() <= b.y(); 
}


bool compare_y_decreasing(const Point a, const Point b){
    return a.y() >= b.y(); 
}


void sort_input_points(PointVector* input_points, String initialization)
{
  if(initialization == "1a")
    std::sort(input_points->begin(), input_points->end(), compare_x_decreasing);
  else if(initialization == "1b")
    std::sort(input_points->begin(), input_points->end(), compare_x_increasing);
  else if(initialization == "2a")
    std::sort(input_points->begin(), input_points->end(), compare_y_decreasing);
  else if(initialization == "2b")
    std::sort(input_points->begin(), input_points->end(), compare_y_increasing);
}


//________________Edge selection functions according to the input strategy <edge_selection>_______________


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
  size_t r = rand() % visible_polygon_edges.size();
  return *(visible_polygon_edges.begin()+r);
}


//_______________________________________Polygonization__________________________________________________


bool is_edge_visible(Point point, Segment segment, Polygon polygon)
{
  Point segment_source = segment.source();
  Point segment_target = segment.target();

  // Special case: If the 3 points are collinear then the segment is not visible
  if(CGAL::collinear(point, segment_source, segment_target))
    return false;

  // Create a triangle using the 3 the segment and the point given
  Triangle triangle(point, segment_source, segment_target);

  // Calculates the intersection of the segment with the polygon. If the intersection is only a point we want it to be the source or the target.
  // Else if the intersection is a segment we want it to be the segment.
  // Else, the segment is not visible from the point.
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


SegmentVector find_red_edges(Point point, Polygon polygon)
{
  Polygon convex_hull;
  // Calculate the CH
  CGAL::convex_hull_2(polygon.begin(), polygon.end(), std::back_inserter(convex_hull));

  SegmentVector red_edges;

  // Find visible edges
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
  
  if(red_edges.size() != 0)
  {      
    pSegmentVector p_red_edges_begin = red_edges.begin();

    // Special case: Point intersects a red edge so the only edge visible is that red edge
    if(CGAL::do_intersect(*p_red_edges_begin, point)){
      return red_edges;
    }
    
    // Iterate all red edges...
    for(pSegmentVector p_red_edges = red_edges.begin(); p_red_edges!=red_edges.end(); ++p_red_edges)
    {
      EdgeCirculator edge_circ = polygon.edges_circulator();
      // Iterate the polygon edges until the first point of the circulator is the same as the first point of the red edge
      while(edge_circ->source()!=p_red_edges->source()) ++edge_circ;

      do{
      // If the edge is visible from the point add it to the structure
      if(is_edge_visible(point, *edge_circ, polygon))
        visible_edges.push_back(*edge_circ);
      edge_circ++;
      }
      // Continue to iterate the polygon until the second point of the circulator is the same as the last point of the red edge
      while(edge_circ->source()!=p_red_edges->target());
    }
  }

  return visible_edges;
}


void add_point_to_polygon(Point point, SegmentVector visible_polygon_edges, int edge_selection, Polygon* polygon)
{
  // Pick the segment to insert a point between the start and end points.
  Segment insert_segment =  pick_edge(point, visible_polygon_edges, edge_selection);

  // Iterate the polygon until you find the correct position for the point.
  for(VertexIterator vertices_itr = polygon->vertices_begin(); vertices_itr != polygon->vertices_end(); ++vertices_itr)
  {
    if(insert_segment.target() == *vertices_itr){
      polygon->insert(vertices_itr, point);
      break;
    }
  }
}


Polygon incremental_algorithm(PointVector input_points, int edge_selection, String initialization)
{
  // Sorting the points according to <initialization>
  sort_input_points(&input_points, initialization);

  pPointVector p_input_points = input_points.begin();

  // Start with the first 3 points
  Polygon polygon(p_input_points, p_input_points + 3);

  // Special case: Check if the first 3 points are collinear...add a 4th
  if(CGAL::collinear(input_points.at(0), input_points.at(1), input_points.at(2)))
  {
    VertexIterator itr = polygon.vertices_begin();
    polygon.insert(itr, input_points.at(3));
    advance(p_input_points, 1);
  } 

  // We need CCW orientation for the polygon points
  if(polygon.is_clockwise_oriented())
    polygon.reverse_orientation();

  advance(p_input_points, 3);

  // While the number of the input points is different of the size of the polygon, repeat...
  while(input_points.size() != polygon.size())
  {

    // Find the red edges of the CH, i.e the visible from the CH
    SegmentVector red_edges = find_red_edges(*p_input_points, polygon);

    // Find the visible edges of the Polygon
    SegmentVector visible_edges = find_visible_edges(*p_input_points, red_edges, polygon);

    // Insert a point into the polygon according to the edge selection strategy (random, min/ max area)
    add_point_to_polygon(*p_input_points, visible_edges, edge_selection, &polygon);
    
    // Continue with the next point...
    advance(p_input_points, 1);
  }

  return polygon;
}
