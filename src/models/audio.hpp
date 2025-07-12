#pragma once
#include <memory>

namespace LambdaSnail::music
{
struct AudioAnalysis {
    double acousticness{};
    double danceability{};
    double energy{};
    double instrumentalness{};
    double liveness{};
    double loudness{};
    double speechiness{};
    double tempo{};
    double valence{};
};

struct AudioInformation
{
    AudioAnalysis data{};
    std::string name{};
};
}