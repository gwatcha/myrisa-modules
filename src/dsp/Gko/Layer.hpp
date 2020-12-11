#pragma once

#include "definitions.hpp"
#include "Recording.hpp"
#include "dsp/Signal.hpp"

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
  // std::vector<myrisa::dsp::SignalType> types;

  Recording *in;
  Recording *recording_strength;

  // FIXME change me to be an array of bools for O(1) lookup
  std::vector<unsigned int> target_layers_idx;

  int samples_per_beat = 0;

  // TODO optinoal samples_per_beat?
  inline Layer(unsigned int start_beat, unsigned int n_beats, std::vector<unsigned int> target_layers_idx, myrisa::dsp::SignalType signal_type) {
    this->start_beat = start_beat;
    this->n_beats = n_beats;
    this->target_layers_idx = target_layers_idx;

    in = new Recording(signal_type);
    recording_strength = new Recording(myrisa::dsp::SignalType::PARAM);
  }

  inline ~Layer() {
    delete in;
    delete recording_strength;
  }

  inline void resizeToLength() {
    int new_size = n_beats * samples_per_beat;
    printf("resizeto %d %d so %d\n", n_beats, samples_per_beat, new_size);
    in->resize(new_size);
    recording_strength->resize(new_size);
  }

  inline bool readableAtPosition(TimelinePosition timeline_position) {
    return loop ?
      start_beat <= timeline_position.beat :
      start_beat <= timeline_position.beat && timeline_position.beat < start_beat + n_beats;
  }

  inline bool writableAtPosition(TimelinePosition timeline_position) {
    return start_beat <= timeline_position.beat && timeline_position.beat <= start_beat + n_beats;
  }

  inline float positionToRecordingPhase(TimelinePosition timeline_position) {
    assert(readableAtPosition(timeline_position));
    assert(0 < n_beats);
    double phase = timeline_position.phase;
    unsigned int layer_beat = (timeline_position.beat - start_beat) % n_beats;
    phase += layer_beat;
    return phase / n_beats;
  }

  inline float readSignal(TimelinePosition timeline_position) {
    if (!readableAtPosition(timeline_position)) {
      return 0.f;
    }

    return in->read(positionToRecordingPhase(timeline_position));
  }

  inline float readRecordingStrength(TimelinePosition timeline_position) {
    if (!readableAtPosition(timeline_position)) {
      return 0.f;
    }

    return recording_strength->read(positionToRecordingPhase(timeline_position));
  }

  inline void write(TimelinePosition timeline_position, RecordParams record_params, bool phase_defined) {
    if (!phase_defined) {
      in->pushBack(record_params.in);
      recording_strength->pushBack(record_params.strength);
      samples_per_beat++;
    } else if (writableAtPosition(timeline_position)) {
      assert(0 != in->size());
      assert(0 != recording_strength->size());

      in->write(positionToRecordingPhase(timeline_position), record_params.in);
   recording_strength->write(positionToRecordingPhase(timeline_position), record_params.strength);
    }
  }
};

} // namespace gko
} // namespace dsp
} // namespace myrisa
