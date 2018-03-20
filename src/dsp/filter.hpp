#pragma once

#include <math.h>

#include "buffer.hpp"

namespace bogaudio {
namespace dsp {

struct Filter {
	Filter() {}
	virtual ~Filter() {}

	virtual float next(float sample) = 0;
};

template<typename T>
struct BiquadFilter : Filter {
	T _a0 = 0.0;
	T _a1 = 0.0;
	T _a2 = 0.0;
	T _b1 = 0.0;
	T _b2 = 0.0 ;

	T _x[3] {};
	T _y[3] {};

	BiquadFilter() {}

	void setParams(T a0, T a1, T a2, T b0, T b1, T b2) {
		if (b0 == 1.0) {
			_a0 = a0;
			_a1 = a1;
			_a2 = a2;
			_b1 = b1;
			_b2 = b2;
		}
		else {
			_a0 = a0 / b0;
			_a1 = a1 / b0;
			_a2 = a2 / b0;
			_b1 = b1 / b0;
			_b2 = b2 / b0;
		}
	}

	virtual float next(float sample) override {
		_x[2] = _x[1];
		_x[1] = _x[0];
		_x[0] = sample;

		_y[2] = _y[1];
		_y[1] = _y[0];
		_y[0] = _a0 * _x[0];
		_y[0] += _a1 * _x[1];
		_y[0] += _a2 * _x[2];
		_y[0] -= _b1 * _y[1];
		_y[0] -= _b2 * _y[2];

		return _y[0];
	}
};

struct ComplexBiquadFilter : BiquadFilter<float> {
	float _gain = 1.0f;
	float _zeroRadius = 1.0f;
	float _zeroTheta = M_PI;
	float _poleRadius = 0.9f;
	float _poleTheta = 0.0f;

	ComplexBiquadFilter() {
		updateParams();
	}

	void setComplexParams(
		float gain,
		float zeroRadius,
		float zeroTheta,
		float poleRadius,
		float poleTheta
	);
	void updateParams();
};

struct LowPassFilter : Filter {
	float _sampleRate;
	float _cutoff;
	float _q;

	BiquadFilter<float> _biquad;

	LowPassFilter(float sampleRate, float cutoff, float q)
	: _sampleRate(sampleRate)
	, _cutoff(cutoff)
	, _q(q)
	{
	}

	void setParams(float sampleRate, float cutoff, float q);
	virtual float next(float sample) override {
		return _biquad.next(sample);
	}
};

struct MultipoleFilter : Filter {
	enum Type {
		LP_TYPE,
		HP_TYPE
	};

	static constexpr int maxPoles = 20;
	static constexpr float maxRipple = 0.29f;
	Type _type = LP_TYPE;
	int _poles = 1;
	float _sampleRate = 0.0f;
	float _cutoff = 0.0f;
	float _ripple = 0.0f;
	BiquadFilter<double> _biquads[maxPoles / 2];

	MultipoleFilter() {}

	void setParams(
		Type type,
		int poles,
		float sampleRate,
		float cutoff,
		float ripple // FIXME: using this with more than two poles creates large gain, need compensation.
	);
	virtual float next(float sample) override;
};

} // namespace dsp
} // namespace bogaudio
