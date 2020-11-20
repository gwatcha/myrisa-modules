#include "Play.hpp"

void Play::processAlways(const ProcessArgs &args) { return; }

struct PlayWidget : ModuleWidget {
	PlayWidget(Play* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Play.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<RoganHalfPSRed>(mm2px(Vec(13.772, 71.608)), module, Play::OUT_POWER_PARAM));
		addParam(createParam<RoganHalfPSLightPurple>(mm2px(Vec(1.499, 71.633)), module, Play::MIX_PARAM));

		addInput(createInput<PJ301MPort>(mm2px(Vec(8.771, 17.295)), module, Play::VEL_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(3.788, 29.78)), module, Play::GATE_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(13.339, 29.78)), module, Play::VOCT_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(14.672, 82.791)), module, Play::OUT_POWER_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(2.398, 82.816)), module, Play::MIX_INPUT));

		addOutput(createOutput<PJ301MPort>(mm2px(Vec(8.444, 43.303)), module, Play::SEL_VEL_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(3.286, 55.431)), module, Play::SEL_GATE_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(13.582, 55.431)), module, Play::SEL_VOCT_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(8.444, 96.627)), module, Play::VEL_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(3.286, 108.576)), module, Play::GATE_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(13.582, 108.576)), module, Play::VOCT_OUTPUT));
	}
};

Model *modelPlay = rack::createModel<Play, PlayWidget>("Myrisa-Play");
