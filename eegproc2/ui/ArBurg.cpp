	const long
	RawEEGData::p_ArBurg(const double a_data[FRAME_LENGTH],
				double a_wgn[FRAME_LENGTH],
				double a_Ar[MAX_ARORDER+1],
				double& a_fpeData,
				double& a_Ep,
				const long& a_length) const
	{
		double	Ao[MAX_ARORDER + 1];
		double  Bo[MAX_ARORDER+1];

		for(long i=0;i<=MAX_ARORDER; i++)
			Ao[i] = a_Ar[i] = Bo[i] = 0.0;

		// Forward Prediction Error - used recursively
		double*	Fe = new double[a_length];

		// Backward Prediction Error - used recursively
		double*	Be=new double[a_length];

		double Eo = 0.0; // variance
		for (long n = 0; n < a_length; n++)
		{
			Eo += a_data[n] * a_data[n];
			a_wgn[n] = Fe[n] = Be[n] = a_data[n];
		}
		// MSE
		Eo /= a_length;

		long	lowestOrder = MAX_ARORDER;

		// Monotonically punished by Order
		double last_fpe_criteria = MIN_DESC_LENGTH(Eo,0,a_length); // Ep/MAX_EP
		double cur_fpe_criteria = last_fpe_criteria; // Ep/MAX_EP

		double	den = 0.0 ;

		for (long m = 1; m <= MAX_ARORDER; m++)
		{
			Ao[0] = a_Ar[0] = 1;
			den = Ao[m] = 0.0; // same as Km

			for (long n = m ; n < a_length; n++)
			{
				// Numerator
				Ao[m] += Fe[n] * Be[n - 1];
				den += Be[n - 1] * Be[n - 1] +
					Fe[n] * Fe[n];
			}

			Ao[m] = Ao[m] * (-2) / den;

			a_Ep = Eo;
			Eo = (1 - Ao[m] * Ao[m]) * Eo;

			// now sets AR coefficients for order m
			for (long i = 1; i <= m - 1; i++){
				Ao[i] =
				a_Ar[i] + Ao[m] * a_Ar[m - i];
			}
			// Select the order and exit
			cur_fpe_criteria =
				MIN_DESC_LENGTH(Eo, m, a_length);

			if(Eo < 0 || fabs(Ao[m]) > 1  // Stability test
				|| (cur_fpe_criteria > last_fpe_criteria &&
				m > MIN_ARORDER))
			{
				lowestOrder = m -1;
				break;
			}else{
				last_fpe_criteria = cur_fpe_criteria;
			}
			double temp = 0;
			double Befactor = 0;
			for (long n = 0; n < a_length - m; n++)
			{
				temp = Be[n];
				Be[n] = Befactor + Ao[m] * Fe[n];
				Fe[n] = Fe[n] + Ao[m] * Befactor;
				Befactor = temp;
			}
			// rest of them has zero error
			for(long n = a_length - m; n < a_length; n++){
				Fe[n] = Be[n] = 0.0;
			}

			for(long i=0; i <= m; i++){
				a_Ar[i] = Ao[i];
			}
		}// end max arorder
		Bo[0] = 1; // all other elements are set to zero
		this->p_Filter(a_wgn, a_length, Bo,a_Ar,lowestOrder); // inverse filtering

		this->p_HanningCorrection(a_wgn, a_length);

		a_fpeData = a_Ep;
		delete[] Fe; Fe = NULL;
		delete[] Be; Be = NULL;
		return lowestOrder;
	} //  end function p_ArBurg
