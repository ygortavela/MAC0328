#include "asgt.h"

int main(int argc, char** argv)
{
  SATDigraph sat_digraph(std::cin);
  SATSolver sat_solver(sat_digraph);

  return EXIT_SUCCESS;
}
