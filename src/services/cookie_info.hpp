#pragma once

#include <filesystem>

namespace LambdaSnail::services
{

class CookieInfo
{
public:
    static bool hasCookieFile()
    {
        return std::filesystem::exists(s_CookieFile);
    }

    static std::filesystem::path const& getCookieFile()
    {
        return s_CookieFile;
    }

private:
    static const std::filesystem::path s_CookieFile;
};

}