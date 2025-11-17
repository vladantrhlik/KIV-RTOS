#pragma once

class CVirtual_Patient {
	private:
		int mRandom_Device_File = -1;

		// aktualni cas
		unsigned int mCurrent_Time_Mins = 0;

		struct TState {
			float Q1 = 300;
			float Q2 = 200;
			float I = 0;
			float X = 0;
			float S1 = 0;
			float D1 = 0;
			float D2 = 0;
		};

		// aktualni stav pacienta
		TState mState;

		// cas do dalsiho jidla
		unsigned int mMins_To_Next_Meal = 0;

	public:
		CVirtual_Patient();

		// provede krok v simulaci
		void Step();

		// zjisti aktualni glykemii
		float Get_Current_Glucose();

		// davkuje inzulin
		void Dose_Insulin(float dose);
};
