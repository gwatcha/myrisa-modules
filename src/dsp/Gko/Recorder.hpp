#pragma once

#include "dsp/AntipopFilter.hpp"
#include "definitions.hpp"
#include "dsp/PhaseAnalyzer.hpp"
#include "Layer.hpp"
#include "Channel.hpp"

namespace myrisa {
namespace dsp {
namespace gko {

class Recorder {
public:
  /** read only */
  Layer *_recording_layer = nullptr;
  AntipopFilter _write_antipop_filter;

public:
  inline bool isRecording() {
    return _recording_layer != nullptr;
  }

  inline Layer* endRecording(const TimePositionAdvancer &time_position_advancer, const LayerManager &layer_manager) {
    assert(isRecording());
    assert(_recording_layer->_n_beats != 0);

    printf("- rec end\n");
    printf("-- start_beat %d n_beats %d  loop %d samples_per_beat %d\n", _recording_layer->_start_beat, _recording_layer->_n_beats,  _recording_layer->_loop, _recording_layer->_in->_samples_per_beat);


    if (!time_position_advancer.isPhaseOscillatorSet())) {
      float period = time_position_advancer.getBeatPeriod();
      if (period == 0.f) {
        period = _recording_layer->time_length;
      }

      time_position_advancer.setPhaseOscillatorFrequency(1 / period);
      printf("-- phase oscillator set with period: %f\n", period);
    }

    layer_manager.add_layer(_recording_layer);

    unsigned int layer_i = _timeline.layers.size() - 1;
    if (_select_new_layers) {
      _selected_layers_idx.push_back(layer_i);
    }

    if (_new_layer_active) {
      _active_layer_i = layer_i;
    }

    _recording_layer = nullptr;
  }

  inline void newRecording(const TimePosition &timeline_position, const Interface &interface, int samples_per_beat) {
    assert(record_interface.active());
    assert(_recording_layer == nullptr);

    _write_antipop_filter.trigger();

    unsigned int n_beats = 1;
    if (interface.record_interface.mode == RecordInterface::Mode::DUB && 0 < _timeline.layers.size()) {
      n_beats = _timeline.layers[_active_layer_i]->_n_beats;
    }

    unsigned int start_beat = _timeline_position.beat;

    // TODO
    _recording_layer = new Layer(start_beat, n_beats, interface.selected_layers_idx, _signal_type, samples_per_beat);

    if (interface.record_interface.time_frame != TimeFrame::TIMELINE) {
      _recording_layer->_loop = true;
    }

    printf("Recording Activate:\n");
    printf("-- start_beat %d n_beats %d loop %d samples per beat %d active layer %d\n", _recording_layer->_start_beat, _recording_layer->_n_beats, _recording_layer->_loop, _recording_layer->_in->_samples_per_beat, _active_layer_i);
  }

  inline void step(const TimePosition timeline_position, const RecordInterface &record_interface) {
    if (_recording_layer) {
      float in =  _write_antipop_filter.process(record_interface.in);
      _recording_layer->write(timeline_position, in, record_interface.strength, phase_defined);
    }
  }

  inline void growRecording(int n_beats) {
    _recording_layer->_n_beats++;
  }

  inline bool pastRecordingEnd(TimePosition timeline_position) {
    return _recording_layer->_start_beat + _recording_layer->_n_beats <= timeline_position.beat;
  }

  inline void handlePhaseEvent(PhaseAnalyzer::PhaseEvent event, const RecordInterface &record_interface) {
    bool phase_flip = (event == PhaseAnalyzer::PhaseEvent::FORWARD || event == PhaseAnalyzer::PhaseEvent::BACKWARD);
    if (_recording_layer && phase_flip) {
      assert(_recording_layer != nullptr);
      assert(_recording_layer->_in->_samples_per_beat != 0);

      bool reached_recording_end = _recording_layer->_start_beat + _recording_layer->_n_beats <= _timeline_position.beat;
      if (reached_recording_end) {
        if (record_interface.mode == RecordInterface::Mode::DUB) {
          printf("DUB END\n");
          this->endRecording();
          this->newRecording();
        } else if (record_interface.mode == RecordInterface::Mode::EXTEND) {
          _recording_layer->_n_beats++;
          printf("extend recording to: %d\n", _recording_layer->_n_beats);
        }
      }
      // TODO more modes
    }
  }
};

} // namespace gko
} // namespace dsp
} // namepsace myrisa
