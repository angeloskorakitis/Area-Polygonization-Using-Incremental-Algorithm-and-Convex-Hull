#include <CGAL/convex_hull_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Simple_Cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>


typedef CGAL::Epick   K;
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
SegmentVector find_red_edges(Point, Polygon );
SegmentVector find_visible_edges(SegmentVector, Polygon);
void add_visible_edge(Point , SegmentVector, Polygon* );
Polygon incremental_algorithm(PointVector );

Segment pick_max_area_edge(Point point ,SegmentVector visible_polygon_edges);
Segment pick_min_area_edge(Point point ,SegmentVector visible_polygon_edges);
Segment pick_random_edge(SegmentVector visible_polygon_edges);

void print_point(Point);
void print_segment(Segment);
void print_polygon(Polygon);