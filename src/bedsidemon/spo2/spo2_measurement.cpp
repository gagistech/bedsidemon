#include "spo2_measurement.hpp"

using namespace std::string_view_literals;

using namespace bedsidemon;

tml::forest spo2_measurement::to_tml() const
{
	tml::forest f;

	f.push_back(tml::tree("strength"sv, {tml::leaf(this->signal_strength)}));
	f.push_back(tml::tree("beat"sv, {tml::leaf(this->pulse_beat)}));
	f.push_back(tml::tree("finger_out"sv, {tml::leaf(this->finger_out)}));
	f.push_back(tml::tree("wf_value"sv, {tml::leaf(this->waveform_point)}));
	f.push_back(tml::tree("pulse"sv, {tml::leaf(this->pulse_rate)}));
	f.push_back(tml::tree("spo2"sv, {tml::leaf(this->spo2)}));
	f.push_back(tml::tree("perf"sv, {tml::leaf(this->perfusion_index)}));
	f.push_back(tml::tree("dt_ms"sv, {tml::leaf(this->delta_time_ms)}));

	return f;
}

namespace {
const tml::leaf& get_prop_val(const tml::tree& t)
{
	if (t.children.empty()) {
		throw std::invalid_argument(utki::cat("no tml property value for property: ", t));
	}

	return t.children.front().value;
}
} // namespace

spo2_measurement spo2_measurement::from_tml(const tml::forest& f)
{
	spo2_measurement meas{};

	unsigned checksum = 0;

	for (const auto& t : f) {
		if (t.value == "strength"sv) {
			meas.signal_strength = uint8_t(get_prop_val(t).to_uint32());
		} else if (t.value == "beat"sv) {
			meas.pulse_beat = get_prop_val(t).to_bool();
		} else if (t.value == "finger_out"sv) {
			meas.finger_out = get_prop_val(t).to_bool();
		} else if (t.value == "wf_value"sv) {
			meas.waveform_point = get_prop_val(t).to_float();
		} else if (t.value == "pulse"sv) {
			meas.pulse_rate = uint8_t(get_prop_val(t).to_uint32());
		} else if (t.value == "spo2"sv) {
			meas.spo2 = uint8_t(get_prop_val(t).to_uint32());
		} else if (t.value == "perf"sv) {
			meas.perfusion_index = uint16_t(get_prop_val(t).to_uint32());
		} else if (t.value == "dt_ms"sv) {
			meas.delta_time_ms = uint16_t(get_prop_val(t).to_uint32());
		} else {
			throw std::invalid_argument(utki::cat("unknown spo2 frame property: ", t));
		}
		++checksum;
	}

	constexpr auto expected_number_of_fields = 8;

	if (checksum != expected_number_of_fields) {
		throw std::invalid_argument(utki::cat("malformed spo2 frame: ", f));
	}

	return meas;
}
