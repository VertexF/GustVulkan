#include "PreComp.h"
#include "Random.h"

std::mt19937 Random::_randomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::_distribution;