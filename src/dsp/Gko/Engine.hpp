#pragma once

#include "Timeline.hpp"
#include "utils.hpp"
#include "definitions.hpp"
#include "dsp/PhaseAnalyzer.hpp"
#include "dsp/PhaseOscillator.hpp"
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
  std::vector<int> _selected_layers;

  /* read only */

  Manifest _manifest;
  Layer *_manifestation = nullptr;

  PhaseOscillator _phase_oscillator;
  PhaseAnalyzer _phase_analyzer;

  TimeFrame _time_frame;

  Timeline _timeline;
  float _time = 0.f;

  const float _manifestActiveThreshold = 0.0001f;

  void step();
  float read();

  void setManifestMode(Manifest::Mode mode);
  void setManifestTimeFrame(TimeFrame time_frame);
  void setManifestStrength(float strength);

private:
  void manifest();
  inline void beginManifestation();
  inline void endManifestation();
  inline PhaseAnalyzer::PhaseFlip advanceTimelinePosition();
  inline void handlePhaseFlip();
};

} // namespace frame
} // namespace dsp
} // namespace myrisa