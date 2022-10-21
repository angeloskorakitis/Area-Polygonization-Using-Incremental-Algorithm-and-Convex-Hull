#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>
#include <vector>
#include <numeric>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;

typedef K::Segment_2 Segment_2;

typedef CGAL::Convex_hull_traits_adapter_2<K,
          CGAL::Pointer_property_map<Point_2>::type > Convex_hull_traits_2;



std::vector<Segment_2> segment_convex_hull_create(std::vector<Point_2> points, std::vector<std::size_t> out);
bool segment_intersects_convex_hull(Segment_2 seg, std::vector<Segment_2> convex_hull, Segment_2 excl);
bool is_edge_visible(Point_2 p, Segment_2 seg, std::vector<Segment_2> convex_hull);

void print_point(Point_2 point);
void print_segment(Segment_2 seg);
void print_convex_hull(std::vector<Segment_2> convex_hull);

int main()
{
  std::vector<Point_2> points = { Point_2(10,0),
                                  Point_2(10,0),
                                  Point_2(0,10),
                                  Point_2(1,1),
                                  Point_2(8,11),
                                  Point_2(3,4),
                                  Point_2(0,0),
                                  Point_2(6,6),
                                  Point_2(18,4),
                                  Point_2(16,16),
                                  Point_2(10,10),
                                  Point_2(2,6) };
  std::vector<std::size_t> indices(points.size()), out;
  std::iota(indices.begin(), indices.end(),0);
  CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
                      Convex_hull_traits_2(CGAL::make_property_map(points)));
  for( std::size_t i : out){
    std::cout << "points[" << i << "] = " << points[i] << std::endl;
  }

  std::cout << "~~~" << std::endl << std::endl;

  std::vector<Segment_2> convex_hull = segment_convex_hull_create(points, out);

  
  print_convex_hull(convex_hull);


 Point_2 rand_p(3, 5);
 if(is_edge_visible(rand_p, convex_hull[0], convex_hull)) {
  std::cout << "VISIBOL" << std::endl;
 }
 else {
  std::cout << "NOT VISIBOL" << std::endl;
 }


  Segment_2 seg1(points[1], points[5]), seg2(points[4], points[8]);

  if(CGAL::do_intersect(seg1, seg2)) {
    std::cout << "INTERSECC" << std::endl;
  }
  else {
    std::cout << "NO INTERSECC" << std::endl;
  }

  return 0;
}
  
// Checks if a given segment seg intersects with the convex_hull. A segment of the hull excl can be excluded from checking.
// true if they intersect, false if they do not intersect.
bool segment_intersects_convex_hull(Segment_2 seg, std::vector<Segment_2> convex_hull, Segment_2 excl) {
  for(Segment_2 to_check : convex_hull) {

    // excl is excluded from checking.
    if((to_check == excl) || (to_check == excl.opposite() )) continue;

    if(CGAL::do_intersect(seg, to_check)) return true;
  }

  return false;
}

// Function to check if a Segment seg is visible from a point p.
bool is_edge_visible(Point_2 p, Segment_2 seg, std::vector<Segment_2> convex_hull) {  //// ISN'T WORKING YET. HITS ON THE ENDPOINTS OF TH SEGMENTS. 
  Point_2 source = seg.source();
  Point_2 target = seg.target();

  // Two segments, both starting from the point and reaching the endpoints of the segment.
  Segment_2 view1(p, source), view2(p, target);

  // Check for view 1.
  segment_intersects_convex_hull(view1, convex_hull, seg);

  // Check for view 2.
  segment_intersects_convex_hull(view2, convex_hull, seg);

  return false;
}

// Returns a vector with the Segments of a Convex Hull.
std::vector<Segment_2> segment_convex_hull_create(std::vector<Point_2> points, std::vector<std::size_t> out) {
  std::vector<Segment_2> convex_hull_segments;
  for(int i = 0; i < out.size() - 1; i++) {
    std::cout << "points[" << out[i] << "] = " << points[out[i]] << std::endl;

    Segment_2 temp_seg(points[out[i]], points[out[i+1]]);

    convex_hull_segments.push_back(temp_seg);
  }

  Segment_2 temp_seg(points[out[out.size() - 1]], points[out[0]]);

  convex_hull_segments.push_back(temp_seg);
  
  return convex_hull_segments;
}


void print_point(Point_2 point) {
  std::cout << "(" << point.x() << ", " << point.y() << ")" << std::endl;
}

void print_segment(Segment_2 seg) {
  std::cout << "(" << seg.source().x() << ", " << seg.source().y() << ") - (" << seg.target().x() << ", " << seg.target().y() << ")" << std::endl;
}

void print_convex_hull(std::vector<Segment_2> convex_hull) {
  for(Segment_2 seg : convex_hull) {
    print_segment(seg);
  }
}