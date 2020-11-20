#include "Frame.hpp"

void Frame::sampleRateChange() {
  _sampleTime = APP->engine->getSampleTime();
}

void Frame::processAlways(const ProcessArgs &args) {
  if (baseConnected()) {
    _fromSignal = fromBase();
    _toSignal = toBase();
  }
}

float getAttenuationValue(float delta, float recording_threshold) {
  float read_position = 0.50f;
  float linear_attenuation_power;
  if (delta < read_position - recording_threshold) {
    linear_attenuation_power = read_position - recording_threshold - delta;
  } else if (delta > read_position - recording_threshold) {
    linear_attenuation_power = delta - read_position + recording_threshold;
  } else {
    linear_attenuation_power = 0.0f;
  }

  float range = read_position - recording_threshold;
  float linear_attenuation_power_scaled = linear_attenuation_power / range;

  // I found that taking to the power 3 gives the most intuitive attenuation
  // power curve
  float attenuation_power = clamp(pow(linear_attenuation_power_scaled, 3), 0.0f, 1.0f);
  return attenuation_power;
}

void Frame::modulateChannel(int c) {
  FrameEngine &e = *_engines[c];

  float delta = params[DELTA_PARAM].getValue();
  if (inputs[DELTA_INPUT].isConnected()) {
    delta *= clamp(inputs[DELTA_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
  }
  if (delta > 0.50f + _recordThreshold) {
    e.setMode(RecordMode::EXTEND);
  } else if (delta < 0.50f - _recordThreshold) {
    e.setMode(RecordMode::DUB);
  } else {
    e.setMode(RecordMode::READ);
  }

  float section_position = params[SECTION_PARAM].getValue() * (16 - 1);
  if (inputs[SECTION_INPUT].isConnected()) {
    section_position += clamp(inputs[SECTION_INPUT].getPolyVoltage(c), -5.0f, 5.0f);
  }
  section_position = clamp(section_position, 0.0f, 15.0f);
  e.setSectionPosition(section_position);

  e.attenuation = getAttenuationValue(delta, _recordThreshold);
}

int Frame::channels() {
  if (baseConnected()) {
    int input_channels = _fromSignal->channels;
    if (_channels < input_channels) {
      return input_channels;
    }
  }
  return _channels;
}

void Frame::processChannel(const ProcessArgs& args, int c) {
  if (!baseConnected()) {
    return;
  }

  FrameEngine &e = *_engines[c];

  if (inputs[CLK_INPUT].isConnected()) {
    float ext_phase = clamp(inputs[CLK_INPUT].getPolyVoltage(c) / 10, 0.0f, 1.0f);
    e.use_ext_phase = true;
    e.ext_phase = ext_phase;
  } else {
    e.use_ext_phase = false;
  }

  float in = _fromSignal->signal[c];
  _toSignal->signal[c] = e.step(in, _sampleTime);
}

void Frame::updateLights(const ProcessArgs &args) {
  FrameEngine &e = *_engines[0];
  Section* active_section = e.getActiveSection();
  float phase = active_section->phase;

  lights[PHASE_LIGHT + 1].setSmoothBrightness(phase, _sampleTime * light_divider.getDivision());

  float attenuation_power = e.attenuation;
  RecordMode active_section_mode = active_section->getMode();

  if (active_section_mode == RecordMode::READ) {
    lights[RECORD_MODE_LIGHT + 0].value = 0.0;
    lights[RECORD_MODE_LIGHT + 1].value = 0.0;
    lights[RECORD_MODE_LIGHT + 2].value = 0.0;
  } else if (active_section_mode == RecordMode::EXTEND) {
    lights[RECORD_MODE_LIGHT + 0].setSmoothBrightness(
        1.0f - attenuation_power, _sampleTime * light_divider.getDivision());
    lights[RECORD_MODE_LIGHT + 1].value = 0.0;
    lights[RECORD_MODE_LIGHT + 2].value = 0.0;
  } else if (active_section_mode == RecordMode::DUB) {
    lights[RECORD_MODE_LIGHT + 0].setSmoothBrightness(
        1.0f - attenuation_power, _sampleTime * light_divider.getDivision());
    lights[RECORD_MODE_LIGHT + 1].setSmoothBrightness(
        1.0f - attenuation_power, _sampleTime * light_divider.getDivision());
    lights[RECORD_MODE_LIGHT + 2].value = 0.0;
  } else if (active_section_mode == RecordMode::DEFINE_DIVISION_LENGTH) {
    lights[RECORD_MODE_LIGHT + 0].value = 0.0;
    lights[RECORD_MODE_LIGHT + 1].value = 0.0;
    lights[RECORD_MODE_LIGHT + 2].value = 1.0;
  }
}

void Frame::postProcessAlways(const ProcessArgs &args) {
  if (light_divider.process()) {
    updateLights(args);
  }
}

void Frame::addChannel(int c) {
    _engines[c] = new FrameEngine();
}

void Frame::removeChannel(int c) {
  delete _engines[c];
  _engines[c] = NULL;
}

struct FrameWidget : ModuleWidget {
  static constexpr int hp = 4;

  FrameWidget(Frame *module) {
    setModule(module);
    box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Frame.svg")));

		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<Rogan4PSGray>(mm2px(Vec(2.247, 18.399)), module, Frame::SECTION_PARAM));
		addParam(createParam<TL1105>(mm2px(Vec(7.365, 48.24)), module, Frame::PLAY_PARAM));
		addParam(createParam<TL1105>(mm2px(Vec(13.909, 48.28)), module, Frame::NEXT_PARAM));
		addParam(createParam<TL1105>(mm2px(Vec(0.848, 48.282)), module, Frame::PREV_PARAM));
		addParam(createParam<TL1105>(mm2px(Vec(3.485, 71.081)), module, Frame::UNDO_PARAM));
		addParam(createParam<TL1105>(mm2px(Vec(11.408, 71.22)), module, Frame::RECORD_MODE_PARAM));
		addParam(createParam<Rogan3PBlue>(mm2px(Vec(2.74, 81.455)), module, Frame::DELTA_PARAM));

		addInput(createInput<PJ301MPort>(mm2px(Vec(5.79, 34.444)), module, Frame::SECTION_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(5.79, 96.94)), module, Frame::DELTA_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(5.905, 109.113)), module, Frame::CLK_INPUT));

		addChild(createLight<MediumLight<RedGreenBlueLight>>(mm2px(Vec(8.528, 63.611)), module, Frame::PHASE_LIGHT));
		addChild(createLight<MediumLight<RedGreenBlueLight>>(mm2px(Vec(8.542, 67.836)), module, Frame::RECORD_MODE_LIGHT));

		// mm2px(Vec(18.593, 7.115))
		addChild(createWidget<Widget>(mm2px(Vec(0.758, 54.214))));
  }
};

Model *modelFrame = rack::createModel<Frame, FrameWidget>("Myrisa-Frame");
