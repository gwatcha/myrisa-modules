#pragma once

#include "Layer.hpp"
#include "definitions.hpp"
#include <vector>

namespace myrisa {
namespace dsp {
namespace gko {

/**
   A Timeline is the top level structure for content.
*/
struct Timeline {
  Layer* rendered_timeline = nullptr;
  TimelinePosition render_head;
  std::vector<Layer*> layers;

  inline float readLayers(TimelinePosition position) {
    float signal_out = 0.f;
    for (unsigned int i = 0; i < layers.size(); i++) {
      if (layers[i]->readableAtPosition(position)) {
        signal_out += layers[i]->readSignal(position);
      }
    }

    return signal_out;
  }


  inline void renderUpTo(TimelinePosition position) {
    assert(render_head.before(position));

    while(render_head.before(position)) {
      bool reached_render_end = rendered_timeline->n_beats <= render_head.beat;
      if (reached_render_end) {
        rendered_timeline->n_beats++;
        rendered_timeline->resizeToLength();
      }

      rendered_timeline->writeSignal(render_head, readLayers(render_head));

      render_head.phase += 1.0f / rendered_timeline->samples_per_beat;
      if (1.0f <= render_head.phase) {
        render_head.phase -= 1.0f;
        render_head.beat++;
      }
    }

    render_head = position;
  }

  inline std::vector<Layer*> getLayersFromIdx(std::vector<unsigned int> layer_idx) {
    std::vector<Layer*> selected_layers;
    for (auto layer_id : layer_idx) {
      selected_layers.push_back(layers[layer_id]);
    }
    return selected_layers;
  }

  inline float getNumberOfBeatsOfLayerSelection(std::vector<unsigned int> layer_idx) {
    assert(layers.size() != 0);
    assert(layer_idx.size() != 0);
    assert(layer_idx.size() <= layers.size());

    unsigned int max_n_beats = 0;

    for (auto layer : getLayersFromIdx(layer_idx)) {
      if (max_n_beats < layer->n_beats) {
        max_n_beats = layer->n_beats;
      }
    }

    return max_n_beats;
  }
};

} // namespace frame
} // namespace dsp
} // namespace myrisa
