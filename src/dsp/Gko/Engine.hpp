#pragma once

#include "Timeline.hpp"
#include "definitions.hpp"
#include "dsp/PhaseAnalyzer.hpp"
#include "dsp/PhaseOscillator.hpp"
#include "dsp/AntipopFilter.hpp"
#include "dsp/Signal.hpp"
#include "Layer.hpp"
#include "rack.hpp"

#include <assert.h>
#include <math.h>
#include <vector>

namespace myrisa {
namespace dsp {
namespace gko {

struct Engine {
  bool _use_ext_phase = false;
  float _ext_phase = 0.f;
  float _sample_time = 1.0f;

  // TODO make me an array to support MIX4 & PLAY
  myrisa::dsp::SignalType _signal_type;

  std::vector<unsigned int> _selected_layers_idx;
  unsigned int _active_layer_i;

  /* read only */

  Layer *_recording_layer = nullptr;
  RecordParams _record_params;

  PhaseOscillator _phase_oscillator;
  PhaseAnalyzer _phase_analyzer;

  Timeline _timeline;
  TimePosition _timeline_position;
  TimeFrame _read_time_frame;

  AntipopFilter _antipop_filter;

  Options _options;

  void step();
  float read();

private:
  inline bool phaseDefined();
  inline void write();
  inline void endRecording();
  inline Layer* newRecording();
  inline void handlePhaseEvent(PhaseAnalyzer::PhaseEvent flip);
  inline PhaseAnalyzer::PhaseEvent advanceTimelinePosition();
};

} // namespace gko
} // namespace dsp
} // namespace myrisa
