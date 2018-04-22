#pragma once

#include "bogaudio.hpp"
#include "dsp/envelope.hpp"
#include "dsp/filter.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelFMOp;

namespace bogaudio {

struct FMOp : Module {
	enum ParamsIds {
		RATIO_PARAM,
		FINE_PARAM,
		ATTACK_PARAM,
		DECAY_PARAM,
		SUSTAIN_PARAM,
		RELEASE_PARAM,
		DEPTH_PARAM,
		FEEDBACK_PARAM,
		LEVEL_PARAM,
		ENV_TO_LEVEL_PARAM,
		ENV_TO_FEEDBACK_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		SUSTAIN_INPUT,
		DEPTH_INPUT,
		FEEDBACK_INPUT,
		LEVEL_INPUT,
		PITCH_INPUT,
		GATE_INPUT,
		FM_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		ENV_TO_LEVEL_LIGHT,
		ENV_TO_FEEDBACK_LIGHT,
		NUM_LIGHTS
	};

	const float amplitude = 5.0f;
	const int modulationSteps = 100;
	static constexpr int oversample = 8;
	const float slewLimitTime = 1.0f;
	int _steps = 0;
	float _feedback = 0.0f;
	float _feedbackDelayedSample = 0.0f;
	float _depth = 0.0f;
	float _level = 0.0f;
	bool _envelopeOn = false;
	bool _levelEnvelopeOn = false;
	bool _feedbackEnvelopeOn = false;
	float _maxFrequency = 0.0f;
	float _buffer[oversample];
	ADSR _envelope;
	Phasor _phasor;
	SineTableOscillator _sineTable;
	LPFDecimator _decimator;
	SchmittTrigger _gateTrigger;
	SlewLimiter _feedbackSL;
	SlewLimiter _depthSL;
	SlewLimiter _levelSL;
	SlewLimiter _sustainSL;

	FMOp()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	, _envelope(ADSR::LINEAR_SHAPE)
	{
		onReset();
		onSampleRateChange();
	}

	virtual void onReset() override;
	virtual void onSampleRateChange() override;
	virtual void step() override;
};

} // namespace bogaudio
