#include "Engine.hpp"

using namespace myrisa::dsp::gko;
using namespace myrisa::dsp;

inline bool Engine::phaseDefined() {
  return _use_ext_phase || _phase_oscillator.isSet();
}

inline void Engine::write() {
  assert(_record_params.active());
  assert(_recording_layer != nullptr);

  if (!phaseDefined()) {
    _recording_layer->pushBack(_record_params.in, _record_params.strength);
    _recording_layer->samples_per_beat++;

    // TODO ?
    _timeline.rendered_timeline->pushBack(_record_params.in, 0.f);
    _timeline.rendered_timeline->samples_per_beat++;

  } else {
    assert(_recording_layer->writableAtPosition(_timeline_position));
    _recording_layer->writeSignal(_timeline_position, _record_params.in);
    _recording_layer->writeRecordingStrength(_timeline_position, _record_params.strength);

    if (_timeline_position.before(_timeline.render_head)) {
      _timeline.render_head = _timeline_position;
    }
  }
}

inline void Engine::endRecording() {
    assert(_recording_layer != nullptr);
    assert(_recording_layer->n_beats != 0.f);
    assert(_recording_layer->in->size() != 0);
    assert(_recording_active);

    float recording_time = _recording_layer->in->size() * _sample_time;

    printf("Recording De-Activate\n");
    printf("-- start_beat %d n_beats %d size %d recording time %fs loop %d samples_per_beat %d\n", _recording_layer->start_beat, _recording_layer->n_beats, _recording_layer->in->size(), recording_time, _recording_layer->loop, _recording_layer->samples_per_beat);

    if (!_phase_oscillator.isSet()) {
      if (_use_ext_phase && _phase_analyzer.getDivisionPeriod() != 0) {
        _phase_oscillator.setFrequency(1 / _phase_analyzer.getDivisionPeriod());
      } else {
        _phase_oscillator.setFrequency(1 / recording_time);
      }
      printf("-- phase oscillator set with frequency: %f, sample time is: %f\n", _phase_oscillator.getFrequency(), _sample_time);
      _timeline.render_head.beat = 1;
      _timeline.render_head.phase = 0.0f;
    }

    _timeline.layers.push_back(_recording_layer);

    _recording_active = false;
    _recording_layer = nullptr;
}

inline void Engine::beginRecording() {
  assert(_record_params.active());
  assert(_recording_layer == nullptr);


  unsigned int n_beats = 1;
  if (_record_params.mode == RecordParams::Mode::DUB && 0 < _selected_layers_idx.size()) {
    n_beats = _timeline.getNumberOfBeatsOfLayerSelection(_selected_layers_idx);
  }

  unsigned int start_beat = _timeline_position.beat;

  std::vector<unsigned int> target_layers_idx;
  for (auto selected_layer_i : _selected_layers_idx) {
    TimelinePosition fully_attenuated_position = _timeline.layers[selected_layer_i]->fully_attenuated_position;
    if (_timeline_position.before(fully_attenuated_position)) {
      target_layers_idx.push_back(selected_layer_i);
    }
  }

  _recording_layer = new Layer(start_beat, n_beats, _selected_layers_idx, _sample_time);
  for (auto target_layer_id : _selected_layers_idx) {
    _timeline.layers[target_layer_id]->attenuation_layers.push_back(_recording_layer);
  }

  if (phaseDefined()) {
    if (_use_ext_phase) {
      _recording_layer->samples_per_beat = _phase_analyzer.getSamplesPerDivision();
    } else if (_phase_oscillator.isSet()) {
      float beat_period = 1 / _phase_oscillator.getFrequency();
      _recording_layer->samples_per_beat = floor(beat_period / _sample_time);
    }
    _recording_layer->resizeToLength();
  }

  if (_timeline.rendered_timeline == nullptr) {
    _timeline.rendered_timeline = new Layer(0, _recording_layer->start_beat + 1, std::vector<unsigned int>(), _sample_time);
    _timeline.rendered_timeline->samples_per_beat = _recording_layer->samples_per_beat;
    _timeline.rendered_timeline->resizeToLength();
  }

  if (_record_params.time_frame != TimeFrame::TIMELINE) {
    _recording_layer->loop = true;
  }

  _recording_active = true;

  printf("Recording Activate:\n");
  printf("-- start_beat %d n_beats %d loop %d samples per beat %d\n", _recording_layer->start_beat, _recording_layer->n_beats, _recording_layer->loop, _recording_layer->samples_per_beat);
}

inline void Engine::handlePhaseFlip(PhaseAnalyzer::PhaseFlip flip) {
  assert(phaseDefined());

  if (_recording_layer) {
    assert(_recording_layer != nullptr);
    assert(_recording_layer->samples_per_beat != 0);

    bool reached_recording_end = _recording_layer->start_beat + _recording_layer->n_beats <= _timeline_position.beat;
    if (reached_recording_end) {
      if (_record_params.mode == RecordParams::Mode::DUB) {
      printf("DUB END\n");
        endRecording();
        beginRecording();
      } else if (_record_params.mode == RecordParams::Mode::EXTEND) {
        _recording_layer->n_beats++;
        printf("extend recording to: %d\n", _recording_layer->n_beats);
        _recording_layer->resizeToLength();
      }
    }
  }

  if (flip == PhaseAnalyzer::PhaseFlip::FORWARD) {
    for (auto layer: _timeline.layers) {
      if (_timeline_position.before(layer->fully_attenuated_position)) {
        layer->tryUpdateFullyAttenuatedPosition(_timeline_position);
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

  if (phaseDefined() && 0 < _timeline.layers.size() && _timeline.render_head.before(_timeline_position)) {
    _timeline.renderUpTo(_timeline_position);
  }

  return phase_flip;
}

void Engine::step() {
  if (phaseDefined()) {
    PhaseAnalyzer::PhaseFlip phase_flip = advanceTimelinePosition();
    if (phase_flip != PhaseAnalyzer::PhaseFlip::NONE) {
      handlePhaseFlip(phase_flip);
    }
  }

  if (_recording_active != _record_params.active()) {
    if (!_recording_active) {
      beginRecording();
    } else {
      endRecording();
    }
  }

  if (_recording_active) {
    write();
  }
}
