#pragma once

#include <math.h>
#include <vector>
#include <assert.h>

#include "dsp/Interpolation.hpp"
#include "definitions.hpp"
#include "rack.hpp"
#include "util/math.hpp"

using namespace std;
using namespace myrisa::util;

namespace myrisa {
namespace dsp {
namespace gko {

struct Recording {

  /* read only */

  enum Type { RECORDING_STRENGTH, AUDIO, PARAM, CV, GATE, VOCT, VEL };
  Type type;
  std::vector<float> buffer;
  rack::dsp::ClockDivider write_divider;

  Recording(Type type) {
    type = type;
    switch (type) {
    case Type::AUDIO: case Type::CV:
      write_divider.setDivision(1);
      break;
    case Type::GATE: case Type::VOCT: case Type::VEL:
      write_divider.setDivision(100); // approx every ~.25ms
      break;
    case Type::PARAM: case Type::RECORDING_STRENGTH:
      write_divider.setDivision(2000); // approx every ~5ms
      break;
    }
  }

  inline void resize(int new_size) {
    int n_samples = new_size / write_divider.getDivision();
    if (n_samples == 0) {
      n_samples++;
    }
    buffer.resize(n_samples);
  }

  inline int size() {
    return buffer.size() * write_divider.getDivision();
  }

  inline void pushBack(float sample) {
    if (size() == 0) {
      write_divider.reset();
      buffer.push_back(sample);
    } else if (write_divider.process()) {
      buffer.push_back(sample);
    }
  }

  inline void write(double phase, float sample) {
    assert(0.f <= phase);
    assert(phase <= 1.0f);
    if (type == Type::PARAM) {
      assert(0.f <= sample);
    }

    int length = buffer.size();
    float position = length * phase;
    int i = floor(position) == length ? length - 1 : floor(position);

    // TODO different more sophisticated ways to write?
    // FIXME explodes if in oscillator mode
    if (type == Type::AUDIO)  {
      int i2 = ceil(position) == length ? 0 : ceil(position);
      float w = position - i;
      buffer[i] += sample * (1 - w);
      buffer[i2] += sample * (w);
    } else {
      buffer[i] = sample;
    }
  }

  inline float getAttenuatedSample(float buffer_sample, float attenuation) {
    float clamped_attenuation = rack::clamp(attenuation, 0.f, 1.0f);
    switch (type) {
    case Type::GATE:
      if (clamped_attenuation == 1.0f) {
        return 0.f;
      } else {
        return buffer_sample;
      }
    case Type::VOCT:
      return buffer_sample;
    default:
      return buffer_sample * (1.0f - clamped_attenuation);
    }
  }

  inline float crossfadeSample(float sample, double phase) {
    const int num_audio_samples_to_fade = 50;
    float fade_length = (float)num_audio_samples_to_fade / (float)buffer.size();
    if (1.0f < fade_length) {
      fade_length = 0.5f;
    }

    if (fade_length <= phase) {
      return sample;
    }

    double fade_amount = rescale(phase, 0.0, fade_length, 0.0);
    return rack::crossfade(buffer[buffer.size()-1], sample, fade_amount);
  }

  inline float read(double phase) {
    assert(0.f <= phase);
    assert(phase <= 1.0);

    int size = buffer.size();
    if (size == 0) {
      return 0.f;
    }

    float buffer_position = size * phase;

    int min_samples_for_interpolation = 4;
    if (size < min_samples_for_interpolation) {
      return buffer[floor(buffer_position)];
    }

    if (type == Type::AUDIO) {
      float interpolated_sample = interpolateBSpline(buffer.data(), buffer_position, buffer.size());
      return crossfadeSample(interpolated_sample, phase);
    } else if (type == Type::CV) {
      return interpolateLineard(buffer.data(), buffer_position, buffer.size());
    } else if (type == Type::PARAM) {
      return rack::clamp(interpolateBSpline(buffer.data(), buffer_position, buffer.size()), 0.0, 1.0);
    } else {
      return buffer[floor(buffer_position)];
    }
  }
};


} // namespace frame
} // namespace dsp
} // namepsace myrisa
