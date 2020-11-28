#pragma once

#include "Frame_shared.hpp"
#include "dsp/Frame/Engine.hpp"
#include "dsp/LongPressButton.hpp"
#include "widgets.hpp"
#include <math.h>

namespace myrisa {

struct Frame : ExpanderModule<SignalExpanderMessage, MyrisaModule> {
  enum ParamIds {
    SELECT_PARAM,
    SELECT_MODE_PARAM,
    SELECT_FUNCTION_PARAM,
    TIME_FRAME_PARAM,
    DELTA_CONTEXT_PARAM,
    DELTA_MODE_PARAM,
    DELTA_PARAM,
    NUM_PARAMS
  };
  enum InputIds { SCENE_INPUT, DELTA_INPUT, PHASE_INPUT, NUM_INPUTS };
  enum OutputIds { PHASE_OUTPUT, NUM_OUTPUTS };
  enum LightIds {
    ENUMS(SELECT_FUNCTION_LIGHT, 3),
    ENUMS(SELECT_MODE_LIGHT, 3),
    ENUMS(TIME_FRAME_LIGHT, 3),
    ENUMS(DELTA_LIGHT, 3),
    ENUMS(DELTA_MODE_LIGHT, 3),
    ENUMS(DELTA_CONTEXT_LIGHT, 3),
    ENUMS(PHASE_LIGHT, 3),
    NUM_LIGHTS
  };

  SignalExpanderMessage *_to_signal = nullptr;
  SignalExpanderMessage *_from_signal = nullptr;

  Delta::Mode _delta_mode = Delta::Mode::DUB;
  Delta::Context _delta_context = Delta::Context::SCENE;
  TimeFrame _time_frame_mode = TimeFrame::TIME;

  const float _recordThreshold = 0.05f;
  float _sampleTime = 1.0f;

  LongPressButton _delta_mode_button;
  LongPressButton _delta_context_button;
  LongPressButton _time_frame_button;

  std::array<myrisa::dsp::frame::Engine*, maxChannels> _engines;
  rack::dsp::ClockDivider _light_divider;
  rack::dsp::ClockDivider _button_divider;

  Frame();

  void sampleRateChange() override;
  int channels() override;
  void modulateChannel(int c) override;
  void addChannel(int c) override;
  void removeChannel(int c) override;
  void processAlways(const ProcessArgs &args) override;
  void processChannel(const ProcessArgs &args, int channel) override;
  void postProcessAlways(const ProcessArgs &args) override;
  void updateLights(const ProcessArgs &args);

private:
  void processButtons();
};

} // namespace myrisa

