
#include "VCA.hpp"

void VCA::onSampleRateChange() {
	float sampleRate = engineGetSampleRate();
	_levelSL1.setParams(sampleRate, 100.0f);
	_levelSL2.setParams(sampleRate, 100.0f);
}

void VCA::step() {
	bool linear = params[LINEAR_PARAM].value > 0.5f;
	lights[LINEAR_LIGHT].value = linear;
	channelStep(inputs[IN1_INPUT], outputs[OUT1_OUTPUT], params[LEVEL1_PARAM], inputs[CV1_INPUT], _amplifier1, _levelSL1, linear);
	channelStep(inputs[IN2_INPUT], outputs[OUT2_OUTPUT], params[LEVEL2_PARAM], inputs[CV2_INPUT], _amplifier2, _levelSL1, linear);
}

void VCA::channelStep(Input& input, Output& output, Param& knob, Input& cv, Amplifier& amplifier, SlewLimiter& levelSL, bool linear) {
	if (input.active && output.active) {
		float level = knob.value;
		if (cv.active) {
			level *= clamp(cv.value / 10.0f, 0.0f, 1.0f);
		}
		level = levelSL.next(level);
		if (linear) {
			output.value = level * input.value;
		}
		else {
			level = 1.0f - level;
			level *= Amplifier::minDecibels;
			amplifier.setLevel(level);
			output.value = amplifier.next(input.value);
		}
	}
}

struct VCAWidget : ModuleWidget {
	VCAWidget(VCA* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/VCA.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto level1ParamPosition = Vec(9.5, 22.5);
		auto level2ParamPosition = Vec(9.5, 181.5);
		auto linearParamPosition = Vec(29.4, 332.9);

		auto cv1InputPosition = Vec(10.5, 60.0);
		auto in1InputPosition = Vec(10.5, 95.0);
		auto cv2InputPosition = Vec(10.5, 219.0);
		auto in2InputPosition = Vec(10.5, 254.0);

		auto out1OutputPosition = Vec(10.5, 133.0);
		auto out2OutputPosition = Vec(10.5, 292.0);

		auto linearLightPosition = Vec(6.5, 334.5);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob26>(level1ParamPosition, module, VCA::LEVEL1_PARAM, 0.0, 1.0, 0.8));
		addParam(ParamWidget::create<Knob26>(level2ParamPosition, module, VCA::LEVEL2_PARAM, 0.0, 1.0, 0.8));
		addParam(ParamWidget::create<StatefulButton9>(linearParamPosition, module, VCA::LINEAR_PARAM, 0.0, 1.0, 0.0));

		addInput(Port::create<Port24>(cv1InputPosition, Port::INPUT, module, VCA::CV1_INPUT));
		addInput(Port::create<Port24>(in1InputPosition, Port::INPUT, module, VCA::IN1_INPUT));
		addInput(Port::create<Port24>(cv2InputPosition, Port::INPUT, module, VCA::CV2_INPUT));
		addInput(Port::create<Port24>(in2InputPosition, Port::INPUT, module, VCA::IN2_INPUT));

		addOutput(Port::create<Port24>(out1OutputPosition, Port::OUTPUT, module, VCA::OUT1_OUTPUT));
		addOutput(Port::create<Port24>(out2OutputPosition, Port::OUTPUT, module, VCA::OUT2_OUTPUT));

		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(linearLightPosition, module, VCA::LINEAR_LIGHT));
	}
};

Model* modelVCA = Model::create<VCA, VCAWidget>("Bogaudio", "Bogaudio-VCA", "VCA", AMPLIFIER_TAG, ATTENUATOR_TAG, DUAL_TAG, UTILITY_TAG);
