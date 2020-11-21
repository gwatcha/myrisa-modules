#pragma once

#include <tuple>
#include <vector>

#include "Layer.hpp"
#include "PhaseOscillator.hpp"
#include "definitions.hpp"

#include "assert.hpp"
#include "modules/Frame/Messages.hpp"

#include "rack.hpp"
#include <assert.h>

using namespace std;
using namespace myrisa;

namespace myrisa {

class Section {
public:
  int division = 0;
  bool use_ext_phase = false;
  float ext_phase = 0.0f;
  float sample_time = 1.0f;
  float attenuation = 0.0f;
  // TODO vector
  float in = 0.0f;

private:
  float _phase = 0.0f;

  vector<Layer*> _layers;
  vector<Layer*> _selected_layers;

  Layer *_active_layer = NULL;

  PhaseOscillator _phase_oscillator;
  bool _phase_defined = false;

  float _prev_phase = 0.0f;

  rack::dsp::ClockDivider _ext_phase_freq_calculator;
  float _freq_calculator_last_capture_phase_distance = 0.0f;

  float _division_time_s = 0.0f;

  RecordMode _record_mode = RecordMode::READ;

  void newLayer(RecordMode layer_mode);
  void startNewLayer();
  void finishNewLayer();
  float getLayerAttenuation(int layer_i);
  void advance();

public:
  Section() {
    _ext_phase_freq_calculator.setDivision(20000);
  }

  virtual ~Section() {
    for (auto layer : _layers) {
      delete layer;
    }

    if (_active_layer) {
      delete _active_layer;
    }
  }

  float getPhase();
  void setMode(RecordMode new_mode);
  RecordMode getMode();
  bool isEmpty();
  void undo();
  float read();
  void step();
};
} // namespace myrisa
