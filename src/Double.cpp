
#include "bogaudio.hpp"
#include "dsp/pitch.hpp"

using namespace bogaudio::dsp;

struct Double : Module {
	enum ParamsIds {
		INTERVAL_PARAM,
		OCTAVE_PARAM,
		FINE_PARAM,
		QUANTIZE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		THRU_OUTPUT,
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		QUANTIZE_LIGHT,
		NUM_LIGHTS
	};

	const float _minCVOut = semitoneToCV(24.0); // C1
	const float _maxCVOut = semitoneToCV(120.0); // C9

	Double() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
	}

	virtual void step() override;
};

void Double::step() {
	if (!(outputs[OUT_OUTPUT].active || outputs[THRU_OUTPUT].active)) {
		return;
	}

	float semitones = params[OCTAVE_PARAM].value * 12.0;
	semitones += params[INTERVAL_PARAM].value;
	if (inputs[CV_INPUT].active) {
		semitones += clampf(inputs[CV_INPUT].value, -5.0, 5.0) * 10.0;
	}
	if (params[QUANTIZE_PARAM].value > 0.5) {
		semitones = roundf(semitones);
	}

	if (inputs[IN_INPUT].active) {
		float cv = clampf(inputs[IN_INPUT].value, _minCVOut, _maxCVOut);
		outputs[THRU_OUTPUT].value = cv;
		outputs[OUT_OUTPUT].value = clampf(semitoneToCV(cvToSemitone(cv) + semitones + params[FINE_PARAM].value), _minCVOut, _maxCVOut);
	}
	else {
		outputs[THRU_OUTPUT].value = semitones / 10.0;
		outputs[OUT_OUTPUT].value = clampf(semitoneToCV(referenceSemitone + semitones + params[FINE_PARAM].value), _minCVOut, _maxCVOut);
	}

	lights[QUANTIZE_LIGHT].value = params[QUANTIZE_PARAM].value > 0.5;
}


DoubleWidget::DoubleWidget() {
	Double *module = new Double();
	setModule(module);
	box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Double.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(0, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 15, 365)));

	// generated by svg_widgets.rb
	auto intervalParamPosition = Vec(9.5, 31.5);
	auto octaveParamPosition = Vec(14.5, 82.5);
	auto fineParamPosition = Vec(14.5, 119.5);
	auto quantizeParamPosition = Vec(26.9, 183.9);

	auto cvInputPosition = Vec(10.5, 149.0);
	auto inInputPosition = Vec(10.5, 205.0);

	auto thruOutputPosition = Vec(10.5, 243.0);
	auto outOutputPosition = Vec(10.5, 279.0);

	auto quantizeLightPosition = Vec(21.0, 187.0);
	// end generated by svg_widgets.rb

	{
		auto w = createParam<Knob26>(intervalParamPosition, module, Double::INTERVAL_PARAM, 0.0, 11.0, 0.0);
		dynamic_cast<Knob*>(w)->snap = true;
		addParam(w);
	}
	{
		auto w = createParam<Knob16>(octaveParamPosition, module, Double::OCTAVE_PARAM, -3.0, 3.0, 0.0);
		auto k = dynamic_cast<SVGKnob*>(w);
		k->snap = true;
		k->minAngle = -0.5 * M_PI;
		k->maxAngle = 0.5 * M_PI;
		addParam(w);
	}
	addParam(createParam<Knob16>(fineParamPosition, module, Double::FINE_PARAM, -0.99, 0.99, 0.0));
	addParam(createParam<StatefulButton9>(quantizeParamPosition, module, Double::QUANTIZE_PARAM, 0.0, 1.0, 1.0));

	addInput(createInput<Port24>(cvInputPosition, module, Double::CV_INPUT));
	addInput(createInput<Port24>(inInputPosition, module, Double::IN_INPUT));

	addOutput(createOutput<Port24>(thruOutputPosition, module, Double::THRU_OUTPUT));
	addOutput(createOutput<Port24>(outOutputPosition, module, Double::OUT_OUTPUT));

	addChild(createLight<TinyLight<GreenLight>>(quantizeLightPosition, module, Double::QUANTIZE_LIGHT));
}