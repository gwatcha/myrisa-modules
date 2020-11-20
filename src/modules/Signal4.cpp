#include "Signal4.hpp"

void Signal4::processAlways(const ProcessArgs &args) {
  return;
}

struct Signal4Widget : ModuleWidget {
	Signal4Widget(Signal4* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Signal4.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<RoganHalfPSRed>(mm2px(Vec(13.772, 71.584)), module, Signal4::VCA_PARAM));
		addParam(createParam<RoganHalfPSLightPurple>(mm2px(Vec(1.499, 71.609)), module, Signal4::MIX_PARAM));

		addInput(createInput<PJ301MPort>(mm2px(Vec(4.04, 18.394)), module, Signal4::IN_1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(12.859, 18.394)), module, Signal4::IN_2_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(4.04, 27.468)), module, Signal4::IN_3_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(12.859, 27.468)), module, Signal4::IN_4_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(14.672, 82.768)), module, Signal4::VCA_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(2.398, 82.792)), module, Signal4::MIX_INPUT));

		addOutput(createOutput<PJ301MPort>(mm2px(Vec(4.04, 44.348)), module, Signal4::SEL_1_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(12.926, 44.348)), module, Signal4::SEL_2_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(4.04, 53.173)), module, Signal4::SEL_3_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(12.926, 53.173)), module, Signal4::SEL_4_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(4.04, 99.44)), module, Signal4::OUT_1_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(12.926, 99.44)), module, Signal4::OUT_2_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(4.04, 108.576)), module, Signal4::OUT_3_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(12.926, 108.576)), module, Signal4::OUT_4_OUTPUT));
	}
};

Model *modelSignal4 = rack::createModel<Signal4, Signal4Widget>("Myrisa-Signal4");
