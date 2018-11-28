#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <chrono>

namespace utils
{
    std::vector<unsigned char> compress(const std::vector<unsigned char> &raw);

    class Timer {
    public:
        void start();

        void stop();

        long long int elapsed();

        bool isStarted();

    private:
        std::chrono::steady_clock::time_point m_start;
        bool m_isStarted = false;
    };

};

#endif // UTILS_HPP
