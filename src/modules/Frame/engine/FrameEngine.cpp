#include "FrameEngine.hpp"

using namespace myrisa;

Section* FrameEngine::getActiveSection() {
  unsigned int active_section_i = round(_section_position);
  Section *active_section = _sections[active_section_i];
  if (!active_section) {
    active_section = new Section();
    _sections[active_section_i] = active_section;
  }
  return active_section;
}

void FrameEngine::setMode(RecordMode mode) {
  _recording_section = getActiveSection();
  if (mode == RecordMode::READ) {
    _recording = false;
    _recording_section->setMode(RecordMode::READ);
  } else {
    _recording = true;
    if (_recording_section->isEmpty() && !use_ext_phase) {
      _recording_section->setMode(RecordMode::DEFINE_DIVISION_LENGTH);
    } else  {
      _recording_section->setMode(mode);
    }
  }
}

void FrameEngine::setSectionPosition(float section_position) {
  _section_position = section_position;
  Section *active_section = getActiveSection();

  // this allows for quickly finishing recording, switching to a new section, and starting recording
  if (_recording && _recording_section != active_section) {
    RecordMode record_mode = _recording_section->getMode();
    _recording_section->setMode(RecordMode::READ);
    _recording_section = active_section;
    _recording_section->setMode(record_mode);
  }
}

float FrameEngine::step() {
  ASSERT(attenuation, <=, 1.0f);
  ASSERT(0.0f, <=, attenuation);

  for (auto section : _sections) {
    if (section) {
      // TODO is this a good way to do things?
      // answer: nested classes
      section->use_ext_phase = use_ext_phase;
      section->ext_phase = ext_phase;
      section->attenuation = attenuation;
      section->sample_time = sample_time;
      section->in = in;
      section->step();
    }
  }

  return read();
}

float FrameEngine::read() {
  int section_1 = floor(_section_position);
  int section_2 = ceil(_section_position);
  float weight = _section_position - floor(_section_position);

  float out = 0.0f;
  if (_sections[section_1]) {
    out += _sections[section_1]->read() * (1 - weight);
  }

  if (_sections[section_2] && section_1 != section_2 && section_2 < _numSections) {
    out += _sections[section_2]->read() * (weight);
  }

  return out;
}
