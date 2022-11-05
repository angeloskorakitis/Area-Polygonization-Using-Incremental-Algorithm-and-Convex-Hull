/////////////////////////////////////////////////////////////////////////////////
//
//
// Polygonization Using the Incremental Algorithm 
//
//
/////////////////////////////////////////////////////////////////////////////////


#include "Utilities.hpp"


//______________________Compare functions for input points <initialization>_____________________________

bool compare_x_increasing(const Point a, const Point b);

bool compare_x_decreasing(const Point a, const Point b);

bool compare_y_increasing(const Point a, const Point b);

bool compare_y_decreasing(const Point a, const Point b);

// Takes a point vector and returns it sorted according to <initialization>.

void sort_input_points(PointVector* input_points, String initialization);


//_____________________<edge_selection> functions according to the input strategy________________________

// Returns a visible edge according to <edge selection> strategy (random, min area, max area).

Segment pick_edge(Point point ,SegmentVector visible_polygon_edges, int edge_selection);

// Returns the -visible from point- edge of the polygon, that creates the largest (area-wise) triangle with the given point.

Segment pick_max_area_edge(Point point ,SegmentVector visible_polygon_edges);

// Returns the -visible from point- edge of the polygon, that creates the smallest (area-wise) triangle with the given point.

Segment pick_min_area_edge(Point point ,SegmentVector visible_polygon_edges);

// Returns a random visible edge of polygon.

Segment pick_random_edge(SegmentVector visible_polygon_edges);


//_______________________________________Polygonization__________________________________________________

// Returns a boolean true if the segment-edge of the polygon is visible from the point, else false.

bool is_edge_visible(Point point,Segment segment,Polygon polygon);

// Returns a vector of segments with the red edges of the CH.

SegmentVector find_red_edges(Point point, Polygon polygon);

// Returns a vector of segments with the visible edges from the point to the polygon.

SegmentVector find_visible_edges(Point point, SegmentVector visible_edges, Polygon polygon);

// Adds the point to the polygon according to the visible edges and the edge selection strategy

void add_point_to_polygon(Point point, SegmentVector visible_polygon_edges ,int edge_selection, Polygon* polygon);

// Implementation of the incremental algorithm

Polygon incremental_algorithm(PointVector input_points ,int edge_selection, String initialization);