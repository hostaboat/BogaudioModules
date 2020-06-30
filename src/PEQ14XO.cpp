
#include "PEQ14XO.hpp"

void PEQ14XO::processAll(const ProcessArgs& args) {
	for (int i = 0; i < 14; ++i) {
		outputs[BAND1_OUTPUT + i].setChannels(_channels);
	}

	if (expanderConnected()) {
		if (baseConnected()) {
			// *toExpander() = *fromBase();
			fromBase()->copyTo(toExpander());
		}
		else {
			toExpander()->reset();
		}
	}
}

void PEQ14XO::processChannel(const ProcessArgs& args, int c) {
	if (baseConnected()) {
		float* outs = fromBase()->outs[c];
		for (int i = 0; i < 14; ++i) {
			outputs[BAND1_OUTPUT + i].setVoltage(outs[i], c);
		}
	}
	else {
		for (int i = 0; i < 14; ++i) {
			outputs[BAND1_OUTPUT + i].setVoltage(0.0f, c);
		}
	}
}

struct PEQ14XOWidget : ModuleWidget {
	static constexpr int hp = 5;

	PEQ14XOWidget(PEQ14XO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PEQ14XO.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto band1OutputPosition = Vec(10.5, 43.0);
		auto band2OutputPosition = Vec(10.5, 78.0);
		auto band3OutputPosition = Vec(10.5, 113.0);
		auto band4OutputPosition = Vec(10.5, 148.0);
		auto band5OutputPosition = Vec(10.5, 183.0);
		auto band6OutputPosition = Vec(10.5, 218.0);
		auto band7OutputPosition = Vec(10.5, 253.0);
		auto band8OutputPosition = Vec(41.5, 43.0);
		auto band9OutputPosition = Vec(41.5, 78.0);
		auto band10OutputPosition = Vec(41.5, 113.0);
		auto band11OutputPosition = Vec(41.5, 148.0);
		auto band12OutputPosition = Vec(41.5, 183.0);
		auto band13OutputPosition = Vec(41.5, 218.0);
		auto band14OutputPosition = Vec(41.5, 253.0);
		// end generated by svg_widgets.rb

		addOutput(createOutput<Port24>(band1OutputPosition, module, PEQ14XO::BAND1_OUTPUT));
		addOutput(createOutput<Port24>(band2OutputPosition, module, PEQ14XO::BAND2_OUTPUT));
		addOutput(createOutput<Port24>(band3OutputPosition, module, PEQ14XO::BAND3_OUTPUT));
		addOutput(createOutput<Port24>(band4OutputPosition, module, PEQ14XO::BAND4_OUTPUT));
		addOutput(createOutput<Port24>(band5OutputPosition, module, PEQ14XO::BAND5_OUTPUT));
		addOutput(createOutput<Port24>(band6OutputPosition, module, PEQ14XO::BAND6_OUTPUT));
		addOutput(createOutput<Port24>(band7OutputPosition, module, PEQ14XO::BAND7_OUTPUT));
		addOutput(createOutput<Port24>(band8OutputPosition, module, PEQ14XO::BAND8_OUTPUT));
		addOutput(createOutput<Port24>(band9OutputPosition, module, PEQ14XO::BAND9_OUTPUT));
		addOutput(createOutput<Port24>(band10OutputPosition, module, PEQ14XO::BAND10_OUTPUT));
		addOutput(createOutput<Port24>(band11OutputPosition, module, PEQ14XO::BAND11_OUTPUT));
		addOutput(createOutput<Port24>(band12OutputPosition, module, PEQ14XO::BAND12_OUTPUT));
		addOutput(createOutput<Port24>(band13OutputPosition, module, PEQ14XO::BAND13_OUTPUT));
		addOutput(createOutput<Port24>(band14OutputPosition, module, PEQ14XO::BAND14_OUTPUT));
	}
};

Model* modelPEQ14XO = createModel<PEQ14XO, PEQ14XOWidget>("Bogaudio-PEQ14XO", "PEQ14XO", "PEQ14 band outputs expander", "Filter", "Expander", "Polyphonic");
