
#include "dsp/digital.hpp"
#include "BogaudioModules.hpp"

struct SampleHold : Module {
	enum ParamIds {
		TRIGGER1_PARAM,
		TRIGGER2_PARAM,
		NUM_PARAMS
	};

	enum InputIds {
		TRIGGER1_INPUT,
		IN1_INPUT,
		TRIGGER2_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};

	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	SchmittTrigger _trigger1, _trigger2;
	float _value1, _value2;

	SampleHold() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {
		reset();
	}

  virtual void reset() override;
  virtual void step() override;
	void step(
		Param& triggerParam,
		Input& triggerInput,
		Input& in,
		Output& out,
		SchmittTrigger& trigger,
		float& value
	);
};

void SampleHold::reset() {
	_trigger1.reset();
	_value1 = 0.0;
	_trigger2.reset();
	_value1 = 0.0;
}

void SampleHold::step() {
	step(
		params[TRIGGER1_PARAM],
		inputs[TRIGGER1_INPUT],
		inputs[IN1_INPUT],
		outputs[OUT1_OUTPUT],
		_trigger1,
		_value1
	);

	step(
		params[TRIGGER2_PARAM],
		inputs[TRIGGER2_INPUT],
		inputs[IN2_INPUT],
		outputs[OUT2_OUTPUT],
		_trigger2,
		_value2
	);
}

void SampleHold::step(
	Param& triggerParam,
	Input& triggerInput,
	Input& in,
	Output& out,
	SchmittTrigger& trigger,
	float& value
) {
	if (trigger.process(triggerParam.value + triggerInput.value)) {
		if (in.active) {
			value = in.value;
		}
		else {
			value = randomf() * 10.0;
		}
	}
	out.value = value;
}

SampleHoldWidget::SampleHoldWidget() {
	SampleHold *module = new SampleHold();
	setModule(module);
	box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/SampleHold.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(0, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 15, 365)));

	// generated by svg_widgets.rb
	auto trigger1ParamPosition = Vec(13.5, 33.0);
	auto trigger2ParamPosition = Vec(13.5, 193.0);

	auto trigger1InputPosition = Vec(10.5, 56.0);
	auto in1InputPosition = Vec(10.5, 93.0);
	auto trigger2InputPosition = Vec(10.5, 216.0);
	auto in2InputPosition = Vec(10.5, 253.0);

	auto out1OutputPosition = Vec(10.5, 131.0);
	auto out2OutputPosition = Vec(10.5, 291.0);
	// end generated by svg_widgets.rb

	addParam(createParam<Button18>(trigger1ParamPosition, module, SampleHold::TRIGGER1_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<Button18>(trigger2ParamPosition, module, SampleHold::TRIGGER2_PARAM, 0.0, 1.0, 0.0));

	addInput(createInput<PJ301MPort>(trigger1InputPosition, module, SampleHold::TRIGGER1_INPUT));
	addInput(createInput<PJ301MPort>(in1InputPosition, module, SampleHold::IN1_INPUT));
	addInput(createInput<PJ301MPort>(trigger2InputPosition, module, SampleHold::TRIGGER2_INPUT));
	addInput(createInput<PJ301MPort>(in2InputPosition, module, SampleHold::IN2_INPUT));

	addOutput(createOutput<PJ301MPort>(out1OutputPosition, module, SampleHold::OUT1_OUTPUT));
	addOutput(createOutput<PJ301MPort>(out2OutputPosition, module, SampleHold::OUT2_OUTPUT));
}