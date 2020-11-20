#pragma once

#include "expanders.hpp"

extern Model *modelSignal;
extern Model *modelFrame;

namespace myrisa {

struct SignalExpanderMessage : ExpanderMessage {
  float signal[MyrisaModule::maxChannels]{};
  int n_channels;
};

} // namespace myrisa
