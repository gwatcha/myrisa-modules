#include "Section.hpp"

using namespace myrisa;

// FIXME performance
float Section::getLayerAttenuation(int layer_i) {
  float layer_attenuation = 0.0f;
  if (_active_layer) {
    for (auto target_layer : _active_layer->target_layers) {
      if (target_layer == _layers[layer_i]) {
        layer_attenuation += attenuation;
        break;
      }
    }
  }

  if (1.0f <= layer_attenuation) {
    return 1.0f;
  }

  for (unsigned int j = layer_i + 1; j < _layers.size(); j++) {
    for (auto target_layer : _layers[j]->target_layers) {
      if (target_layer == _layers[layer_i]) {
        layer_attenuation += _layers[j]->readSendAttenuation(division, _phase);
        if (1.0f <= layer_attenuation) {
          return 1.0f;
        }
      }
    }
  }

  return layer_attenuation;
}

float Section::read() {
  float out = 0.0f;
  for (unsigned int i = 0; i < _layers.size(); i++) {
    float layer_attenuation = getLayerAttenuation(i);
    if (layer_attenuation < 1.0f) {
      float layer_out = _layers[i]->readSampleWithAttenuation(division, _phase, layer_attenuation);
      out += layer_out;
    }
  }

  return out;
}

float Section::getPhase() {
  return _phase;
}

void Section::advance() {
  float prev_phase = _phase;

  if (use_ext_phase) {
    _phase = ext_phase;
  } else if (_phase_defined) {
    _phase_oscillator.step(sample_time);
    _phase = _phase_oscillator.getPhase();
  } else {
    _phase = 0.0f;
  }

  ASSERT(0.0f, <=, _phase);
  ASSERT(_phase, <=, 1.0f);

  float phase_change = _phase - prev_phase;
  float phase_abs_change = fabs(phase_change);
  bool phase_flip = (phase_abs_change > 0.95 && phase_abs_change <= 1.0);
  if (phase_flip) {
    if (0 < phase_change && 0 < division) {
      division--;
    } else if (phase_change < 0) {
      division++;
    }
  }

  // FIXME has a hard time with internal division loops
  if (use_ext_phase) {
    if (phase_flip && 0 < phase_change) {
      _freq_calculator_last_capture_phase_distance += phase_change - 1;
    } else if (phase_flip && phase_change < 0) {
      _freq_calculator_last_capture_phase_distance += phase_change + 1;
    } else {
      bool in_division_phase_flip =
          (0 < _freq_calculator_last_capture_phase_distance &&
           phase_change < 0) ||
          (_freq_calculator_last_capture_phase_distance < 0 &&
           0 < phase_change);

      if (!in_division_phase_flip) {
        _freq_calculator_last_capture_phase_distance += phase_change;
      }
    }

    if (_ext_phase_freq_calculator.process()) {
      float ext_phase_change = fabs(_freq_calculator_last_capture_phase_distance);
      float phase_change_per_sample = ext_phase_change / _ext_phase_freq_calculator.getDivision();
      if (phase_change_per_sample == 0) {
        return;
      }
      _division_time_s = sample_time / phase_change_per_sample;
      _phase_defined = true;
      _freq_calculator_last_capture_phase_distance = 0;
    }
  }
}

void Section::newLayer(RecordMode layer_mode) {
  if (layer_mode != RecordMode::DEFINE_DIVISION_LENGTH && !_phase_defined) {
    // TODO
    printf("Myrisa Frame Error: New layer that isn't define division, but no _phase defined.\n");
    return;
  }

  // TODO FIXME
  _selected_layers = _layers;


  int samples_per_division = floor(_division_time_s / sample_time);
  printf("NEW LAYER: division: %d, div time s %f sample time %f sapls per %d\n", division, _division_time_s, sample_time, samples_per_division);

  _active_layer = new Layer(layer_mode, division, _selected_layers,
                        samples_per_division);
}

void Section::step() {
  if (_record_mode != RecordMode::READ) {
    assert(_active_layer != NULL);
  }

  bool dub_layer_end = (_active_layer->start_division + _active_layer->n_divisions == division);
  if (_record_mode == RecordMode::DUB && dub_layer_end) {
    printf("END recording via overdub\n");
    printf("-- start div: %d, length: %d\n", _active_layer->start_division, _active_layer->n_divisions);
    _layers.push_back(_active_layer);

    // TODO FIXME
    _selected_layers = _layers;
    newLayer(RecordMode::DUB);
  }

  if (_record_mode != RecordMode::READ) {
    _active_layer->write(division, _phase, in, attenuation);
  }

  advance();
}

RecordMode Section::getMode() {
  return _record_mode;
}

void Section::setMode(RecordMode new_mode) {
  if (_record_mode != RecordMode::READ && new_mode == RecordMode::READ) {
    assert(_active_layer != NULL);
    if (_active_layer->mode == RecordMode::DEFINE_DIVISION_LENGTH) {
      assert(_active_layer->samples_per_division != 0);
      float freq = 1 / (_active_layer->samples_per_division * sample_time);
      _phase_oscillator.setFreq(freq);
      _phase_defined = true;
      printf("_phase defined with pitch %f, s/div %d, s_time %f\n", _phase_oscillator.getFreq(), _active_layer->samples_per_division, sample_time);
      _division_time_s = 1 / _phase_oscillator.getFreq();
    }

    _layers.push_back(_active_layer);

    printf("END recording\n");
    printf("-- mode %d, start div: %d, length: %d\n", _active_layer->mode, _active_layer->start_division, _active_layer->n_divisions);

    _active_layer = NULL;
  }

  if (_record_mode == RecordMode::READ && new_mode != RecordMode::READ) {
    newLayer(new_mode);
  }

  _record_mode = new_mode;
}

bool Section::isEmpty() { return (_layers.size() == 0); }
