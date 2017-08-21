#include <chrono>

using namespace std::chrono;

high_resolution_clock::time_point take_timestamp() {
  return high_resolution_clock::now();
}

long long take_timestamp_diff(const high_resolution_clock::time_point& timestamp) {
  high_resolution_clock::time_point timestamp_2 = high_resolution_clock::now();
  return duration_cast<microseconds>(timestamp_2 - timestamp).count();
}
