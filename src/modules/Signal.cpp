#include "Signal.hpp"
#include "SignalWidget.hpp"

int Signal::channels() {
  // TODO define # channels based on Gko layers, may be recorded with more
  // or less channels also, additional behaviours with mono or stereo
  // duplication via pos & rate will increase channels
  // best to just send # of channels from Gko
  return inputs[IN_INPUT].getChannels();
}


void Signal::modulateChannel(int c) {
  Engine &e = *_engines[c];
  e.in_attenuation = params[IN_ATTENUATION_PARAM].getValue();
  if (inputs[IN_ATTENUATION_INPUT].isConnected()) {
    e.in_attenuation *= rack::clamp(inputs[IN_ATTENUATION_INPUT].getPolyVoltage(c) / 10.f, 0.f, 1.0f);
  }
  e.in_attenuation = 1.0f - e.in_attenuation;

  e.out_attenuation = params[OUT_ATTENUATION_PARAM].getValue();
  if (inputs[OUT_ATTENUATION_INPUT].isConnected()) {
    e.out_attenuation *= rack::clamp(inputs[OUT_ATTENUATION_INPUT].getPolyVoltage(c) / 10.f, 0.f, 1.0f);
  }
  e.out_attenuation = 1.0f - e.out_attenuation;
}

void Signal::processAlways(const ProcessArgs &args) {
  outputs[OUT_OUTPUT].setChannels(_channels);
  outputs[SEL_OUTPUT].setChannels(_channels);
}

void Signal::processChannel(const ProcessArgs& args, int c) {
  Engine &e = *_engines[c];

  float in = inputs[IN_INPUT].getPolyVoltage(c);
  in = myrisa::dsp::attenuate(in, e.in_attenuation, _gko_connection->signal_type);

  _gko_connection->to[c] = in;

  if (outputs[OUT_OUTPUT].isConnected()) {
    float out = myrisa::dsp::sum(_gko_connection->from[c], in, _gko_connection->signal_type);
    out = myrisa::dsp::attenuate(out, e.out_attenuation, _gko_connection->signal_type);
    outputs[OUT_OUTPUT].setVoltage(out, c);
  }
}

void Signal::addChannel(int c) {
  _engines[c] = new Engine();
  _gko_connection->send_channels++;
}

void Signal::removeChannel(int c) {
  delete _engines[c];
  _engines[c] = nullptr;
  _gko_connection->send_channels--;
}

Model* modelSignal = rack::createModel<Signal, SignalWidget>("Myrisa-Signal");
