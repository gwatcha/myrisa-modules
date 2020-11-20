#include "PhaseBuffer.hpp"

using namespace std;
using namespace myrisa;

PhaseBuffer::PhaseBuffer(BufferType type) {
  _type = _type;
  switch (_type) {
  case BufferType::AUDIO: case BufferType::CV:
    _divider.setDivision(1);
    break;
  case BufferType::GATE: case BufferType::VOCT: case BufferType::VEL:
    _divider.setDivision(100); // approx every ~.25ms
    break;
  case BufferType::PARAM:
    _divider.setDivision(2000); // approx every ~5ms
    break;
  }
}

BufferType PhaseBuffer::getBufferType() {
  return _type;
}

void PhaseBuffer::resize(int new_size) {
  _buffer.resize(new_size / _divider.getDivision());
}

int PhaseBuffer::size() {
  return _buffer.size() * _divider.getDivision();
}

void PhaseBuffer::pushBack(float sample) {
  if (size() == 0) {
    _divider.reset();
    _buffer.push_back(sample);
  } else if (_divider.process()) {
    _buffer.push_back(sample);
  }
}

void PhaseBuffer::write(float phase, float sample) {
  ASSERT(0.0f, <=, phase);
  ASSERT(phase, <=, 1.0f);
  if (_type == BufferType::PARAM) {
    ASSERT(0.0f, <=, sample);
  }

  if (_divider.process()) {
    int length = _buffer.size();
    float position = length * phase;
    int i = floor(position) == length ? length - 1 : floor(position);

    // TODO different more sophisticated ways to write?
    // FIXME explodes if in oscillator mode
    if (_type == BufferType::AUDIO)  {
      int i2 = ceil(position) == length ? 0 : ceil(position);
      float w = position - i;
      _buffer[i] += sample * (1 - w);
      _buffer[i2] += sample * (w);
    } else {
      _buffer[i] = sample;
    }
  }
}

float PhaseBuffer::getAttenuatedSample(float buffer_sample, float attenuation) {
  float clamped_attenuation = rack::clamp(attenuation, 0.0f, 1.0f);

  switch (_type) {
  case BufferType::GATE:
    if (clamped_attenuation == 1.0f) {
      return 0.0f;
    } else {
      return buffer_sample;
    }
  case BufferType::VOCT:
    return buffer_sample;
  default:
    return buffer_sample * (1.0f - clamped_attenuation);
  }
}

float PhaseBuffer::crossfadeSample(float sample, float phase) {
  const int num_audio_samples_to_fade = 50;
  float fade_length = (float)num_audio_samples_to_fade / (float)_buffer.size();
  if (1.0f < fade_length) {
    fade_length = 0.5f;
  }

  if (fade_length <= phase) {
    return sample;
  }

  float fade_amount = rescale(phase, 0.0f, fade_length, 0.0f);
  return rack::crossfade(_buffer[_buffer.size()-1], sample, fade_amount);
}

float PhaseBuffer::read(float phase) {
  ASSERT(0.0f, <=, phase);
  ASSERT(phase, <=, 1.0f);

  int size = _buffer.size();
  if (size == 0) {
    return 0.0f;
  }

  float buffer_position = size * phase;
  int min_samples_for_interpolation = 4;
  if (size < min_samples_for_interpolation) {
    return _buffer[floor(buffer_position)];
  }

  if (_type == BufferType::AUDIO) {
    // float interpolated_sample = InterpolateHermite(_buffer.data(),
    float interpolated_sample = Interpolater::interpolate(Interpolater::Method::HERMITE, _buffer, buffer_position);
    return crossfadeSample(interpolated_sample, phase);
  } else if (_type == BufferType::CV) {
    return Interpolater::interpolate(Interpolater::Method::LINEAR, _buffer, buffer_position);
  } else if (_type == BufferType::PARAM) {
    return Interpolater::interpolate(Interpolater::Method::BSPLINE, _buffer, buffer_position);
  } else {
    return Interpolater::interpolate(Interpolater::Method::NONE, _buffer, buffer_position);
  }
}
