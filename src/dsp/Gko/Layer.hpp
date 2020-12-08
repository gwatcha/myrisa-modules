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
  unsigned int start_beat = 0;
  unsigned int n_beats = 0;
  bool loop = false;

  // TODO
  // std::vector<Recording*> recordings;
  // std::vector<Recording::Type> types;

  Recording *in;
  Recording *recording_strength;

  std::vector<unsigned int> target_layers_idx;

  std::vector<Layer*> attenuation_layers;
  // unsigned int fully_attenuated_beat = 0;

  int samples_per_beat = 0;

  TimelinePosition last_content_position;
  TimelinePosition fully_attenuated_position;

  // TODO optinoal samples_per_beat?
  inline Layer(unsigned int start_beat, unsigned int n_beats, std::vector<unsigned int> target_layers_idx) {
    this->start_beat = start_beat;
    this->n_beats = n_beats;
    this->target_layers_idx = target_layers_idx;

    fully_attenuated_position.beat = std::numeric_limits<unsigned int>::max();
    in = new Recording(Recording::Type::AUDIO);
    recording_strength = new Recording(Recording::Type::PARAM);
  }

  inline ~Layer() {
    delete in;
    delete recording_strength;
  }

  // FIXME won't work if timeline looping
  inline void tryUpdateFullyAttenuatedPosition(TimelinePosition position) {
    if (attenuation_layers.size() == 0) {
      return;
    }

    unsigned int max_attenuation_layers_beats = 0;
    for (auto attenuation_layer : attenuation_layers) {
      if (attenuation_layer->loop && max_attenuation_layers_beats < attenuation_layer->n_beats) {
        max_attenuation_layers_beats = attenuation_layer->n_beats;
      }
    }

    if (position.beat < last_content_position.beat) {
      return;
    }

    if (max_attenuation_layers_beats < (position.beat - last_content_position.beat)) {
      fully_attenuated_position = position;
      printf("fully attenuated\n");
    }
  }

  inline void pushBack(float signal_sample, float recording_strength_sample) {
    in->pushBack(signal_sample);
    recording_strength->pushBack(recording_strength_sample);
  }

  inline void resizeToLength() {
    int new_size = n_beats * samples_per_beat;
    printf("resizeto %d %d so %d\n", n_beats, samples_per_beat, new_size);
    in->resize(new_size);
    recording_strength->resize(new_size);
  }

  inline bool readableAtPosition(TimelinePosition position) {
    return loop ?
      start_beat <= position.beat :
      start_beat <= position.beat && position.beat < start_beat + n_beats;
  }

  inline bool writableAtPosition(TimelinePosition position) {
    return start_beat <= position.beat && position.beat <= start_beat + n_beats;
  }

  inline float positionToRecordingPhase(TimelinePosition position) {
    assert(readableAtPosition(position));
    assert(0 < n_beats);
    double phase = position.phase;
    unsigned int layer_beat = (position.beat - start_beat) % n_beats;
    phase += layer_beat;
    return phase / n_beats;
  }

  inline float readSignal(TimelinePosition position) {
    if (fully_attenuated_position.before(position) || !readableAtPosition(position)) {
      return 0.f;
    }

    float attenuation = 0.f;
    for (auto layer : attenuation_layers) {
      attenuation += layer->readRecordingStrength(position);
      if (1.0f <= attenuation) {
        return 0.f;
      }
    }

    last_content_position = position;
    return in->read(positionToRecordingPhase(position)) * (1.0f - attenuation);
  }

  inline float readRecordingStrength(TimelinePosition position) {
    if (!readableAtPosition(position)) {
      return 0.f;
    }

    return recording_strength->read(positionToRecordingPhase(position));
  }

  inline void write(TimelinePosition position, float signal_sample, float recording_strength_sample) {
    assert(writableAtPosition(position));
    assert(0 != in->size());
    assert(0 != recording_strength->size());

    in->write(positionToRecordingPhase(position), signal_sample);
    recording_strength->write(positionToRecordingPhase(position), recording_strength_sample);
  }
};

} // namespace frame
} // namespace dsp
} // namespace myrisa
