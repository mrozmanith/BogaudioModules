#pragma once

#include "bogaudio.hpp"
#include "dsp/envelope.hpp"
#include "dsp/oscillator.hpp"

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

	const int modulationSteps = 100;
	int _steps = 0;
	float _baseHZ = 0.0f;
	float _feedback = 0.0f;
	float _depth = 0.0f;
	float _level = 0.0f;
	bool _levelEnvelopeOn = false;
	bool _feedbackEnvelopeOn = false;
	ADSR _envelope;
	Phasor _phasor;
	SineTableOscillator _sineTable;
	SchmittTrigger _gateTrigger;

	FMOp()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	, _envelope(engineGetSampleRate())
	, _phasor(engineGetSampleRate(), 0.0f)
	, _sineTable(engineGetSampleRate(), 0.0f)
	{
		onReset();
	}

	virtual void onReset() override;
	virtual void onSampleRateChange() override;
	virtual void step() override;
};

} // namespace bogaudio
