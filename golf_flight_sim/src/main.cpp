//#include "./math/stats.h"
// #define WIN32_LEAN_AND_MEAN
//#define _CRTDBG_MAP_ALLOC_NEW
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
#include "Application.h"
#include "./tracy/tracy/Tracy.hpp"

// TODO: Make a release .exe and a Linux build as well.
// TODO: v2.0: Render the game in 3D! :D
// TODO: This isn't necessary at the moment, but the slowest part of the game
// loop by far right now is the rendering of GUI text with SDL_ttf. In the
// future when performance constraints become tighter, we can speed this up by
// saving all the text as a tilemap and creating a function which draws text to
// the screen by selecting tiles from the map.

int main(int argc, char *args[]) {
  //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  //// Send all reports to STDOUT
  //_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  //_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

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
  //_CrtDumpMemoryLeaks();
  return 0;
}