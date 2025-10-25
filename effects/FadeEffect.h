#ifndef FADE_EFFECT_H
#define FADE_EFFECT_H

#include <memory>
#include <atomic>
#include <huestream/HueStream.h>
#include <huestream/effect/effects/ManualEffect.h>
#include <huestream/common/data/Color.h>

class FadeEffect {
private:
    std::shared_ptr<huestream::HueStream> _huestream;
    std::shared_ptr<huestream::ManualEffect> _currentEffect;

public:
    FadeEffect(std::shared_ptr<huestream::HueStream> huestream);
    ~FadeEffect();
    
    void play(std::atomic<bool>& shutdownRequested);
    void stop();
};

#endif // FADE_EFFECT_H