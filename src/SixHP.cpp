
#include "bogaudio.hpp"

struct SixHP : Module {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	SixHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		reset();
	}

	virtual void reset() override;
	virtual void step() override;
};

void SixHP::reset() {
}

void SixHP::step() {
}


SixHPWidget::SixHPWidget() {
	SixHP *module = new SixHP();
	setModule(module);
	box.size = Vec(RACK_GRID_WIDTH * 6, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/SixHP.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(0, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 15, 365)));
}