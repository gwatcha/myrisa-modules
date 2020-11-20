#pragma once

#include "Section.hpp"
#include "definitions.hpp"

namespace myrisa {

class FrameEngine {
public:
  float attenuation;
  bool use_ext_phase = false;
  float ext_phase = 0.0f;

private:
  bool _recording = false;
  Section *_recording_section = NULL;
  vector<Section*> _sections;
  float _section_position = 0.0f;

public:
  virtual ~FrameEngine() {
    for (Section* section : _sections) {
      delete section;
    }
  }

  void startRecording();
  void endRecording();
  float step(float in, float sample_time);

  Section* getActiveSection();
  void setSectionPosition(float section_position);
  void setUseExtPhase(bool use_ext_phase);
  void setMode(RecordMode mode);

private:
  float read();
};

} // namespace myrisa
