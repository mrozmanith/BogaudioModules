
#include "DGate.hpp"

void DGate::onReset() {
	_trigger.reset();
	_triggerOuptutPulseGen.process(10.0);
	_stage = STOPPED_STAGE;
	_stageProgress = 0.0;
}

void DGate::step() {
	float envelope = 0.0;
	bool complete = false;
	if (
		_trigger.process(params[TRIGGER_PARAM].value + inputs[TRIGGER_INPUT].value) ||
		(_firstStep && _triggerOnLoad && _shouldTriggerOnLoad && params[LOOP_PARAM].value <= 0.0)
	) {
		_stage = DELAY_STAGE;
		_stageProgress = 0.0;
	}
	else {
		switch (_stage) {
			case STOPPED_STAGE: {
				break;
			}
			case DELAY_STAGE: {
				if (stepStage(params[DELAY_PARAM])) {
					_stage = GATE_STAGE;
					_stageProgress = 0.0;
				}
				break;
			}
			case GATE_STAGE: {
				if (stepStage(params[GATE_PARAM])) {
					complete = true;
					if (params[LOOP_PARAM].value <= 0.0 || _trigger.isHigh()) {
						_stage = DELAY_STAGE;
						_stageProgress = 0.0;
					}
					else {
						_stage = STOPPED_STAGE;
					}
				}
				else {
					envelope = 1.0;
				}
				break;
			}
		}
	}

	outputs[GATE_OUTPUT].value = envelope * 10.0;
	if (complete) {
		_triggerOuptutPulseGen.trigger(0.001);
	}
	outputs[END_OUTPUT].value = _triggerOuptutPulseGen.process(engineGetSampleTime()) ? 5.0 : 0.0;

	lights[DELAY_LIGHT].value = _stage == DELAY_STAGE;
	lights[GATE_LIGHT].value = _stage == GATE_STAGE;

	_firstStep = false;
}

bool DGate::stepStage(Param& knob) {
	float t = knob.value;
	t = pow(t, 2);
	t = fmaxf(t, 0.001);
	t *= 10.0;
	_stageProgress += engineGetSampleTime() / t;
	return _stageProgress > 1.0;
}

struct DGateWidget : ModuleWidget {
	static constexpr int hp = 3;

	DGateWidget(DGate* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/DGate.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto delayParamPosition = Vec(8.0, 33.0);
		auto gateParamPosition = Vec(8.0, 92.0);
		auto loopParamPosition = Vec(15.0, 144.5);
		auto triggerParamPosition = Vec(13.5, 191.0);

		auto triggerInputPosition = Vec(10.5, 213.0);

		auto gateOutputPosition = Vec(10.5, 252.0);
		auto endOutputPosition = Vec(10.5, 287.0);

		auto delayLightPosition = Vec(20.8, 65.0);
		auto gateLightPosition = Vec(20.8, 124.0);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob29>(delayParamPosition, module, DGate::DELAY_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob29>(gateParamPosition, module, DGate::GATE_PARAM, 0.0, 1.0, 0.32));
		addParam(ParamWidget::create<SliderSwitch2State14>(loopParamPosition, module, DGate::LOOP_PARAM, 0.0, 1.0, 1.0));
		addParam(ParamWidget::create<Button18>(triggerParamPosition, module, DGate::TRIGGER_PARAM, 0.0, 1.0, 0.0));

		addInput(Port::create<Port24>(triggerInputPosition, Port::INPUT, module, DGate::TRIGGER_INPUT));

		addOutput(Port::create<Port24>(gateOutputPosition, Port::OUTPUT, module, DGate::GATE_OUTPUT));
		addOutput(Port::create<Port24>(endOutputPosition, Port::OUTPUT, module, DGate::END_OUTPUT));

		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(delayLightPosition, module, DGate::DELAY_LIGHT));
		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(gateLightPosition, module, DGate::GATE_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
	  DGate* dgate = dynamic_cast<DGate*>(module);
		assert(dgate);
		menu->addChild(new MenuLabel());
		menu->addChild(new TriggerOnLoadMenuItem(dgate, "Resume loop on load"));
	}
};

Model* modelDGate = createModel<DGate, DGateWidget>("Bogaudio-DGate", "DGate",  "trigger-to-gate with delay");
