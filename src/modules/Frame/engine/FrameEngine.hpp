#pragma once

#include "Section.hpp"
#include "definitions.hpp"

namespace myrisa {

class FrameEngine {
public:
  float in = 0.0f;
  float attenuation = 0.0f;
  float sample_time = 1.0f;
  float ext_phase = 0.0f;
  float use_ext_phase = false;

private:
  static const int _numSections = 16;
  bool _recording = false;
  Section *_recording_section = NULL;
  Section* _sections[_numSections];
  float _section_position = 0.0f;

public:
  virtual ~FrameEngine() {
    for (Section* section : _sections) {
      delete section;
    }
  }

  void startRecording();
  void endRecording();
  float step();

  Section* getActiveSection();
  void setSectionPosition(float section_position);
  void setMode(RecordMode mode);

private:
  float read();
};

} // namespace myrisa
