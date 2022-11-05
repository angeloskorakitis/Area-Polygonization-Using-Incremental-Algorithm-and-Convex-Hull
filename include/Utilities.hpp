/////////////////////////////////////////////////////////////////////////////////
//
//
//                                  Utilities  
//
//
/////////////////////////////////////////////////////////////////////////////////


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <algorithm>
#include <vector>
#include <numeric>
#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>


// Some typedefs for readability

typedef CGAL::Epick                                           K;
typedef K::Point_2                                            Point;
typedef CGAL::Segment_2<K>                                    Segment;
typedef CGAL::Object                                          Object;   
typedef CGAL::Polygon_2<K>                                    Polygon;
typedef CGAL::Triangle_2<K>                                   Triangle;
typedef Polygon::Vertex_iterator                              VertexIterator;
typedef Polygon::Edge_const_circulator                        EdgeCirculator;
typedef Polygon::Edge_const_iterator                          EdgeIterator;
typedef std::vector<Point>                                    PointVector;
typedef std::vector<Segment>                                  SegmentVector;
typedef PointVector::iterator                                 pPointVector;
typedef SegmentVector::iterator                               pSegmentVector;
typedef std::string                                           String;

//______________________________Input/Output functions___________________________________


// Parses the file filename and returns a PointVector with the input points

PointVector parse_file(String filename);

// Output file function. Writes the results into the output file. 

void print_output(Polygon polygon, PointVector points, String filename, String algorithm, int edge_selection, String initialization, std::chrono::duration<double> duration);


//________________________________Printing functions______________________________________


// Prints a Point

void print_point(Point);

// Prints a Segment

void print_segment(Segment);

// Prints a Polygon

void print_polygon(Polygon);

// Prints a PointVector

void print_point_vector(PointVector points);

// Prints a SegmentVector

void print_segment_vector(SegmentVector segments);