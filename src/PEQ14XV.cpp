
#include "PEQ14XV.hpp"

PEQ14XV::Engine::Engine() {
	filters[0] = new MultimodeFilter8();
	for (int i = 1; i < 13; ++i) {
		filters[i] = new MultimodeFilter4();
	}
	filters[13] = new MultimodeFilter8();
}

PEQ14XV::Engine::~Engine() {
	for (int i = 0; i < 14; ++i) {
		delete filters[i];
	}
}

void PEQ14XV::sampleRateChange() {
	_sampleRate = APP->engine->getSampleRate();
}

void PEQ14XV::addChannel(int c) {
	_engines[c] = new Engine();
}

void PEQ14XV::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void PEQ14XV::processAlways(const ProcessArgs& args) {
	outputs[OUT_OUTPUT].setChannels(_channels);

	_baseMessage = NULL;
	if (baseConnected()) {
		_baseMessage = fromBase();
	}

	if (expanderConnected()) {
		if (_baseMessage) {
			// *toExpander() = *_baseMessage;
			_baseMessage->copyTo(toExpander());
		}
		else {
			toExpander()->reset();
		}
	}
}

void PEQ14XV::processChannel(const ProcessArgs& args, int c) {
	if (_baseMessage && _baseMessage->valid) {
		Engine& e = *_engines[c];
		float in = inputs[IN_INPUT].getPolyVoltage(c);
		float out = 0.0f;
		for (int i = 0; i < 14; ++i) {
			auto mode = MultimodeFilter::BANDPASS_MODE;
			int poles = 4;
			float bandwidth = _baseMessage->bandwidths[c];
			if (i == 0 && _baseMessage->lowLP) {
				mode = MultimodeFilter::LOWPASS_MODE;
				poles = 12;
				bandwidth = MultimodeFilter::minQbw;
			}
			if (i == 13 && _baseMessage->highHP) {
				mode = MultimodeFilter::HIGHPASS_MODE;
				poles = 12;
				bandwidth = MultimodeFilter::minQbw;
			}
			e.filters[i]->setParams(
				_sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				poles,
				mode,
				_baseMessage->frequencies[c][i],
				bandwidth,
				MultimodeFilter::PITCH_BANDWIDTH_MODE
			);

			float db = _baseMessage->levels[c][i];
			db *= 0.2f;
			db = std::max(0.0f, std::min(1.0f, db));
			db = 1.0f - db;
			db *= Amplifier::minDecibels;
			e.amplifiers[i].setLevel(db);

			out += e.amplifiers[i].next(e.filters[i]->next(in));
		}
		outputs[OUT_OUTPUT].setVoltage(_saturator.next(out), c);
	}
	else {
		outputs[OUT_OUTPUT].setVoltage(0.0f, c);
	}
}

struct PEQ14XVWidget : ModuleWidget {
	static constexpr int hp = 5;

	PEQ14XVWidget(PEQ14XV* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PEQ14XV.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto inInputPosition = Vec(9.0, 322.0);

		auto outOutputPosition = Vec(41.0, 322.0);
		// end generated by svg_widgets.rb

		addInput(createInput<Port24>(inInputPosition, module, PEQ14XV::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, PEQ14XV::OUT_OUTPUT));
	}
};

Model* modelPEQ14XV = createModel<PEQ14XV, PEQ14XVWidget>("Bogaudio-PEQ14XV", "PEQ14XV", "PEQ14 vocoder expander", "Filter", "Vocoder", "Expander", "Polyphonic");
