#include "expanders.hpp"
#include "myrisa.hpp"
#include "modules/Frame/Messages.hpp"

namespace myrisa {

struct Signal : ExpandableModule<SignalExpanderMessage, MyrisaModule> {
  enum ParamIds { MIX_PARAM, VCA_PARAM, NUM_PARAMS };
  enum InputIds { IN_INPUT, MIX_INPUT, VCA_INPUT, NUM_INPUTS };
  enum OutputIds { SEL_OUTPUT, OUT_OUTPUT, NUM_OUTPUTS };
  enum LightIds { NUM_LIGHTS };

  struct Engine {
    float mix = 0.5f;
    float vca = 0.0f;
    float last_out = 0.0f;
  };

  Engine *_engines[maxChannels] {};

  SignalExpanderMessage *_toFrame = NULL;
  SignalExpanderMessage *_fromFrame = NULL;

  Signal() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(MIX_PARAM, 0.f, 1.f, 0.f, "Mix");
    configParam(VCA_PARAM, 0.f, 1.f, 0.f, "VCA");

    setExpanderModelPredicate([](Model *m) {
      // TODO chainable expanders
      return m == modelFrame;
    });
  }

  void modulateChannel(int c) override;
  void addChannel(int c) override;
  void removeChannel(int c) override;
  int channels() override;
  void processAlways(const ProcessArgs &args) override;
  void processChannel(const ProcessArgs &args, int channel) override;
};

} // namespace myrisa
