#include "Engine.hpp"

using namespace myrisa::dsp::gko;

// TODO just read rendered timeline
float Engine::read() {
  if (_timeline.layers.size() == 0) {
    return 0.f;
  }

  float timeline_out;
  if (_options.eco_mode) {
    timeline_out = _timeline.rendered_timeline->readSignal(_timeline_position);
  } else {
    timeline_out = _timeline.readLayers(_timeline_position);
  }

  // return timeline_out;
  if (_options.use_antipop) {
    timeline_out = _antipop_filter.process(timeline_out);
  }

  return timeline_out;
}
