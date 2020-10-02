
#include "AddrSeq.hpp"

void AddrSeq::processAlways(const ProcessArgs& args) {
	if (expanderConnected()) {
		AddrSeqExpanderMessage* te = toExpander();
		te->baseID = _id;
		te->position = 1;
		te->rangeOffset = _rangeOffset;
		te->rangeScale = _rangeScale;
	}
}

void AddrSeq::processChannel(const ProcessArgs& args, int c) {
	std::lock_guard<SpinLock> lock(_elementsLock);
	std::vector<AddrSeqStep*>& steps = _elements;
	int stepsN = steps.size();

	if (c == 0) {
		for (int i = 0; i < stepsN; ++i) {
			steps[i]->lightSum = 0.0f;
		}
	}

	int step = nextStep(
		c,
		&inputs[RESET_INPUT],
		inputs[CLOCK_INPUT],
		&params[STEPS_PARAM],
		params[DIRECTION_PARAM],
		&params[SELECT_PARAM],
		inputs[SELECT_INPUT],
		stepsN
	);

	float out = steps[step]->param.getValue();
	out += _rangeOffset;
	out *= _rangeScale;
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(out, c);

	steps[step]->lightSum += 1;

	if (c == _channels - 1) {
		for (int i = 0; i < stepsN; ++i) {
			steps[i]->light.value = steps[i]->lightSum * _inverseChannels;
		}
	}
}

struct AddrSeqWidget : AddressableSequenceModuleWidget {
	static constexpr int hp = 6;

	AddrSeqWidget(AddrSeq* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "AddrSeq");
		createScrews();

		// generated by svg_widgets.rb
		auto stepsParamPosition = Vec(15.5, 131.5);
		auto directionParamPosition = Vec(16.0, 167.5);
		auto selectParamPosition = Vec(9.0, 230.0);
		auto out1ParamPosition = Vec(58.5, 36.0);
		auto out2ParamPosition = Vec(58.5, 77.3);
		auto out3ParamPosition = Vec(58.5, 118.6);
		auto out4ParamPosition = Vec(58.5, 159.9);
		auto out5ParamPosition = Vec(58.5, 201.1);
		auto out6ParamPosition = Vec(58.5, 242.4);
		auto out7ParamPosition = Vec(58.5, 283.7);
		auto out8ParamPosition = Vec(58.5, 325.0);

		auto clockInputPosition = Vec(11.5, 35.0);
		auto resetInputPosition = Vec(11.5, 72.0);
		auto selectInputPosition = Vec(11.5, 270.0);

		auto outOutputPosition = Vec(11.5, 324.0);

		auto out1LightPosition = Vec(66.5, 58.5);
		auto out2LightPosition = Vec(66.5, 99.8);
		auto out3LightPosition = Vec(66.5, 141.1);
		auto out4LightPosition = Vec(66.5, 182.4);
		auto out5LightPosition = Vec(66.5, 223.6);
		auto out6LightPosition = Vec(66.5, 264.9);
		auto out7LightPosition = Vec(66.5, 306.2);
		auto out8LightPosition = Vec(66.5, 347.5);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(stepsParamPosition, module, AddrSeq::STEPS_PARAM));
		addParam(createParam<SliderSwitch2State14>(directionParamPosition, module, AddrSeq::DIRECTION_PARAM));
		addParam(createParam<Knob29>(selectParamPosition, module, AddrSeq::SELECT_PARAM));
		addParam(createParam<Knob16>(out1ParamPosition, module, AddrSeq::OUT1_PARAM));
		addParam(createParam<Knob16>(out2ParamPosition, module, AddrSeq::OUT2_PARAM));
		addParam(createParam<Knob16>(out3ParamPosition, module, AddrSeq::OUT3_PARAM));
		addParam(createParam<Knob16>(out4ParamPosition, module, AddrSeq::OUT4_PARAM));
		addParam(createParam<Knob16>(out5ParamPosition, module, AddrSeq::OUT5_PARAM));
		addParam(createParam<Knob16>(out6ParamPosition, module, AddrSeq::OUT6_PARAM));
		addParam(createParam<Knob16>(out7ParamPosition, module, AddrSeq::OUT7_PARAM));
		addParam(createParam<Knob16>(out8ParamPosition, module, AddrSeq::OUT8_PARAM));

		addInput(createInput<Port24>(clockInputPosition, module, AddrSeq::CLOCK_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, AddrSeq::RESET_INPUT));
		addInput(createInput<Port24>(selectInputPosition, module, AddrSeq::SELECT_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, AddrSeq::OUT_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(out1LightPosition, module, AddrSeq::OUT1_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out2LightPosition, module, AddrSeq::OUT2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out3LightPosition, module, AddrSeq::OUT3_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out4LightPosition, module, AddrSeq::OUT4_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out5LightPosition, module, AddrSeq::OUT5_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out6LightPosition, module, AddrSeq::OUT6_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out7LightPosition, module, AddrSeq::OUT7_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out8LightPosition, module, AddrSeq::OUT8_LIGHT));
	}

	void contextMenu(Menu* menu) override {
		AddressableSequenceModuleWidget::contextMenu(menu);
		OutputRangeOptionMenuItem::addOutputRangeOptionsToMenu(module, menu);
	}
};

Model* modelAddrSeq = bogaudio::createModel<AddrSeq, AddrSeqWidget>("Bogaudio-AddrSeq", "ADDR-SEQ", "Voltage-addressable 8-step sequencer", "Sequencer", "Polyphonic");
