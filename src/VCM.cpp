
#include "VCM.hpp"

bool VCM::LevelParamQuantity::isLinear() {
	return dynamic_cast<VCM*>(module)->isLinear();
}

bool VCM::active() {
	return outputs[MIX_OUTPUT].isConnected();
}

int VCM::channels() {
	return inputs[IN1_INPUT].getChannels();
}

void VCM::processChannel(const ProcessArgs& args, int c) {
	bool linear = isLinear();
	float out = channelStep(c, inputs[IN1_INPUT], params[LEVEL1_PARAM], inputs[CV1_INPUT], _amplifier1[c], linear);
	out += channelStep(c, inputs[IN2_INPUT], params[LEVEL2_PARAM], inputs[CV2_INPUT], _amplifier2[c], linear);
	out += channelStep(c, inputs[IN3_INPUT], params[LEVEL3_PARAM], inputs[CV3_INPUT], _amplifier3[c], linear);
	out += channelStep(c, inputs[IN4_INPUT], params[LEVEL4_PARAM], inputs[CV4_INPUT], _amplifier4[c], linear);
	float level = params[MIX_PARAM].getValue();
	if (inputs[MIX_CV_INPUT].isConnected()) {
		level *= clamp(inputs[MIX_CV_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	out *= level;
	if (!_disableOutputLimit) {
		out = clamp(out, -12.0f, 12.0f);
	}
	outputs[MIX_OUTPUT].setChannels(_channels);
	outputs[MIX_OUTPUT].setVoltage(level * out, c);
}

float VCM::channelStep(int c, Input& input, Param& knob, Input& cv, Amplifier& amplifier, bool linear) {
	if (!input.isConnected()) {
		return 0.0f;
	}
	float level = knob.getValue();
	if (cv.isConnected()) {
		level *= clamp(cv.getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	if (linear) {
		return level * input.getPolyVoltage(c);
	}
	level = 1.0f - level;
	level *= Amplifier::minDecibels;
	amplifier.setLevel(level);
	return amplifier.next(input.getPolyVoltage(c));
}

struct VCMWidget : DisableOutputLimitModuleWidget {
	static constexpr int hp = 10;

	VCMWidget(VCM* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCM.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto level1ParamPosition = Vec(89.5, 35.5);
		auto level2ParamPosition = Vec(89.5, 99.5);
		auto level3ParamPosition = Vec(89.5, 163.5);
		auto level4ParamPosition = Vec(89.5, 228.5);
		auto mixParamPosition = Vec(22.5, 293.5);
		auto linearParamPosition = Vec(91.0, 342.7);

		auto in1InputPosition = Vec(14.0, 37.0);
		auto cv1InputPosition = Vec(45.0, 37.0);
		auto in2InputPosition = Vec(14.0, 101.0);
		auto cv2InputPosition = Vec(45.0, 101.0);
		auto in3InputPosition = Vec(14.0, 165.0);
		auto cv3InputPosition = Vec(45.0, 165.0);
		auto in4InputPosition = Vec(14.0, 230.0);
		auto cv4InputPosition = Vec(45.0, 230.0);
		auto mixCvInputPosition = Vec(81.0, 294.0);

		auto mixOutputPosition = Vec(112.0, 294.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(level1ParamPosition, module, VCM::LEVEL1_PARAM));
		addParam(createParam<Knob38>(level2ParamPosition, module, VCM::LEVEL2_PARAM));
		addParam(createParam<Knob38>(level3ParamPosition, module, VCM::LEVEL3_PARAM));
		addParam(createParam<Knob38>(level4ParamPosition, module, VCM::LEVEL4_PARAM));
		addParam(createParam<Knob38>(mixParamPosition, module, VCM::MIX_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(linearParamPosition, module, VCM::LINEAR_PARAM));

		addInput(createInput<Port24>(in1InputPosition, module, VCM::IN1_INPUT));
		addInput(createInput<Port24>(cv1InputPosition, module, VCM::CV1_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, VCM::IN2_INPUT));
		addInput(createInput<Port24>(cv2InputPosition, module, VCM::CV2_INPUT));
		addInput(createInput<Port24>(in3InputPosition, module, VCM::IN3_INPUT));
		addInput(createInput<Port24>(cv3InputPosition, module, VCM::CV3_INPUT));
		addInput(createInput<Port24>(in4InputPosition, module, VCM::IN4_INPUT));
		addInput(createInput<Port24>(cv4InputPosition, module, VCM::CV4_INPUT));
		addInput(createInput<Port24>(mixCvInputPosition, module, VCM::MIX_CV_INPUT));

		addOutput(createOutput<Port24>(mixOutputPosition, module, VCM::MIX_OUTPUT));
	}
};

Model* modelVCM = bogaudio::createModel<VCM, VCMWidget>("Bogaudio-VCM", "VCM", "4-channel CV controllable mixer", "Mixer", "Polyphonic");
