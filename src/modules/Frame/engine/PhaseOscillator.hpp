#pragma once

#include "rack.hpp"
#include <math.h>

namespace myrisa {

class PhaseOscillator {
private:
  float _phase = 0.0f;
  float _freq = 1.0f;

public:
  PhaseOscillator() {}

  void setFreq(float freq);
  float getFreq();
  void reset(float value);
  void step(float dt);
  float getPhase();
};

} // namespace myrisa
