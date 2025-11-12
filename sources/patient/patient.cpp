#include "patient.h"

#include <stdfile.h>

namespace {
	// prevodni faktory
	constexpr float mgdl_to_mmol = 18.018f;
	constexpr float iu_to_mu = 50.0f;

	float floatmax(float a, float b) {
		return a > b ? a : b;
	}
}

// parametry modelu
namespace params {
	constexpr float k12 = 0.4f;
	constexpr float kIQ = 0.6f;
	constexpr float kId = 0.3f;
	constexpr float kIabs = 0.35f;
	constexpr float kD2Q = 0.4f;
	constexpr float kEmp = 0.001f;
	constexpr float Qthreshold = 250.0f;
}

CVirtual_Patient::CVirtual_Patient() {
	mMins_To_Next_Meal = 20;
	mRandom_Device_File = open("DEV:trng", NFile_Open_Mode::Read_Only);
}

void CVirtual_Patient::Step() {
	TState current_state = mState;

	// toto je v podstate redukovana verze Hovorkova modelu

	const float k_emp = mState.Q1 > params::Qthreshold ? params::kEmp : 0.0f;

	mState.Q1 += -params::k12 * (current_state.Q1 - current_state.Q2) - current_state.X * params::kIQ + params::kD2Q * current_state.D2 - k_emp * current_state.Q1;
	mState.Q2 += params::k12 * (current_state.Q1 - current_state.Q2);
	mState.I += params::kIabs * current_state.S1 - params::kId * current_state.I;
	mState.X += params::kId * current_state.I - params::kIQ * current_state.X;
	mState.S1 += -params::kIabs * current_state.S1;
	mState.D1 += -params::kD2Q * current_state.D1;
	mState.D2 += params::kD2Q * current_state.D1 - params::kD2Q * current_state.D2;

	mState.D1 = floatmax(0.0f, mState.D1);
	mState.D2 = floatmax(0.0f, mState.D2);
	mState.Q1 = floatmax(0.0f, mState.Q1);
	mState.Q2 = floatmax(0.0f, mState.Q2);
	mState.I = floatmax(0.0f, mState.I);
	mState.X = floatmax(0.0f, mState.X);
	mState.S1 = floatmax(0.0f, mState.S1);

	mCurrent_Time_Mins += 5;
	if (mMins_To_Next_Meal > 5) {
		mMins_To_Next_Meal -= 5;
	}
	else {
		uint32_t rnd = 0;
		read(mRandom_Device_File, reinterpret_cast<char*>(&rnd), sizeof(rnd));

		const float cho = (rnd & 0xFF) % 56 + 5; // 5 - 60 g sacharidu
		mState.D1 += cho;

		mMins_To_Next_Meal = (static_cast<unsigned int>(((rnd >> 8) % 121) + 120)); // 120 - 240 min do dalsiho jidla
	}
}

float CVirtual_Patient::Get_Current_Glucose() {
	return mState.Q1 / mgdl_to_mmol;
}

void CVirtual_Patient::Dose_Insulin(float dose) {
	mState.S1 += dose * iu_to_mu;
}
