#include "expanders.hpp"
#include "myrisa.hpp"
#include "modules/Frame/Messages.hpp"

extern Model *modelPlay;

namespace myrisa {

struct Play : MyrisaModule {
	enum ParamIds {
		OUT_POWER_PARAM,
		MIX_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		VEL_INPUT,
		GATE_INPUT,
		VOCT_INPUT,
		OUT_POWER_INPUT,
		MIX_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SEL_VEL_OUTPUT,
		SEL_GATE_OUTPUT,
		SEL_VOCT_OUTPUT,
		VEL_OUTPUT,
		GATE_OUTPUT,
		VOCT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Play() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(OUT_POWER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MIX_PARAM, 0.f, 1.f, 0.f, "");
	}

	void processAlways(const ProcessArgs& args) override;
};

} // namespace myrisa
