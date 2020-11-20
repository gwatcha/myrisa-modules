#include "PhaseOscillator.hpp"

using namespace myrisa;

void PhaseOscillator::setFreq(float freq) {
  _freq = freq;
}

float PhaseOscillator::getFreq() {
  return _freq;
}

void PhaseOscillator::reset(float value) {
  _phase = std::fmod(value, 1.0f);
}

void PhaseOscillator::step(float dt) {
  float delta_phase = _freq * dt;
  float sum = _phase + delta_phase;
  _phase = rack::math::eucMod(sum, 1.0f);
}

float PhaseOscillator::getPhase() {
  return _phase;
}
