#ifndef time_measure_hpp
#define time_measure_hpp

#include <chrono>
#include <utility>

/**
* Just a wrapper to specify the time unit and the clock type via template parameters.
*/
template <typename timeT = std::chrono::milliseconds, class clockT = std::chrono::system_clock> struct time_measure {
	/**
	* Returns the quantity (count) of the elapsed time as TimeT units
	*/
	template <typename F, typename... Args> static typename timeT::rep execute(F &&func, Args &&... args) {
		const auto start = clockT::now();

		std::forward<decltype(func)>(func)(std::forward<Args>(args)...);

		const auto duration = std::chrono::duration_cast<timeT>(clockT::now() - start);

		return duration.count();
	}

	/**
	* Returns the duration (in chrono's type system) of the elapsed time
	*/
	template <typename F, typename... Args> static timeT duration(F &&func, Args &&... args) {
		const auto start = clockT::now();

		std::forward<decltype(func)>(func)(std::forward<Args>(args)...);

		return std::chrono::duration_cast<timeT>(clockT::now() - start);
	}
};

#endif /* end of include guard: time_measure_hpp */
