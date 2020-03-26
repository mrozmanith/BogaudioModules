
#include "TestExpander.hpp"

int TestExpanderBase::channels() {
	return inputs[IN_INPUT].getChannels();
}

void TestExpanderBase::processAll(const ProcessArgs& args) {
	outputs[OUT_OUTPUT].setChannels(_channels);
	lights[COM_LIGHT].value = connected();
}

void TestExpanderBase::processChannel(const ProcessArgs& args, int c) {
	if (connected()) {
		toExpander()->sample[c] = inputs[IN_INPUT].getPolyVoltage(c);
		outputs[OUT_OUTPUT].setVoltage(fromExpander()->sample[c], c);
	}
	else {
		outputs[OUT_OUTPUT].setVoltage(inputs[IN_INPUT].getPolyVoltage(c), c);
	}
}

struct TestExpanderBaseWidget : ModuleWidget {
	static constexpr int hp = 3;

	TestExpanderBaseWidget(TestExpanderBase* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TestExpanderBase.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto inInputPosition = Vec(10.5, 263.0);

		auto outOutputPosition = Vec(10.5, 301.0);

		auto comLightPosition = Vec(10.0, 245.5);
		// end generated by svg_widgets.rb

		addInput(createInput<Port24>(inInputPosition, module, TestExpanderBase::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, TestExpanderBase::OUT_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(comLightPosition, module, TestExpanderBase::COM_LIGHT));
	}
};

Model* modelTestExpanderBase = createModel<TestExpanderBase, TestExpanderBaseWidget>("Bogaudio-TestExpanderBase", "TEB", "expanders test base module");


void TestExpanderExtension::processAll(const ProcessArgs& args) {
	outputs[OUT_OUTPUT].setChannels(_channels);
	lights[COM_LIGHT].value = connected();
}

void TestExpanderExtension::processChannel(const ProcessArgs& args, int c) {
	if (connected()) {
		float sample = fromBase()->sample[c];
		toBase()->sample[c] = -sample;
		outputs[OUT_OUTPUT].setVoltage(sample, c);
	}
	else {
		outputs[OUT_OUTPUT].setVoltage(0.0f, c);
	}
}

struct TestExpanderExtensionWidget : ModuleWidget {
	static constexpr int hp = 3;

	TestExpanderExtensionWidget(TestExpanderExtension* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TestExpanderExtension.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto outOutputPosition = Vec(10.5, 301.0);

		auto comLightPosition = Vec(10.0, 280.5);
		// end generated by svg_widgets.rb

		addOutput(createOutput<Port24>(outOutputPosition, module, TestExpanderExtension::OUT_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(comLightPosition, module, TestExpanderExtension::COM_LIGHT));
	}
};

Model* modelTestExpanderExtension = createModel<TestExpanderExtension, TestExpanderExtensionWidget>("Bogaudio-TestExpanderExtension", "TEE", "expanders test expander module");