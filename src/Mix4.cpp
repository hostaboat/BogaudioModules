
#include "Mix4.hpp"

void Mix4::onSampleRateChange() {
	float sr = engineGetSampleRate();
	_channel1.setSampleRate(sr);
	_channel2.setSampleRate(sr);
	_channel3.setSampleRate(sr);
	_channel4.setSampleRate(sr);
	_slewLimiter.setParams(sr, slewTimeMS);
	_rms.setSampleRate(sr);
}

void Mix4::step() {
	bool stereo = outputs[L_OUTPUT].active && outputs[R_OUTPUT].active;
	_channel1.next(stereo);
	_channel2.next(stereo);
	_channel3.next(stereo);
	_channel4.next(stereo);

	float level = params[MIX_PARAM].value;
	if (inputs[MIX_CV_INPUT].active) {
		level *= clamp(inputs[MIX_CV_INPUT].value / 10.0f, 0.0f, 1.0f);
	}
	level *= maxDecibels - minDecibels;
	level += minDecibels;
	_amplifier.setLevel(_slewLimiter.next(level));

	float mono = 0.0f;
	mono += _channel1.out;
	mono += _channel2.out;
	mono += _channel3.out;
	mono += _channel4.out;
	mono = _amplifier.next(mono);
	_rmsLevel = _rms.next(mono) / 5.0f;

	if (stereo) {
		float left = 0.0f;
		left += _channel1.left;
		left += _channel2.left;
		left += _channel3.left;
		left += _channel4.left;
		left = _amplifier.next(left);
		outputs[L_OUTPUT].value = left;

		float right = 0.0f;
		right += _channel1.right;
		right += _channel2.right;
		right += _channel3.right;
		right += _channel4.right;
		right = _amplifier.next(right);
		outputs[R_OUTPUT].value = right;
	}
	else {
		outputs[L_OUTPUT].value = outputs[R_OUTPUT].value = mono;
	}
}

