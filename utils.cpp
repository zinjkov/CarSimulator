#include "utils.hpp"
#include <opencv2/opencv.hpp>


std::vector<unsigned char> utils::compress(const std::vector<unsigned char> &raw)
{
    std::vector<unsigned char> compressed;
    cv::Mat image(240, 320, CV_8UC3, (char*)raw.data());
    cv::cvtColor(image, image, CV_RGB2BGR);
    cv::imencode(".jpg", image, compressed);
    return compressed;
}

void utils::Timer::start()
{
    m_start = std::chrono::steady_clock::now();
    m_isStarted = true;
}

void utils::Timer::stop()
{
    m_isStarted = false;
}

long long utils::Timer::elapsed()
{
    return m_isStarted ?
           std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::steady_clock::now() - m_start).count()
                       : 0;
}

bool utils::Timer::isStarted()
{
    return m_isStarted;
}
