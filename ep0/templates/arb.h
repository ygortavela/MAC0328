/* the definition of HeadStart may be changed in this file; the rest
 * MUST NOT be changed
 */

#ifndef ARB_H
#define ARB_H

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS> Arb;
typedef boost::graph_traits<Arb>::vertex_descriptor Vertex;
typedef boost::graph_traits<Arb>::vertices_size_type ArbSize;

/* Students must adapt this class for their needs; the innards of
   these objects are only accessed by the student's code */
class HeadStart {
public:
  std::vector<int> discovery_time, finish_time;
  HeadStart(std::vector<int> discovery_time, std::vector<int> finish_time) : discovery_time(discovery_time), finish_time(finish_time) {}
};

void process_vertex(Arb &arb, Vertex current_vertex, std::vector<int> &discovery_time, std::vector<int> &finish_time, std::vector<int> &visited_vertices);

#endif // #ifndef ARB_H
