
#include "ADSR.hpp"

void ADSR::onReset() {
	_gateTrigger.reset();
	_envelope.reset();
	_modulationStep = modulationSteps;
}

void ADSR::onSampleRateChange() {
	_envelope.setSampleRate(engineGetSampleRate());
	_modulationStep = modulationSteps;
}

void ADSR::step() {
	lights[LINEAR_LIGHT].value = _linearMode = params[LINEAR_PARAM].value > 0.5f;
	if (!(outputs[OUT_OUTPUT].active || inputs[GATE_INPUT].active)) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		_envelope.setAttack(powf(params[ATTACK_PARAM].value, 2.0f) * 10.f);
		_envelope.setDecay(powf(params[DECAY_PARAM].value, 2.0f) * 10.f);
		_envelope.setSustain(params[SUSTAIN_PARAM].value);
		_envelope.setRelease(powf(params[RELEASE_PARAM].value, 2.0f) * 10.f);
		_envelope.setLinearShape(_linearMode);
	}

	_gateTrigger.process(inputs[GATE_INPUT].value);
	_envelope.setGate(_gateTrigger.isHigh());
	outputs[OUT_OUTPUT].value = _envelope.next() * 10.0f;

	lights[ATTACK_LIGHT].value = _envelope.isStage(bogaudio::dsp::ADSR::ATTACK_STAGE);
	lights[DECAY_LIGHT].value = _envelope.isStage(bogaudio::dsp::ADSR::DECAY_STAGE);
	lights[SUSTAIN_LIGHT].value = _envelope.isStage(bogaudio::dsp::ADSR::SUSTAIN_STAGE);
	lights[RELEASE_LIGHT].value = _envelope.isStage(bogaudio::dsp::ADSR::RELEASE_STAGE);
}

struct ADSRWidget : ModuleWidget {
	ADSRWidget(ADSR* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/ADSR.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto attackParamPosition = Vec(7.5, 32.5);
		auto decayParamPosition = Vec(7.5, 89.5);
		auto sustainParamPosition = Vec(7.5, 146.5);
		auto releaseParamPosition = Vec(7.5, 203.5);
		auto linearParamPosition = Vec(32.0, 245.7);

		auto gateInputPosition = Vec(10.5, 265.0);

		auto outOutputPosition = Vec(10.5, 303.0);

		auto attackLightPosition = Vec(20.8, 65.0);
		auto decayLightPosition = Vec(20.8, 122.0);
		auto sustainLightPosition = Vec(20.8, 179.0);
		auto releaseLightPosition = Vec(20.8, 236.0);
		auto linearLightPosition = Vec(4.0, 247.0);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob29>(attackParamPosition, module, ADSR::ATTACK_PARAM, 0.0, 1.0, 0.12));
		addParam(ParamWidget::create<Knob29>(decayParamPosition, module, ADSR::DECAY_PARAM, 0.0, 1.0, 0.31623));
		addParam(ParamWidget::create<Knob29>(sustainParamPosition, module, ADSR::SUSTAIN_PARAM, 0.0, 1.0, 1.0));
		addParam(ParamWidget::create<Knob29>(releaseParamPosition, module, ADSR::RELEASE_PARAM, 0.0, 1.0, 0.31623));
		addParam(ParamWidget::create<StatefulButton9>(linearParamPosition, module, ADSR::LINEAR_PARAM, 0.0, 1.0, 0.0));

		addInput(Port::create<Port24>(gateInputPosition, Port::INPUT, module, ADSR::GATE_INPUT));

		addOutput(Port::create<Port24>(outOutputPosition, Port::OUTPUT, module, ADSR::OUT_OUTPUT));

		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(attackLightPosition, module, ADSR::ATTACK_LIGHT));
		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(decayLightPosition, module, ADSR::DECAY_LIGHT));
		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(sustainLightPosition, module, ADSR::SUSTAIN_LIGHT));
		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(releaseLightPosition, module, ADSR::RELEASE_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(linearLightPosition, module, ADSR::LINEAR_LIGHT));
	}
};

Model* modelADSR = Model::create<ADSR, ADSRWidget>("Bogaudio", "Bogaudio-ADSR", "ADSR", ENVELOPE_GENERATOR_TAG, UTILITY_TAG);