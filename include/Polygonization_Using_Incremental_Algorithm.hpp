/////////////////////////////////////////////////////////////////////////////////
//
//
// Polygonization Using the Incremental Algorithm 
//
//
/////////////////////////////////////////////////////////////////////////////////

// CHECK: Not sure about 2 include files

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Simple_Cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>


// Some typedefs in order to make the code more readable

typedef CGAL::Epick                                           K;
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


bool is_edge_visible(Point ,Segment ,Polygon );


void add_visible_edge(Point , SegmentVector, Polygon* );


// Returns a vector of Segments with the red edges of the CH

SegmentVector find_red_edges(Point, Polygon );

// Returns a vector of Segments with the visible edges of the Polygon

SegmentVector find_visible_edges(SegmentVector, Polygon);


// Implementation of the incremental algorithm

Polygon incremental_algorithm(PointVector );


//_____________Edge selection functions according to the input <strategy>_________

// Returns the -visible from point- edge of the polygon, that creates the largest (area-wise) triangle with the given point.

Segment pick_max_area_edge(Point point ,SegmentVector visible_polygon_edges);

// Returns the -visible from point- edge of the polygon, that creates the smallest (area-wise) triangle with the given point.

Segment pick_min_area_edge(Point point ,SegmentVector visible_polygon_edges);

// Returns random edge of polygon.

Segment pick_random_edge(SegmentVector visible_polygon_edges);


//___________________________Printing functions_________________________________

// Prints a Point

void print_point(Point);

// Prints a Segment

void print_segment(Segment);

// Prints a Polygon

void print_polygon(Polygon);