#include "Engine.hpp"

using namespace myrisa::dsp::gko;
using namespace myrisa::dsp;

inline bool Engine::phaseDefined() {
  return _use_ext_phase || _phase_oscillator.isSet();
}

// modifies engine
inline void Engine::endRecording() {
    assert(_recording_layer != nullptr);
    assert(_recording_layer->_n_beats != 0.f);

    printf("-- start_beat %d n_beats %d  loop %d samples_per_beat %d\n", _recording_layer->_start_beat, _recording_layer->_n_beats,  _recording_layer->_loop, _recording_layer->_in->_samples_per_beat);

    if (!_phase_oscillator.isSet()) {
      if (_use_ext_phase && _phase_analyzer.getDivisionPeriod() != 0) {
        _phase_oscillator.setFrequency(1 / _phase_analyzer.getDivisionPeriod());
      } else {
        float recording_time = _recording_layer->_in->_samples_per_beat * _sample_time;
        _phase_oscillator.setFrequency(1 / recording_time);
      }
      printf("-- phase oscillator set with frequency: %f, sample time is: %f\n", _phase_oscillator.getFrequency(), _sample_time);
    }

    _timeline.layers.push_back(_recording_layer);
    _timeline.last_calculated_attenuation.resize(_timeline.layers.size());
    _timeline.current_attenuation.resize(_timeline.layers.size());

    _recording_layer = nullptr;
}

// does not modify engine
inline Layer* Engine::newRecording() {
  assert(_record_params.active());
  assert(_recording_layer == nullptr);

  unsigned int n_beats = 1;
  if (_record_params.mode == RecordParams::Mode::DUB && 0 < _selected_layers_idx.size()) {
    n_beats = _timeline.layers[_active_layer_i]->_n_beats;
  }

  unsigned int start_beat = _timeline_position.beat;

  int samples_per_beat = 0;
  if (phaseDefined()) {
    if (_use_ext_phase) {
      samples_per_beat = _phase_analyzer.getSamplesPerBeat(_sample_time);
    } else if (_phase_oscillator.isSet()) {
      float beat_period = 1 / _phase_oscillator.getFrequency();
      samples_per_beat = floor(beat_period / _sample_time);
    }
  }

  Layer* recording_layer = new Layer(start_beat, n_beats, _selected_layers_idx, _signal_type, samples_per_beat);

  if (_record_params.time_frame != TimeFrame::TIMELINE) {
    recording_layer->_loop = true;
  }

  printf("Recording Activate:\n");
  printf("-- start_beat %d n_beats %d loop %d samples per beat %d\n", recording_layer->_start_beat, recording_layer->_n_beats, recording_layer->_loop, recording_layer->_in->_samples_per_beat);

  return recording_layer;
}

/*
 * Alters recording layer of engine.
 */
inline void Engine::handlePhaseFlip(PhaseAnalyzer::PhaseFlip flip) {
  assert(phaseDefined());

  if (_recording_layer) {
    assert(_recording_layer != nullptr);
    assert(_recording_layer->_in->_samples_per_beat != 0);

    bool reached_recording_end = _recording_layer->_start_beat + _recording_layer->_n_beats <= _timeline_position.beat;
    if (reached_recording_end) {
      if (_record_params.mode == RecordParams::Mode::DUB) {
      printf("DUB END\n");
        endRecording();
        _recording_layer = newRecording();
      } else if (_record_params.mode == RecordParams::Mode::EXTEND) {
        _recording_layer->_n_beats++;
        printf("extend recording to: %d\n", _recording_layer->_n_beats);
      }
    }
  }
}

inline PhaseAnalyzer::PhaseFlip Engine::advanceTimelinePosition() {
  float internal_phase = _phase_oscillator.step(_sample_time);
  _timeline_position.phase = _use_ext_phase ? _ext_phase : internal_phase;

  PhaseAnalyzer::PhaseFlip phase_flip = _phase_analyzer.process(_timeline_position.phase, _sample_time);
  if (phase_flip == PhaseAnalyzer::PhaseFlip::BACKWARD && 1 <= _timeline_position.beat) {
    _timeline_position.beat--;
  } else if (phase_flip == PhaseAnalyzer::PhaseFlip::FORWARD) {
    _timeline_position.beat++;
  }

  return phase_flip;
}

void Engine::step() {
  bool phase_defined = this->phaseDefined();

  if (phase_defined) {
    PhaseAnalyzer::PhaseFlip phase_flip = this->advanceTimelinePosition();
    if (phase_flip != PhaseAnalyzer::PhaseFlip::NONE) {
      this->handlePhaseFlip(phase_flip);
    }
  }

  if (!_recording_layer && _record_params.active()) {
    _recording_layer = this->newRecording();
  } else if (_recording_layer && !_record_params.active()) {
    this->endRecording();
  }

  if (_recording_layer) {
    _recording_layer->write(_timeline_position, _record_params, phase_defined);
  }
}
