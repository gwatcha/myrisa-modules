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
  float phase = 0.0f;

private:
  vector<Layer*> _layers;
  vector<Layer*> _selected_layers;

  Layer *_active_layer = NULL;

  PhaseOscillator _phase_oscillator;
  bool _phase_defined = false;

  float _sample_time = 1.0f;

  rack::dsp::ClockDivider _ext_phase_freq_calculator;
  float _freq_calculator_last_capture_phase_distance = 0.0f;

  bool _use_ext_phase = false;
  float _ext_phase = 0.0f;
  float _division_time_s = 0.0f;

  float _attenuation = 0.0f;

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

  void setMode(RecordMode new_mode);
  RecordMode getMode();
  bool isEmpty();
  void undo();
  float read();
  void step(float in, float attenuation_power, float sample_time, bool use_ext_phase, float ext_phase);
};
} // namespace myrisa
