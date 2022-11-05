/////////////////////////////////////////////////////////////////////////////////
//
//
// Implementation of Polygonization Using the Convex Hull Algorithm 
//
//
/////////////////////////////////////////////////////////////////////////////////


#include <Polygonization_Using_Convex_Hull_Algorithm.hpp>


////////////////////////////// Helpful Functions //////////////////////////////


bool point_is_on_polygon(Point point, Polygon polygon) {

  if(CGAL::bounded_side_2(polygon.begin(), polygon.end(), point) == CGAL::ON_BOUNDARY) {
    return true;
  }
  return false;
}


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


bool edge_is_visible(Point point, Segment segment, Polygon polygon, PointVector points) {
  if(!edge_is_visible_edges(point, segment, polygon)) return false;
  if(!edge_is_visible_points(point, segment, points)) return false;
  return true;
}


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



////////////////////////////// Pick Edge Functions //////////////////////////////

Segment pick_random_edge(Polygon polygon) {
  int r = rand() % polygon.size();
  return *(polygon.edges_begin() + r);
}


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


//////////////////////////////// Polygonization /////////////////////////////////////


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


void add_point_to_polygon(Polygon* polygon, Tuple tuple) {

  // If point was added to the polygon on a previous iteration "by accident",
  // just continue to next inner point.
  if(!point_is_on_polygon(tuple->point, *polygon)) {

    (*polygon).insert((*polygon).begin() + tuple->position, tuple->point);
  }


}


Polygon convex_hull_algorithm(PointVector input_points, int edge_selection) {

  // Polygon initally is the convex hull.
  Polygon polygon;
  
  CGAL::convex_hull_2(input_points.begin(), input_points.end(), std::back_inserter(polygon));

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