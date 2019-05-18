
#include "AMRM.hpp"

void AMRM::step() {
	if (!(outputs[OUT_OUTPUT].active || outputs[RECTIFY_OUTPUT].active)) {
		return;
	}

	float rectify = params[RECTIFY_PARAM].value;
	if (inputs[RECTIFY_INPUT].active) {
		rectify = clamp(rectify + inputs[RECTIFY_INPUT].value / 10.0f, 0.0f, 1.0f);
	}
	rectify = 1.0f - rectify;

	float depth = params[DRYWET_PARAM].value;
	if (inputs[DRYWET_INPUT].active) {
		depth = clamp(depth + inputs[DRYWET_INPUT].value / 10.0f, 0.0f, 1.0f);
	}

	float modulator = inputs[MODULATOR_INPUT].value;
	if (rectify < 1.0f) {
		rectify *= -5.0f;
		if (modulator < rectify) {
			modulator = rectify - (modulator - rectify);
		}
	}
	outputs[RECTIFY_OUTPUT].value = modulator;

	modulator *= depth;
	modulator += (1.0f - depth) * 5.0f;
	outputs[OUT_OUTPUT].value = _saturator.next(modulator * inputs[CARRIER_INPUT].value * 0.2f);
}

struct AMRMWidget : ModuleWidget {
	static constexpr int hp = 6;

	AMRMWidget(AMRM* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/AMRM.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto rectifyParamPosition = Vec(22.5, 66.5);
		auto drywetParamPosition = Vec(22.5, 165.5);

		auto modulatorInputPosition = Vec(16.0, 244.0);
		auto carrierInputPosition = Vec(50.0, 244.0);
		auto rectifyInputPosition = Vec(16.0, 280.0);
		auto drywetInputPosition = Vec(50.0, 280.0);

		auto rectifyOutputPosition = Vec(16.0, 320.0);
		auto outOutputPosition = Vec(50.0, 320.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob45>(rectifyParamPosition, module, AMRM::RECTIFY_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Knob45>(drywetParamPosition, module, AMRM::DRYWET_PARAM, 0.0, 1.0, 1.0));

		addInput(createPort<Port24>(modulatorInputPosition, PortWidget::INPUT, module, AMRM::MODULATOR_INPUT));
		addInput(createPort<Port24>(carrierInputPosition, PortWidget::INPUT, module, AMRM::CARRIER_INPUT));
		addInput(createPort<Port24>(rectifyInputPosition, PortWidget::INPUT, module, AMRM::RECTIFY_INPUT));
		addInput(createPort<Port24>(drywetInputPosition, PortWidget::INPUT, module, AMRM::DRYWET_INPUT));

		addOutput(createPort<Port24>(rectifyOutputPosition, PortWidget::OUTPUT, module, AMRM::RECTIFY_OUTPUT));
		addOutput(createPort<Port24>(outOutputPosition, PortWidget::OUTPUT, module, AMRM::OUT_OUTPUT));
	}
};

Model* modelAMRM = bogaudio::createModel<AMRM, AMRMWidget>("Bogaudio-AMRM", "AM/RM", "ring modulator", RING_MODULATOR_TAG, EFFECT_TAG);
