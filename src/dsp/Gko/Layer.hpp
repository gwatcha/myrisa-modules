#pragma once

#include "definitions.hpp"
#include "Recording.hpp"

namespace myrisa {
namespace dsp {
namespace gko {

/**
  A Layer is one row in the Posline.
*/
struct Layer {
  float start = 0;
  float length = 0;
  bool loop = false;

  // TODO
  // std::vector<Recording*> recordings;
  // std::vector<Recording::Type> types;

  Recording *signal;
  Recording *manifestation_strength;
  std::vector<int> target_layers_idx;

  int samples_per_beat = 0;

  // TODO optinoal samples_per_beat?
  inline Layer(float start, float length, std::vector<int> target_layers_idx) {
    this->start = start;
    this->length = length;
    this->target_layers_idx = target_layers_idx;

    signal = new Recording(Recording::Type::AUDIO);
    manifestation_strength = new Recording(Recording::Type::PARAM);
  }

  inline ~Layer() {
    delete signal;
    delete manifestation_strength;
  }

  inline void pushBack(float signal_sample, float manifestation_strength_sample) {
    signal->pushBack(signal_sample);
    manifestation_strength->pushBack(manifestation_strength_sample);
  }

  inline void resizeToLength() {
    int new_size = length * samples_per_beat;
    signal->resize(new_size);
    manifestation_strength->resize(new_size);
  }

  inline bool readableAtTime(float time) {
    return loop ?
      start <= time && time <= start + length :
      start <= time;
  }

  inline bool writableAtTime(float time) {
    return start <= time && time <= start + length;
  }

  inline float timeToRecordingPhase(float time) {
    assert(readableAtTime(time));
    float time_in_recording = std::fmod(time - start, length);
    return time_in_recording / length;
  }

  inline float readSignal(float time) {
    if (!readableAtTime(time)) {
      return 0.f;
    }

    return signal->read(timeToRecordingPhase(time));
  }


  inline float readManifestationStrength(float time) {
    if (!readableAtTime(time)) {
      return 0.f;
    }

    return manifestation_strength->read(timeToRecordingPhase(time));
  }

  inline void write(float time, float signal_sample, float manifestation_strength_sample) {
    assert(writableAtTime(time));
    assert(0 < signal->size());
    assert(0 < manifestation_strength->size());

    signal->write(timeToRecordingPhase(time), signal_sample);
    manifestation_strength->write(timeToRecordingPhase(time), manifestation_strength_sample);
  }
};

} // namespace frame
} // namespace dsp
} // namespace myrisa