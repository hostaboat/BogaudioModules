
#include "XFade.hpp"

void XFade::onSampleRateChange() {
	_mixSL.setParams(engineGetSampleRate(), 10.0f, 2.0f);
}

void XFade::step() {
	bool linear = params[LINEAR_PARAM].value > 0.5f;
	lights[LINEAR_LIGHT].value = linear;
	if (!outputs[OUT_OUTPUT].active) {
		return;
	}

	float mix = params[MIX_PARAM].value;
	if (inputs[MIX_INPUT].active) {
		mix *= clamp(inputs[MIX_INPUT].value / 5.0f, -1.0f, 1.0f);
	}
	mix = _mixSL.next(mix);

	float curveIn = params[CURVE_PARAM].value;

	if (_linear != linear || _mix != mix || _curveIn != curveIn) {
		_linear = linear;
		_mix = mix;
		_curveIn = curveIn;
		if (!linear) {
			curveIn = powf(params[CURVE_PARAM].value, 0.082f);
		}
		curveIn *= 2.0f;
		curveIn -= 1.0f;

		_mixer.setParams(mix, curveIn, linear);
	}

	outputs[OUT_OUTPUT].value = _mixer.next(inputs[A_INPUT].value, inputs[B_INPUT].value);
}

struct XFadeWidget : ModuleWidget {
	static constexpr int hp = 3;

	XFadeWidget(XFade* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(pluginInstance, "res/XFade.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto mixParamPosition = Vec(8.0, 36.0);
		auto curveParamPosition = Vec(14.5, 148.5);
		auto linearParamPosition = Vec(29.4, 175.9);

		auto mixInputPosition = Vec(10.5, 77.0);
		auto aInputPosition = Vec(10.5, 213.0);
		auto bInputPosition = Vec(10.5, 248.0);

		auto outOutputPosition = Vec(10.5, 286.0);

		auto linearLightPosition = Vec(6.5, 177.5);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob29>(mixParamPosition, module, XFade::MIX_PARAM, -1.0, 1.0, 0.0));
		{
			auto w = ParamWidget::create<Knob16>(curveParamPosition, module, XFade::CURVE_PARAM, 0.0, 1.0, 0.5);
			auto k = dynamic_cast<SVGKnob*>(w);
			k->minAngle = -0.5 * M_PI;
			k->maxAngle = 0.5 * M_PI;
			addParam(w);
		}
		addParam(ParamWidget::create<StatefulButton9>(linearParamPosition, module, XFade::LINEAR_PARAM, 0.0, 1.0, 0.0));

		addInput(Port::create<Port24>(mixInputPosition, Port::INPUT, module, XFade::MIX_INPUT));
		addInput(Port::create<Port24>(aInputPosition, Port::INPUT, module, XFade::A_INPUT));
		addInput(Port::create<Port24>(bInputPosition, Port::INPUT, module, XFade::B_INPUT));

		addOutput(Port::create<Port24>(outOutputPosition, Port::OUTPUT, module, XFade::OUT_OUTPUT));

		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(linearLightPosition, module, XFade::LINEAR_LIGHT));
	}
};

Model* modelXFade = createModel<XFade, XFadeWidget>("Bogaudio-XFade", "XFade",  "crossfader", MIXER_TAG);
