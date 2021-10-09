/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#include "asgt.h"

int main(int argc, char** argv)
{
  SATDigraph sat_digraph(std::cin);
  sat_digraph.print_digraph();

  SATSolver sat_solver(sat_digraph);
  sat_solver.print_strong_components();

  return EXIT_SUCCESS;
}
