#pragma once

#include <math.h>
#include <vector>

#include "PhaseBuffer.hpp"
#include "definitions.hpp"
#include "rack.hpp"

using namespace std;
using namespace myrisa::util;
using namespace myrisa;

namespace myrisa {

class Layer {
public:
  RecordMode mode;
  int start_division = 0;
  int n_divisions = 0;
  int samples_per_division = 0;

private:
  PhaseBuffer *buffer;
  PhaseBuffer *send_attenuation;

public:
  Layer(RecordMode record_mode, int division, vector<Layer*> selected_layers, int layer_samples_per_division);

  virtual ~Layer() {
    delete buffer;
    delete send_attenuation;
  }

  vector<Layer*> target_layers;

  void write(int division, float phase, float sample, float attenuation);

  float readSample(int division, float phase);

  float readSampleWithAttenuation(int division, float phase, float attenuation);

  float readSendAttenuation(int division, float phase);

private:
  float getBufferPhase(int division, float phase);
};

} // namespace myrisa
