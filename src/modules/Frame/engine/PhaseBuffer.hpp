#pragma once

#include <math.h>
#include <vector>

#include "Interpolater.hpp"
#include "assert.hpp"
#include "definitions.hpp"
#include "rack.hpp"
#include "util/math.hpp"

using namespace std;
using namespace myrisa::util;

namespace myrisa {

/** a buffer that can be read and written to via a phase float in [0.0f, 1.0f]
 handles interpolation, with behaviour dependant on the type */
class PhaseBuffer {
public:

private:
  BufferType _type;
  vector<float> _buffer;
  // TODO ???
  rack::dsp::ClockDivider _divider;

public:
  PhaseBuffer(BufferType type);

  void resize(int new_size);
  int size();
  void pushBack(float sample);
  BufferType getBufferType();
  float getAttenuatedSample(float buffer_sample, float attenuation);
  void write(float phase, float sample);
  float read(float phase);

private:
  float crossfadeSample(float sample, float phase);
};

} // namepsace myrisa
