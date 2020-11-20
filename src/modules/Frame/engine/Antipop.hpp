#pragma once

namespace myrisa {

struct AntipopFilter {

    float alpha_ = 0.00001f;
    float filter_;

    inline void trigger() {
        alpha_ = 0.0f;
    }

    inline float process(float in) {
        if (alpha_ >= 1.0f) {
            filter_ = in;
            return in;
        }

        alpha_ += 3000.0 / 44100.0; // recovers in ~30 samples

        filter_ += alpha_ * (in - filter_);

        return filter_;
    }
};

} // namespace myrisa
