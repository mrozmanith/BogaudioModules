#pragma once

#include "bogaudio.hpp"
#include "mixer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMix1;

namespace bogaudio {

struct Mix1 : Module {
	enum ParamsIds {
		LEVEL_PARAM,
		MUTE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		MUTE_INPUT,
		LEVEL_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	MixerChannel _channel;

	Mix1()
	:  _channel(
		params[LEVEL_PARAM],
		params[LEVEL_PARAM], // not used
		params[MUTE_PARAM],
		inputs[IN_INPUT],
		inputs[LEVEL_INPUT],
		inputs[LEVEL_INPUT], // not used
		1000.0f,
		&inputs[MUTE_INPUT]
	)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LEVEL_PARAM, 0.0f, 1.0f, fabsf(MixerChannel::minDecibels, "level");
		configParam(MUTE_PARAM, 0.0f, 1.0f, 0.0f, "mute");

		onSampleRateChange();
	}

	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio