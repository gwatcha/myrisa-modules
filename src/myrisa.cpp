#include "myrisa.hpp"

#include "modules/Gko.hpp"
#include "modules/Signal.hpp"
#include "modules/Signal4.hpp"
#include "modules/Play.hpp"

Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;

  p->addModel(modelSignal);
  p->addModel(modelGko);
  p->addModel(modelSignal4);
  p->addModel(modelPlay);
}
