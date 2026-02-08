#pragma once

#include <chrono>
#include <iostream>
#include <mutex>
#include <variant>

namespace LambdaSnail::utils
{
    struct LoadBalanceCheckResult
    {
        bool isAllowed{ false };
        std::chrono::milliseconds waitHint{};
    };

    class LoadBalancer
    {
    public:
        LoadBalancer(size_t maxTokens, size_t fillRate, std::chrono::milliseconds fillInterval, size_t fillSize = 1) :
            m_MaxTokenCount(maxTokens),
            m_FillRate(fillRate),
            m_CurrentTokenCount(maxTokens),
            m_FillInterval(fillInterval),
            m_FillSize(fillSize)
        {
            m_LastFillTime = std::chrono::steady_clock::now();
        }

        LoadBalanceCheckResult isAllowed()
        {
            std::scoped_lock lock(m_Lock);

            auto waitHint = refillTokens();
            if (m_CurrentTokenCount > 0)
            {
                std::cout << "Request allowed" << std::endl;
                --m_CurrentTokenCount;
                return LoadBalanceCheckResult
                {
                    .isAllowed = true
                };
            }

            std::cout << "Recommend waiting for: " << waitHint.value() << std::endl;
            return LoadBalanceCheckResult
            {
                .waitHint = waitHint.value()
            };
        }

    private:
        size_t m_MaxTokenCount;
        size_t m_FillRate;
        size_t m_CurrentTokenCount{};

        std::chrono::steady_clock::time_point m_LastFillTime{};

        std::chrono::milliseconds m_FillInterval;
        uint32_t m_FillSize;

        std::mutex m_Lock{};

        std::optional<std::chrono::milliseconds> refillTokens()
        {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastFillTime);

            std::cout << elapsed << " elapsed since last refill" << std::endl;
            std::cout << "Token count: " << m_CurrentTokenCount << std::endl;

            if (elapsed > m_FillInterval)
            {
                auto const newTokens = static_cast<size_t>(elapsed.count() * 0.001 * m_FillRate);
                m_CurrentTokenCount = std::min(m_MaxTokenCount, m_CurrentTokenCount + newTokens);
                m_LastFillTime = now;

                std::cout << "Token count increased: " << m_CurrentTokenCount << std::endl;
                return std::nullopt;
            }

            return m_FillInterval - elapsed;
        }
    };
}