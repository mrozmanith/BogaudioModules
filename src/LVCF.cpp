
#include "LVCF.hpp"

#define POLES_KEY "poles"
#define BANDWIDTH_MODE_KEY "bandwidthMode"
#define LINEAR_BANDWIDTH_MODE_KEY "linear"
#define PITCH_BANDWIDTH_MODE_KEY "pitched"

void LVCF::Engine::setParams(
	int poles,
	MultimodeFilter::Mode mode,
	float frequency,
	float qbw,
	MultimodeFilter::BandwidthMode bwm
) {
	frequency = semitoneToFrequency(_frequencySL.next(frequencyToSemitone(frequency)));

	_filter.setParams(
		_sampleRate,
		MultimodeFilter::BUTTERWORTH_TYPE,
		poles,
		mode,
		frequency,
		qbw,
		bwm
	);
}

void LVCF::Engine::sampleRateChange(int modulationSteps) {
	_sampleRate = APP->engine->getSampleRate();
	_frequencySL.setParams(_sampleRate, 100.0f / (float)modulationSteps, frequencyToSemitone(maxFrequency - minFrequency));
	_finalHP.setParams(_sampleRate, MultimodeFilter::BUTTERWORTH_TYPE, 2, MultimodeFilter::HIGHPASS_MODE, 80.0f, BOGAUDIO_DSP_MULTIMODEFILTER_MINQBW);
}

void LVCF::Engine::reset() {
	_filter.reset();
}

float LVCF::Engine::next(float sample) {
	return _finalHP.next(_filter.next(sample));
}

json_t* LVCF::dataToJson() {
	json_t* root = json_object();

	json_object_set_new(root, POLES_KEY, json_integer(_polesSetting));

	switch (_bandwidthMode) {
		case MultimodeFilter::LINEAR_BANDWIDTH_MODE: {
			json_object_set_new(root, BANDWIDTH_MODE_KEY, json_string(LINEAR_BANDWIDTH_MODE_KEY));
			break;
		}
		case MultimodeFilter::PITCH_BANDWIDTH_MODE: {
			json_object_set_new(root, BANDWIDTH_MODE_KEY, json_string(PITCH_BANDWIDTH_MODE_KEY));
			break;
		}
		default: {}
	}

	return root;
}

void LVCF::dataFromJson(json_t* root) {
	json_t* p = json_object_get(root, POLES_KEY);
	if (p) {
		_polesSetting = clamp(json_integer_value(p), 1, 12);
	}

	json_t* bwm = json_object_get(root, BANDWIDTH_MODE_KEY);
	if (bwm) {
		if (strcmp(json_string_value(bwm), LINEAR_BANDWIDTH_MODE_KEY) == 0) {
			_bandwidthMode = MultimodeFilter::LINEAR_BANDWIDTH_MODE;
		}
		else {
			_bandwidthMode = MultimodeFilter::PITCH_BANDWIDTH_MODE;
		}
	}
}

void LVCF::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange(_modulationSteps);
	}
}

bool LVCF::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

int LVCF::channels() {
	return inputs[IN_INPUT].getChannels();
}

void LVCF::addChannel(int c) {
	_engines[c] = new Engine();
}

void LVCF::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void LVCF::modulate() {
	MultimodeFilter::Mode mode = (MultimodeFilter::Mode)(1 + clamp((int)params[MODE_PARAM].getValue(), 0, 4));
	if (_mode != mode || _poles != _polesSetting) {
		_mode = mode;
		_poles = _polesSetting;
		for (int c = 0; c < _channels; ++c) {
			_engines[c]->reset();
		}
	}

	_q = clamp(params[Q_PARAM].getValue(), 0.0f, 1.0f);
}

