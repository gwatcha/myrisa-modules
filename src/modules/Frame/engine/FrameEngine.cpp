#include "FrameEngine.hpp"

using namespace myrisa;

Section* FrameEngine::getActiveSection() {
  return _sections[round(_section_position)];
}

void FrameEngine::setMode(RecordMode mode) {
  _recording_section = getActiveSection();
  if (mode == RecordMode::READ) {
    _recording = false;
    _recording_section->setMode(RecordMode::READ);
  } else {
    _recording = true;
    _recording_section = getActiveSection();
    _recording_section->setMode(RecordMode::DEFINE_DIVISION_LENGTH);
    _recording_section->setMode(mode);
  }
}

void FrameEngine::setSectionPosition(float section_position) {
  _section_position = section_position;
  unsigned int active_section_i = round(_section_position);

  if (_sections.size() <= active_section_i) {
    _sections.resize(active_section_i + 1);
  }

  Section *active_section = _sections[active_section_i];
  if (!active_section) {
    active_section = new Section();
    _sections[active_section_i] = active_section;
  }

  if (_recording && _recording_section != active_section) {
    RecordMode record_mode = _recording_section->getMode();
    _recording_section->setMode(RecordMode::READ);
    _recording_section = active_section;
    _recording_section->setMode(record_mode);
  }
}

float FrameEngine::step(float in, float sample_time) {
  ASSERT(attenuation, <=, 1.0f);
  ASSERT(0.0f, <=, attenuation);

  for (auto section : _sections) {
    if (section) {
      section->step(in, attenuation, sample_time, use_ext_phase, ext_phase);
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

  if (_sections[section_2] && section_1 != section_2 && section_2 < _sections.size()) {
    out += _sections[section_2]->read() * (weight);
  }

  return out;
}
