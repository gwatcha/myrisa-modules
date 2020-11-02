#pragma once

#include "Frame_IO.hpp"

namespace myrisa {

struct Frame : ExpanderModule<SignalExpanderMessage, MyrisaModule> {
	enum ParamIds {
		SCENE_PARAM,
		PLAY_PARAM,
		NEXT_PARAM,
		PREV_PARAM,
		UNDO_PARAM,
		RECORD_MODE_PARAM,
		DELTA_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		SCENE_INPUT,
		DELTA_INPUT,
		CLK_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		DELTA_MODE_LIGHT,
		RECORD_MODE_LIGHT,
		NUM_LIGHTS
	};

  SignalExpanderMessage *_toSignal = NULL;
  SignalExpanderMessage *_fromSignal = NULL;

  struct Engine {
    struct Layer {
      std::vector<float> buffer;
      std::vector<float> attenuation_envelope;
      std::vector<int> attenuation_target_layers;
      int layer_number;
      float phase;
    };

   Layer* layers[]{};
  };

  Engine *_engines[maxChannels]{};

  Frame() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SCENE_PARAM, 0.f, 1.f, 0.f, "Scene");
		configParam(PLAY_PARAM, 0.f, 1.f, 0.f, "Play Layer");
		configParam(NEXT_PARAM, 0.f, 1.f, 0.f, "Next Layer");
		configParam(PREV_PARAM, 0.f, 1.f, 0.f, "Prev Layer");
		configParam(UNDO_PARAM, 0.f, 1.f, 0.f, "Undo");
		configParam(RECORD_MODE_PARAM, 0.f, 1.f, 0.f, "Record Mode");
		configParam(DELTA_PARAM, 0.f, 1.f, 0.f, "Delta");

    // TODO chainable
    setBaseModelPredicate([](Model *m) { return m == modelSignal; });
 }

  void modulateChannel(int c) override;
  void addChannel(int c) override;
  void removeChannel(int c) override;
  int channels() override;
  void processAlways(const ProcessArgs &args) override;
  void processChannel(const ProcessArgs &args, int channel) override;
};

} // namespace myrisa
