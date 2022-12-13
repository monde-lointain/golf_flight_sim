#include "./math/stats.h"
#include "Application.h"
#include <chrono>
#include <intrin.h>
#include <iostream>
#include <numeric>

// TODO: Add distance markers and a system to track the coordinates of each ball
// TODO: Add a user GUI that displays the wind speed and shows the different
// force vectors acting upon the ball.

int main(int argc, char *args[]) {

  //// initialize parameters for benchmarking
  // const int num_simulations = 100;
  // int i = 0;
  // uint64_t simulation_cycles[num_simulations];

  //// start the clock for the overall benchmarking session
  // auto benchmark_start = std::chrono::high_resolution_clock::now();
  Application app;

  app.initialize();
  app.run();
  app.destroy();

  // run_simulation();

  // while (i < num_simulations) {

  //  // Get the CPU cycles at the start of the benchmark
  //  auto start = __rdtsc();

  //  // Run the simulation
  //  run_simulation();

  //  // Get the number of CPU cycles at the end of the benchmark
  //  auto end = __rdtsc();

  //  // Calculate the number of CPU cycles taken and insert it into the array
  //  auto num_cycles = end - start;
  //  simulation_cycles[i] = num_cycles;

  //  i++;

  //}

  //// Set the end time for the overall benchmarking session
  // auto benchmark_end = std::chrono::high_resolution_clock::now();

  //// Calculate the average number of cycles taken and the standard deviation
  // auto avg_cycles =
  //     std::reduce(std::cbegin(simulation_cycles),
  //     std::cend(simulation_cycles)) / num_simulations;
  // auto cycles_std = stdev_s(simulation_cycles);

  // std::chrono::duration<double, std::micro> avg_time =
  //     (benchmark_end - benchmark_start) / num_simulations;

  //// Get the total time of the overall benchmarking session
  // auto benchmark_duration_time =
  //     std::chrono::duration<double>(benchmark_end - benchmark_start);

  // std::cout << "Total benchmark time: "
  //           << std::to_string(benchmark_duration_time.count()) << " seconds."
  //           << "\n";

  // std::cout << "Number of simulations: " << std::to_string(num_simulations)
  //           << "\n";

  // std::cout << "Average simulation time: " << avg_time.count()
  //           << " microseconds."
  //           << "\n";

  // std::cout << "Average cycles: " << std::to_string(avg_cycles) << " +/- "
  //           << std::to_string(cycles_std) << " cycles."
  //           << "\n";

  return 0;
}