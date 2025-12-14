/*
Simple timer classes.

© 2025 BroknApples — modifications allowed; do not remove this notice.
*/


#ifndef TIMERS_HPP
#define TIMERS_HPP


#include <iostream>
#include <chrono>
#include <optional>


/**
 * @brief Class used to track time between two points as fps
 */
class FpsTimer {
  private:
    std::chrono::steady_clock::time_point _last_frame;
    double _fps;
    double _delta;

  public:
    /**
     * @brief Construct new object
     */
    FpsTimer() : _fps(0.0) {
      _last_frame = std::chrono::steady_clock::now();
    }

    /**
     * @brief Sets the new time from the last frame.
     * NOTE: Call every frame.
     */
    void update() {
      auto now = std::chrono::steady_clock::now();
      std::chrono::duration<double> frametime = now - _last_frame;
      _last_frame = now;

      _delta = frametime.count();
      if (_delta > 0.0) {
        _fps = 1.0 / _delta;
      } 
    }


    /**
     * @brief Gets the fps tracked in this timer
     * @returns double: FPS
     */
    double getFps() const {
      return _fps;
    }


    /**
     * @brief Gets the frametime
     * @returns double: dt
     */
    double getDelta() const {
      return _delta;
    }
};


/**
 * @brief Simple stopwatch class
 */
class StopwatchTimer {
  private:
    std::optional<std::chrono::steady_clock::time_point> _start;
  
  public:
    /**
     * @brief Set params to default
     */
    StopwatchTimer() = default;


    /**
     * @brief Starts the stopwatch
     * @returns bool: True/False of operation success.
     */
    bool start() {
      // Do not allow the timer to be ended without being reset
      if (_start.has_value()) {
        std::cout << "Please reset the timer before starting again." << std::endl;
        return false;
      }

      _start = std::chrono::steady_clock::now();
      return true;
    }


    /**
     * @brief Resets the stopwatch.
     */
    void reset() {
      _start.reset();
    }


    /**
     * @brief Elapsed time since the start of the watch.
     * 
     * Usage  ->   timer_name.elapsed(std::chrono::milliseconds{});
     * 
     * 
     * @tparam Duration: A 'std::chrono::' duration value such as 'milliseconds' or 'microseconds'
     * @returns double: Elapsed time
     */
    template <typename Duration>
    double elapsed(Duration) const {
      if (!_start.has_value()) {
        std::cout << "Must start() the stopwatch before getting the time difference." << std::endl;
        return 0.0;
      }

      return std::chrono::duration<double, typename Duration::period>(std::chrono::steady_clock::now() - *_start).count();
    }

    /** Helper methods for seconds, milliseconds, and microseconds */

    double elapsed_s()  const { return elapsed(std::chrono::seconds{}); }
    double elapsed_ms() const { return elapsed(std::chrono::milliseconds{}); }
    double elapsed_us() const { return elapsed(std::chrono::microseconds{}); }
};


/*
Simple timer class.

© 2025 BroknApples — modifications allowed; do not remove this notice.
*/
#include <iostream>
#include <optional>


/**
 * @brief Timer which tracks the time elapsed between two points in time.
 */
class IntervalTimer {
  private:
    std::optional<std::chrono::high_resolution_clock::time_point> _start;
    std::optional<std::chrono::high_resolution_clock::time_point> _end;


  public:
    /**
     * @brief Set params to default
     */
    IntervalTimer() = default;


    /**
     * @brief Set the starting point for time measuring
     * @returns bool: True/False of success
     */
    bool start() {
      // Do not allow the timer to be started without being reset
      if (_start.has_value() || _end.has_value()) {
        std::cout << "Please reset the timer before starting it." << std::endl;
        return false;
      }

      _start = std::chrono::high_resolution_clock::now();
      return true;
    }


    /**
     * @brief Set the ending point for time measuring
     * @returns bool: True/False of success
     */
    bool end() {
      // Do not allow the timer to be ended without being reset
      if (_end.has_value() || !_start.has_value()) {
        std::cout << "Please reset the timer before ending." << std::endl;
        return false;
      }

      _end = std::chrono::high_resolution_clock::now();
      return true;
    }

    /**
     * @brief Reset the start and end values
     */
    void reset() {
      _start.reset();
      _end.reset();
    }


    /**
     * @brief Get the difference between the recorded start and end values
     * 
     * Usage  ->   timer_name.diff(std::chrono::milliseconds{});
     * 
     * 
     * @tparam Duration: A 'std::chrono::' duration value such as 'milliseconds' or 'microseconds'
     * @returns double: Elapsed time
     */
    template <typename Duration>
    double diff(Duration) const {
      // In order to get the difference, the timer must have started and ended
      if (!_start.has_value() || !_end.has_value()) {
        std::cout << "Must start() and end() the timer before getting the time difference." << std::endl;
        return 0.0;
      }

      return std::chrono::duration<double, typename Duration::period>(*_end - *_start).count();
    }

    /** Helper methods for seconds, milliseconds, and microseconds */

    double diff_s()  const { return diff(std::chrono::seconds{}); }
    double diff_ms() const { return diff(std::chrono::milliseconds{}); }
    double diff_us() const { return diff(std::chrono::microseconds{}); }
};

#endif // TIMERS_HPP
