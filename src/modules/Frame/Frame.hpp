#pragma once

#include "Messages.hpp"
#include "engine/FrameEngine.hpp"

#include "myrisa.hpp"

using namespace std;

namespace myrisa {

struct Frame : ExpanderModule<SignalExpanderMessage, MyrisaModule> {
public:
	enum ParamIds {
		SECTION_PARAM,
		PLAY_PARAM,
		NEXT_PARAM,
		PREV_PARAM,
		UNDO_PARAM,
		RECORD_MODE_PARAM,
		DELTA_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		SECTION_INPUT,
		DELTA_INPUT,
		CLK_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(PHASE_LIGHT, 3),
		ENUMS(RECORD_MODE_LIGHT, 3),
		NUM_LIGHTS
	};

private:
  static constexpr float _recordThreshold = 0.05f;

  SignalExpanderMessage *_toSignal = NULL;
  SignalExpanderMessage *_fromSignal = NULL;

  float _sampleTime = 1.0f;
  FrameEngine *_engines[maxChannels]{};
  dsp::ClockDivider light_divider;

public:
  Frame() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(SECTION_PARAM, 0.f, 1.f, 0.f, "Section");
    configParam(PLAY_PARAM, 0.f, 1.f, 0.f, "Play Layer");
    configParam(NEXT_PARAM, 0.f, 1.f, 0.f, "Next Layer");
    configParam(PREV_PARAM, 0.f, 1.f, 0.f, "Prev Layer");
    configParam(UNDO_PARAM, 0.f, 1.f, 0.f, "Undo");
    configParam(RECORD_MODE_PARAM, 0.f, 1.f, 0.f, "Record Mode");
    configParam(DELTA_PARAM, 0.f, 1.f, 0.5f, "Delta");
    // TODO more
    setBaseModelPredicate([](Model *m) { return m == modelSignal; });
    light_divider.setDivision(16);
  }

  void sampleRateChange() override;
  int channels() override;
  void modulateChannel(int c) override;
  void addChannel(int c) override;
  void removeChannel(int c) override;
  void processAlways(const ProcessArgs &args) override;
  void processChannel(const ProcessArgs &args, int channel) override;
  void postProcessAlways(const ProcessArgs &args) override;

private:
  bool deltaEngaged();
  void updateLights(const ProcessArgs &args);
};

} // namespace myrisa

