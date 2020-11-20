#include "myrisa.hpp"

#include "modules/Frame/Frame.hpp"
#include "modules/Signal.hpp"
#include "modules/Signal4.hpp"
#include "modules/Play.hpp"

Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;

  p->addModel(modelSignal);
  p->addModel(modelFrame);
  p->addModel(modelSignal4);
  p->addModel(modelPlay);
}
