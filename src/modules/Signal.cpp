#include "Signal.hpp"

int Signal::channels() {
  // TODO define # channels based on FRAME out layers may be recorded with more
  // or less channels also, additional behaviours with mono or stereo
  // duplication via pos & rate will increase channels
  return inputs[IN_INPUT].getChannels();
}

void Signal::modulateChannel(int c) {
  Engine &e = *_engines[c];
  e.mix = params[MIX_PARAM].getValue();
  if (inputs[MIX_INPUT].isConnected()) {
    e.mix *= clamp(inputs[MIX_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
  }

  e.vca = params[VCA_PARAM].getValue();
  if (inputs[VCA_INPUT].isConnected()) {
    e.vca *= clamp(inputs[VCA_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
  }
}

void Signal::processAlways(const ProcessArgs &args) {
  outputs[OUT_OUTPUT].setChannels(_channels);
  outputs[SEL_OUTPUT].setChannels(_channels);
}

void Signal::processChannel(const ProcessArgs& args, int c) {
  Engine &e = *_engines[c];

  // TODO mix knob with SEL
  float in = inputs[IN_INPUT].getPolyVoltage(c) * e.mix;

  float out = in;
  if (expanderConnected()) {
    auto toFrame = toExpander();
    auto fromFrame = fromExpander();

    toFrame->signal[c] = in;
    toFrame->channels = _channels;
    out += fromFrame->signal[c];
  }

  if (outputs[OUT_OUTPUT].isConnected()) {
    outputs[OUT_OUTPUT].setVoltage(out * e.vca, c);
  }
}

void Signal::addChannel(int c) {
  _engines[c] = new Engine();
}

void Signal::removeChannel(int c) {
  delete _engines[c];
  _engines[c] = NULL;
}

struct SignalWidget : ModuleWidget {
	SignalWidget(Signal* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Signal.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<RoganHalfPSLightPurple>(mm2px(Vec(2.687, 50.584)), module, Signal::MIX_PARAM));
		addParam(createParam<RoganHalfPSRed>(mm2px(Vec(2.687, 77.502)), module, Signal::VCA_PARAM));

		addInput(createInput<PJ301MPort>(mm2px(Vec(3.69, 17.88)), module, Signal::IN_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(3.586, 61.767)), module, Signal::MIX_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(3.586, 88.685)), module, Signal::VCA_INPUT));

		addOutput(createOutput<PJ301MPort>(mm2px(Vec(3.516, 32.077)), module, Signal::SEL_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(3.516, 108.703)), module, Signal::OUT_OUTPUT));
	}
};

Model* modelSignal = rack::createModel<Signal, SignalWidget>("Myrisa-Signal");