struct Mix4Widget : ModuleWidget {
	Mix4Widget(Mix4* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * 15, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/Mix4.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto level1ParamPosition = Vec(17.5, 32.5);
		auto pan1ParamPosition = Vec(18.5, 221.0);
		auto mute1ParamPosition = Vec(17.2, 252.7);
		auto level2ParamPosition = Vec(60.5, 32.5);
		auto pan2ParamPosition = Vec(61.5, 221.0);
		auto mute2ParamPosition = Vec(60.2, 252.7);
		auto level3ParamPosition = Vec(103.5, 32.5);
		auto pan3ParamPosition = Vec(104.5, 221.0);
		auto mute3ParamPosition = Vec(103.2, 252.7);
		auto level4ParamPosition = Vec(146.5, 32.5);
		auto pan4ParamPosition = Vec(147.5, 221.0);
		auto mute4ParamPosition = Vec(146.2, 252.7);
		auto mixParamPosition = Vec(189.5, 32.5);

		auto cv1InputPosition = Vec(14.5, 290.0);
		auto in1InputPosition = Vec(14.5, 325.0);
		auto cv2InputPosition = Vec(57.5, 290.0);
		auto in2InputPosition = Vec(57.5, 325.0);
		auto cv3InputPosition = Vec(100.5, 290.0);
		auto in3InputPosition = Vec(100.5, 325.0);
		auto cv4InputPosition = Vec(143.5, 290.0);
		auto in4InputPosition = Vec(143.5, 325.0);
		auto mixCvInputPosition = Vec(186.5, 253.0);

		auto lOutputPosition = Vec(186.5, 291.0);
		auto rOutputPosition = Vec(186.5, 326.0);
		// end generated by svg_widgets.rb

		{
			auto slider = ParamWidget::create<VUSlider>(
				level1ParamPosition,
				module,
				Mix4::LEVEL1_PARAM,
				0.0,
				1.0,
				abs(module->minDecibels) / (module->maxDecibels - module->minDecibels)
			);
			dynamic_cast<VUSlider*>(slider)->setVULevel(&(module->_channel1.rms));
			addParam(slider);
		}
		addParam(ParamWidget::create<Knob16>(pan1ParamPosition, module, Mix4::PAN1_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<StatefulButton18>(mute1ParamPosition, module, Mix4::MUTE1_PARAM, 0.0, 1.0, 0.0));
		{
			auto slider = ParamWidget::create<VUSlider>(
				level2ParamPosition,
				module,
				Mix4::LEVEL2_PARAM,
				0.0,
				1.0,
				abs(module->minDecibels) / (module->maxDecibels - module->minDecibels)
			);
			dynamic_cast<VUSlider*>(slider)->setVULevel(&(module->_channel2.rms));
			addParam(slider);
		}
		addParam(ParamWidget::create<Knob16>(pan2ParamPosition, module, Mix4::PAN2_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<StatefulButton18>(mute2ParamPosition, module, Mix4::MUTE2_PARAM, 0.0, 1.0, 0.0));
		{
			auto slider = ParamWidget::create<VUSlider>(
				level3ParamPosition,
				module,
				Mix4::LEVEL3_PARAM,
				0.0,
				1.0,
				abs(module->minDecibels) / (module->maxDecibels - module->minDecibels)
			);
			dynamic_cast<VUSlider*>(slider)->setVULevel(&(module->_channel3.rms));
			addParam(slider);
		}
		addParam(ParamWidget::create<Knob16>(pan3ParamPosition, module, Mix4::PAN3_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<StatefulButton18>(mute3ParamPosition, module, Mix4::MUTE3_PARAM, 0.0, 1.0, 0.0));
		{
			auto slider = ParamWidget::create<VUSlider>(
				level4ParamPosition,
				module,
				Mix4::LEVEL4_PARAM,
				0.0,
				1.0,
				abs(module->minDecibels) / (module->maxDecibels - module->minDecibels)
			);
			dynamic_cast<VUSlider*>(slider)->setVULevel(&(module->_channel4.rms));
			addParam(slider);
		}
		addParam(ParamWidget::create<Knob16>(pan4ParamPosition, module, Mix4::PAN4_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<StatefulButton18>(mute4ParamPosition, module, Mix4::MUTE4_PARAM, 0.0, 1.0, 0.0));
		{
			auto slider = ParamWidget::create<VUSlider>(
				mixParamPosition,
				module,
				Mix4::MIX_PARAM,
				0.0,
				1.0,
				abs(module->minDecibels) / (module->maxDecibels - module->minDecibels)
			);
			dynamic_cast<VUSlider*>(slider)->setVULevel(&(module->_rmsLevel));
			addParam(slider);
		}

		addInput(Port::create<Port24>(cv1InputPosition, Port::INPUT, module, Mix4::CV1_INPUT));
		addInput(Port::create<Port24>(in1InputPosition, Port::INPUT, module, Mix4::IN1_INPUT));
		addInput(Port::create<Port24>(cv2InputPosition, Port::INPUT, module, Mix4::CV2_INPUT));
		addInput(Port::create<Port24>(in2InputPosition, Port::INPUT, module, Mix4::IN2_INPUT));
		addInput(Port::create<Port24>(cv3InputPosition, Port::INPUT, module, Mix4::CV3_INPUT));
		addInput(Port::create<Port24>(in3InputPosition, Port::INPUT, module, Mix4::IN3_INPUT));
		addInput(Port::create<Port24>(cv4InputPosition, Port::INPUT, module, Mix4::CV4_INPUT));
		addInput(Port::create<Port24>(in4InputPosition, Port::INPUT, module, Mix4::IN4_INPUT));
		addInput(Port::create<Port24>(mixCvInputPosition, Port::INPUT, module, Mix4::MIX_CV_INPUT));

		addOutput(Port::create<Port24>(lOutputPosition, Port::OUTPUT, module, Mix4::L_OUTPUT));
		addOutput(Port::create<Port24>(rOutputPosition, Port::OUTPUT, module, Mix4::R_OUTPUT));
	}
};

Model* modelMix4 = Model::create<Mix4, Mix4Widget>("Bogaudio", "Bogaudio-Mix4", "Mix4", MIXER_TAG, PANNING_TAG, DUAL_TAG);