
#include "VCO.hpp"
#include "dsp/pitch.hpp"

void VCO::onReset() {
	_syncTrigger.reset();
	_modulationStep = modulationSteps;
}

void VCO::onSampleRateChange() {
	_phasor.setSampleRate(engineGetSampleRate());
	_modulationStep = modulationSteps;
}

void VCO::step() {
	lights[SLOW_LIGHT].value = _slowMode;
	lights[FM_EXP_LIGHT].value = !_fmLinearMode;
	lights[FM_LIN_LIGHT].value = _fmLinearMode;

	if (!(
		outputs[SQUARE_OUTPUT].active ||
		outputs[SAW_OUTPUT].active ||
		outputs[TRIANGLE_OUTPUT].active ||
		outputs[SINE_OUTPUT].active
	)) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		_baseVOct = params[FREQUENCY_PARAM].value;
		_baseVOct += params[FINE_PARAM].value;
		if (inputs[PITCH_INPUT].active) {
			_baseVOct += clamp(inputs[PITCH_INPUT].value, -5.0f, 5.0f);
		}
		if ((_slowMode = ((int)params[SLOW_PARAM].value)) == 1) {
			_baseVOct -= 5.0f;
		}
		_baseHz = cvToFrequency(_baseVOct);

		if (_syncTrigger.process(inputs[SYNC_INPUT].value)) {
			_phasor.setPhase(0.0f);
		}

		float pw = params[PW_PARAM].value;
		if (inputs[PW_INPUT].active) {
			pw *= clamp(inputs[PW_INPUT].value / 5.0f, -1.0f, 1.0f);
		}
		pw = (pw + 1.0f) / 2.0f;
		pw *= 1.0f - 2.0f * _square.minPulseWidth;
		_square.setPulseWidth(pw);

		_fmDepth = params[FM_PARAM].value;
		_fmLinearMode = ((int)params[FM_TYPE_PARAM].value) == 1;
	}

	if (inputs[FM_INPUT].active) {
		float fm = inputs[FM_INPUT].value * _fmDepth;
		if (_fmLinearMode) {
			_phasor.setFrequency(_baseHz + fm * _baseHz);
		}
		else {
			_phasor.setFrequency(cvToFrequency(_baseVOct + fm));
		}
	}
	else {
		_phasor.setFrequency(_baseHz);
	}
	_phasor.advancePhase();
	if (outputs[SQUARE_OUTPUT].active) {
		outputs[SQUARE_OUTPUT].value = amplitude * _square.nextFromPhasor(_phasor);
	}
	if (outputs[SAW_OUTPUT].active) {
		outputs[SAW_OUTPUT].value = amplitude * _saw.nextFromPhasor(_phasor);
	}
	if (outputs[TRIANGLE_OUTPUT].active) {
		outputs[TRIANGLE_OUTPUT].value = amplitude * _triangle.nextFromPhasor(_phasor);
	}
	if (outputs[SINE_OUTPUT].active) {
		outputs[SINE_OUTPUT].value = amplitude * _sine.nextFromPhasor(_phasor);
	}
}

struct VCOWidget : ModuleWidget {
	VCOWidget(VCO* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * 10, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/VCO.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(0, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(40.0, 45.0);
		auto fineParamPosition = Vec(33.0, 151.0);
		auto slowParamPosition = Vec(130.0, 155.2);
		auto pwParamPosition = Vec(28.5, 186.5);
		auto fmParamPosition = Vec(28.5, 231.5);
		auto fmTypeParamPosition = Vec(92.0, 239.5);

		auto pitchInputPosition = Vec(14.0, 318.0);
		auto syncInputPosition = Vec(52.0, 318.0);
		auto pwInputPosition = Vec(14.0, 274.0);
		auto fmInputPosition = Vec(52.0, 274.0);

		auto squareOutputPosition = Vec(113.0, 186.0);
		auto sawOutputPosition = Vec(113.0, 230.0);
		auto triangleOutputPosition = Vec(113.0, 274.0);
		auto sineOutputPosition = Vec(113.0, 318.0);

		auto slowLightPosition = Vec(90.0, 156.5);
		auto fmExpLightPosition = Vec(62.0, 234.0);
		auto fmLinLightPosition = Vec(62.0, 247.3);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob68>(frequencyParamPosition, module, VCO::FREQUENCY_PARAM, -3.0, 6.0, 0.0));
		addParam(ParamWidget::create<Knob16>(fineParamPosition, module, VCO::FINE_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<StatefulButton9>(slowParamPosition, module, VCO::SLOW_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(pwParamPosition, module, VCO::PW_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(fmParamPosition, module, VCO::FM_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<StatefulButton9>(fmTypeParamPosition, module, VCO::FM_TYPE_PARAM, 0.0, 1.0, 0.0));

		addInput(Port::create<Port24>(pitchInputPosition, Port::INPUT, module, VCO::PITCH_INPUT));
		addInput(Port::create<Port24>(syncInputPosition, Port::INPUT, module, VCO::SYNC_INPUT));
		addInput(Port::create<Port24>(pwInputPosition, Port::INPUT, module, VCO::PW_INPUT));
		addInput(Port::create<Port24>(fmInputPosition, Port::INPUT, module, VCO::FM_INPUT));

		addOutput(Port::create<Port24>(squareOutputPosition, Port::OUTPUT, module, VCO::SQUARE_OUTPUT));
		addOutput(Port::create<Port24>(sawOutputPosition, Port::OUTPUT, module, VCO::SAW_OUTPUT));
		addOutput(Port::create<Port24>(triangleOutputPosition, Port::OUTPUT, module, VCO::TRIANGLE_OUTPUT));
		addOutput(Port::create<Port24>(sineOutputPosition, Port::OUTPUT, module, VCO::SINE_OUTPUT));

		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(slowLightPosition, module, VCO::SLOW_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(fmExpLightPosition, module, VCO::FM_EXP_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(fmLinLightPosition, module, VCO::FM_LIN_LIGHT));
	}
};

Model* modelVCO = Model::create<VCO, VCOWidget>("Bogaudio", "Bogaudio-VCO", "VCO", OSCILLATOR_TAG);
