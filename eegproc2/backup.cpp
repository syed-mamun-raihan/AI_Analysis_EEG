void
p_HanningCorrection(double a_data[FRAME_LENGTH],
					const int& a_length)
	{
		// Removing edges with 5% hanning taper
		int hann_len = a_length/10+1;
		for(int x = 1;x < int(hann_len/2);x++){
			double hann_coef = 0.5*(1 - cos(2*M_PI*x/hann_len));
			a_data[x-1] = a_data[x-1] * hann_coef;
			a_data[FRAME_LENGTH-x] =
				a_data[FRAME_LENGTH-x] * hann_coef;
		}
	}

const int p_ArBurg(const float a_data[FRAME_LENGTH],
				float a_wgn[FRAME_LENGTH],
				float a_Ar[MAX_ARORDER+1],
				float& a_fpeData,
				float& a_Ep,
				const int& a_length)
	{
		double	Ao[MAX_ARORDER + 1];

		for(int i=0;i<=MAX_ARORDER; i++)
			Ao[i] = a_Ar[i] = 0.0;

		// Forward Prediction Error - used recursively
		double*	Fe = new double[a_length];

		// Backward Prediction Error - used recursively
		double*	Be=new double[a_length];

		double Eo = 0.0; // variance
		a_Ep = 0.0; // mean of white noise
		for (int n = 0; n < a_length; n++)
		{
			Eo += a_data[n] * a_data[n];
			Fe[n] = Be[n] = a_data[n];
			a_Ep += fabs(a_data[n]);
		}
		// MSE
		Eo /= a_length;
		a_Ep /= a_length;

		int	lowestOrder = MAX_ARORDER;

		// Monotonically punished by Order
		double cur_fpe_criteria = 1; // Ep/MAX_EP
		double last_fpe_criteria = 1; // Ep/MAX_EP
		// very important
		const double MAX_EP = a_Ep;

		double	den = 0.0 ;

		for (int m = 1; m <= MAX_ARORDER; m++)
		{
			Ao[0] = a_Ar[0] = 1;
			den = Ao[m] = 0.0; // same as Km

			for (int n = m ; n < a_length; n++)
			{
				// Numerator
				Ao[m] += Fe[n] * Be[n - 1];
				den += Be[n - 1] * Be[n - 1] +
					Fe[n] * Fe[n];
			}

			Ao[m] = Ao[m] * (-2) / den;

			Eo = (1 - Ao[m] * Ao[m]) * Eo;

			// now sets AR coefficients for order m
			for (int i = 1; i <= m - 1; i++){
				Ao[i] =
				a_Ar[i] + Ao[m] * a_Ar[m - i];
			}
			// Select the order and exit
			cur_fpe_criteria =
				MIN_LENGTH_ORDER(a_Ep/MAX_EP, m, a_length);

			if(Eo < 0 || fabs(Ao[m]) > 1  // Stability test
				|| (cur_fpe_criteria > last_fpe_criteria &&
				m > MIN_ARORDER))
			{
				lowestOrder = m -1;
				break;
			}else{
				last_fpe_criteria = cur_fpe_criteria;
			}
			// Calculate Forward/Backward Error & Ep
			a_Ep = 0.0;
			double temp = 0;
			double Befactor = 0;
			for (int n = 0; n < a_length - m; n++)
			{
				temp = Be[n];
				Be[n] = Befactor + Ao[m] * Fe[n];
				Fe[n] = Fe[n] + Ao[m] * Befactor;
				a_Ep += fabs(Fe[n]) + fabs(Be[n]);
				Befactor = temp;
			}
			// rest of them has zero error
			for(int n = a_length - m; n < a_length; n++){
				Fe[n] = Be[n] = 0.0;
			}

			for(int i=0; i <= lowestOrder; i++){
				a_Ar[i] = Ao[i];
			}
			a_Ep /= (2*(a_length-1)); // mean error
		}// end max arorder

		for(int n=0;n < a_length;n++)
			a_wgn[n]= (Fe[n]+Be[n])/2;

		this->p_HanningCorrection(a_wgn, a_length);

		a_fpeData = (a_Ep/MAX_EP);

		delete[] Fe; Fe = NULL;
		delete[] Be; Be = NULL;
		return lowestOrder;
	} //  end function p_ArBurg

// Filter constants a_A and a_B must be of same a_length

	void
	p_Filter(float* a_data,
				const int& len,
				const float* a_A,
				const float* a_B,
				const int& a_FilterLength)
	{
		double* l_X=new double[a_FilterLength];
		double* l_Y=new double[a_FilterLength];

		// initialize the circular FIFO
		for(int i=0; i < a_FilterLength;i++)
			l_Y[i] = l_X[i] = 0.0;

		int butter_b_index = 0;
		int index = 0;
		double l_num = 0.0;
		for(int x = 0; x < len;x++){
			// it is still unmodified
			l_X[butter_b_index] = a_data[x];

			index = butter_b_index;

			l_num = a_B[0]*l_X[index];

			if(--index < 0)
				index = a_FilterLength -1;
			// using difference equation
		 	for(int i=1; i < a_FilterLength; i++){
				l_num += (a_B[i]*l_X[index] - a_A[i]*l_Y[index]);
				// wrap around :)
				if(--index < 0)
					index = a_FilterLength -1;
			}
			l_Y[butter_b_index] = l_num;

			a_data[x] = l_Y[butter_b_index];
			// wrap around :)
			if(++butter_b_index == a_FilterLength)
				butter_b_index = 0;
		} //  end processing filtering
		delete[] l_X;l_X=NULL;
		delete[] l_Y;l_Y=NULL;
	}//end function

	float
	p_ArCeps(const float a_Ar[MAX_ARORDER+1],
			float a_ceps[MAX_ARORDER+2], const int& a_order) 
	{
		a_ceps[0] = 0.0;

		double l_Sumc = fabs(a_ceps[0]);

		for (int i = 1; i <= a_order; i++)
		{
			a_ceps[i] = a_Ar[i];
			for (int k = 1; k < i; k++)
			{
				a_ceps[i] +=
				double(k) / double(i) * a_ceps[k] * a_Ar[i-k];
			}
			a_ceps[i] *= -1;
			l_Sumc += fabs (a_ceps[i]);
		}
		return l_Sumc;
	}