void LVCF::modulateChannel(int c) {
	Engine& e = *_engines[c];

	float q = _q;
	float f = clamp(params[FREQUENCY_PARAM].getValue(), 0.0f, 1.0f);
	f *= f;
	if (inputs[FREQUENCY_CV_INPUT].isConnected()) {
		float fcv = clamp(inputs[FREQUENCY_CV_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		fcv *= clamp(params[FREQUENCY_CV_PARAM].getValue(), -1.0f, 1.0f);
		f = std::max(0.0f, f + fcv);
	}
	f *= maxFrequency;
	f = clamp(f, minFrequency, maxFrequency);

	e.setParams(
		_poles,
		_mode,
		f,
		q,
		_bandwidthMode
	);
}

void LVCF::processAlways(const ProcessArgs& args) {
	lights[LOWPASS_LIGHT].value = _mode == MultimodeFilter::LOWPASS_MODE;
	lights[HIGHPASS_LIGHT].value = _mode == MultimodeFilter::HIGHPASS_MODE;
	lights[BANDPASS_LIGHT].value = _mode == MultimodeFilter::BANDPASS_MODE;
	lights[BANDREJECT_LIGHT].value = _mode == MultimodeFilter::BANDREJECT_MODE;
}

void LVCF::processAll(const ProcessArgs& args) {
	outputs[OUT_OUTPUT].setChannels(_channels);
}

void LVCF::processChannel(const ProcessArgs& args, int c) {
	outputs[OUT_OUTPUT].setVoltage(_engines[c]->next(inputs[IN_INPUT].getVoltage(c)), c);
}

struct LVCFWidget : ModuleWidget {
	static constexpr int hp = 3;

	LVCFWidget(LVCF* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LVCF.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(9.5, 39.0);
		auto frequencyCvParamPosition = Vec(14.5, 93.5);
		auto qParamPosition = Vec(9.5, 138.0);
		auto modeParamPosition = Vec(18.0, 204.0);

		auto inInputPosition = Vec(10.5, 228.0);
		auto frequencyCvInputPosition = Vec(10.5, 263.0);

		auto outOutputPosition = Vec(10.5, 301.0);

		auto lowpassLightPosition = Vec(3.0, 181.0);
		auto bandpassLightPosition = Vec(3.0, 194.0);
		auto highpassLightPosition = Vec(25.0, 181.0);
		auto bandrejectLightPosition = Vec(25.0, 194.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(frequencyParamPosition, module, LVCF::FREQUENCY_PARAM));
		addParam(createParam<Knob16>(frequencyCvParamPosition, module, LVCF::FREQUENCY_CV_PARAM));
		addParam(createParam<Knob26>(qParamPosition, module, LVCF::Q_PARAM));
		addParam(createParam<StatefulButton9>(modeParamPosition, module, LVCF::MODE_PARAM));

		addInput(createInput<Port24>(inInputPosition, module, LVCF::IN_INPUT));
		addInput(createInput<Port24>(frequencyCvInputPosition, module, LVCF::FREQUENCY_CV_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, LVCF::OUT_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(lowpassLightPosition, module, LVCF::LOWPASS_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(bandpassLightPosition, module, LVCF::BANDPASS_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(highpassLightPosition, module, LVCF::HIGHPASS_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(bandrejectLightPosition, module, LVCF::BANDREJECT_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		LVCF* m = dynamic_cast<LVCF*>(module);
		assert(m);
		menu->addChild(new MenuLabel());

		OptionsMenuItem* s = new OptionsMenuItem("Slope");
		s->addItem(OptionMenuItem("1 pole", [m]() { return m->_polesSetting == 1; }, [m]() { m->_polesSetting = 1; }));
		s->addItem(OptionMenuItem("2 poles", [m]() { return m->_polesSetting == 2; }, [m]() { m->_polesSetting = 2; }));
		s->addItem(OptionMenuItem("3 poles", [m]() { return m->_polesSetting == 3; }, [m]() { m->_polesSetting = 3; }));
		s->addItem(OptionMenuItem("4 poles", [m]() { return m->_polesSetting == 4; }, [m]() { m->_polesSetting = 4; }));
		s->addItem(OptionMenuItem("5 poles", [m]() { return m->_polesSetting == 5; }, [m]() { m->_polesSetting = 5; }));
		s->addItem(OptionMenuItem("6 poles", [m]() { return m->_polesSetting == 6; }, [m]() { m->_polesSetting = 6; }));
		s->addItem(OptionMenuItem("7 poles", [m]() { return m->_polesSetting == 7; }, [m]() { m->_polesSetting = 7; }));
		s->addItem(OptionMenuItem("8 poles", [m]() { return m->_polesSetting == 8; }, [m]() { m->_polesSetting = 8; }));
		s->addItem(OptionMenuItem("9 poles", [m]() { return m->_polesSetting == 9; }, [m]() { m->_polesSetting = 9; }));
		s->addItem(OptionMenuItem("10 poles", [m]() { return m->_polesSetting == 10; }, [m]() { m->_polesSetting = 10; }));
		s->addItem(OptionMenuItem("11 poles", [m]() { return m->_polesSetting == 11; }, [m]() { m->_polesSetting = 11; }));
		s->addItem(OptionMenuItem("12 poles", [m]() { return m->_polesSetting == 12; }, [m]() { m->_polesSetting = 12; }));
		OptionsMenuItem::addToMenu(s, menu);

		OptionsMenuItem* bwm = new OptionsMenuItem("Bandwidth mode");
		bwm->addItem(OptionMenuItem("Pitched", [m]() { return m->_bandwidthMode == MultimodeFilter::PITCH_BANDWIDTH_MODE; }, [m]() { m->_bandwidthMode = MultimodeFilter::PITCH_BANDWIDTH_MODE; }));
		bwm->addItem(OptionMenuItem("Linear", [m]() { return m->_bandwidthMode == MultimodeFilter::LINEAR_BANDWIDTH_MODE; }, [m]() { m->_bandwidthMode = MultimodeFilter::LINEAR_BANDWIDTH_MODE; }));
		OptionsMenuItem::addToMenu(bwm, menu);
	}
};

Model* modelLVCF = createModel<LVCF, LVCFWidget>("Bogaudio-LVCF", "LVCF", "Compact multimode filter", "Filter", "Polyphonic");
