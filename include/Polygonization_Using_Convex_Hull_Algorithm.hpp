/////////////////////////////////////////////////////////////////////////////////
//
//
// Polygonization Using the Convex Hull Algorithm
//
//
/////////////////////////////////////////////////////////////////////////////////


#include "Utilities.hpp"


enum { RANDOM = 1, MIN_AREA = 2, MAX_AREA = 3};


// Struct to improve time performance.
struct tuple {
  Point point;
  int position;
};

typedef struct tuple* Tuple;


// Helpful Functions.
// Returns true if a point is on the perimeter of a polygon.

bool point_is_on_polygon(Point point, Polygon polygon);

// Checking visibility regarding only the polygon edges.

bool edge_is_visible_edges(Point p, Segment seg, Polygon polygon);

// Checking visibility regarding only for the inner points.

bool edge_is_visible_points(Point point, Segment segment, PointVector points);

// Checking visibility regarding both the edges of the polygon and the inner points.

bool edge_is_visible(Point point, Segment segment, Polygon polygon, PointVector points);

// Finds from a set of points the one closest to edge.
// There must be visibility from point to edge.
// If there is no visible point from edge, return point (-1, -1)

Point point_closest_to_edge(Segment edge, PointVector points, Polygon polygon);

// Removes point from a set of points. 
// True on success, false otherwise. 

bool remove_point_from_vector(PointVector* points, Point point);

// Returns random edge of polygon.

Segment pick_random_edge(Polygon polygon);

// Finds the edge of the polygon, that creates the largest (area-wise) triangle with the closest visible point to it.
// Returns the tuple with the edge and the point.

Tuple pick_max_area_edge(Polygon polygon, PointVector points);

// Returns the edge of the polygon, that creates the smallest (area-wise) triangle with the closest visible point to it.

Tuple pick_min_area_edge(Polygon polygon, PointVector points);


// Convex Hull.


// The convex hull algorithm function. 

Polygon convex_hull_algorithm(PointVector initial_points, int edge_selection);

// Point gets added to polygon. The edge of polygon gets erased and
// two new edges previous_edge.source() to point and previous_edge.target() to point are added.

void add_point_to_polygon(Point point, Segment edge, Polygon* polygon);

// Point gets added to polygon. The edge of polygon gets erased and
// two new edges previous_edge.source() to point and previous_edge.target() to point are added.

void add_point_to_polygon(Polygon* polygon, Tuple tuple);