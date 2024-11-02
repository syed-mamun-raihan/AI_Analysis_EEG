namespace boost { namespace math {
	// This code is licensed under GPLv3
	// @ copy right Syed Mamun Raihan - 2008
	// shaikat@hotmail.com
	// Author provides no warranty or suitability for certain application
	template<typename T>
	class Filter
        {
        public:
                /*!
                @param data 1D array upon which function will operate
                @param A_Coeff Filter coefficients
                @param B_Coeff Filter coefficients
                @return void
                This function implements Generic Filter algorithm based on
                Matlab documentation.
                Filter constants must be of same length
                */
                void
                operator () (std::vector<T>& data,
                        	const std::vector<T>& A_Coeff,
                        	const std::vector<T>& B_Coeff) const;

        };

	template<typename T>
	class MovingAverageFilter
	{
	        Filter<T> filter;	
        public:
                /*!
                @name p_MAFilter
                @brief A Moving Average Filter Function
                @param data - 1D array
                @return void
                */
                void
                operator () (std::vector<T>& data)const;
	};

	template<typename T>
	class Convolution
	{
	protected:
		Filter<T> filter;
	public:
		void
		operator () (const std::vector<T>& data1,
				const std::vector<T>& data2,
				std::vector<T>& result)const;//result must be length1+length2-1
	};

	template<typename T>
	class HanningCorrection
	{	
	public:
		void
		operator () (std::vector<T>& a_data,
			const long& correctionLength) const;
	};

	template<typename T>
	class ARBurg{
	protected:
		Filter<T> filter;
		HanningCorrection<T> hncorr;
	public:
		/*!
		@name p_ArBurg
		@brief Search and look for its use ! :)
		@param a_data 1D array upon which function will operate
		@param a_Ar used to return AR coefficients
		@param a_fpeData used to calculate system performance
		@param a_length segment length to window a_data
		@return lowestOrder := long
		This function implements Burg's algorithm for AR Model
		a_length determines the max segment size for the a_data[]
		it can be different from the actual size
		*/
		const T
		operator () (const std::vector<T>& a_data,
				std::vector<T>& a_wgn, // error terms
				std::vector<T>& a_Ar,
				const long& max_arorder) const; //data length
		//@}
	};

	template<typename T> 
	class ARCeps
	{
	protected:
		ARBurg<T> arburg;
	public:
		/*!
		@name p_ArCeps
		@param a_data 1D array upon which function will operate
		@param a_ceps used to return cepstrum coefficients
		*/
  		double
		operator () (const std::vector<T>& a_Ar, // MAX_ARORDER+1
			std::vector<T>& a_ceps,
			const long& a_order,
			const long& datalen) const;
	};
	// This code is licensed under GPLv3
	// @ copy right Syed Mamun Raihan - 2008
	// shaikat@hotmail.com
	template<typename T> inline void
	Filter::operator() (std::vector<T>& data,
                        const std::vector<T>& A_Coeff,
                        const std::vector<T>& B_Coeff)
	{
		if(A_Coeff.length() != B_Coeff.length()) return;
		const size_t FilterLength = A_Coeff.length();
	        std::vector<T> l_X ( FilterLength,0.0 );//FIFO
                std::vector<T> l_Y ( FilterLength,0.0 );//FIFO
                std::vector<T> l_A ( FilterLength,0.0 );//FIFO
                std::vector<T> l_B ( FilterLength,0.0 );//FIFO

                // initialize the circular FIFO buffers
		l_A = A_Coeff;
                l_B = B_Coeff;

                long b_index = 0;// b vector index
                long index = 0; // temp index
                double l_num = 0.0;
                for ( long x = 0; x < data.size();x++ )
                {
                        // Update current element in X(j) vector
                        l_X[b_index] = data[x];
                        // temporary index based on current b_index
                        // that will be used to loop through coefficients and X(j)
                        // Y(i) vector
                        index = b_index;
                        l_num = l_B[0]*l_X[index];
                        // wrap around index to implement circular buffer
                        if ( --index < 0 )
                                index = ( long ) FilterLength -1;
                        // using difference equation
                        for ( long i=1; i < FilterLength; i++ )
                        {
                                l_num += ( l_B[i]*l_X[index] - l_A[i]*l_Y[index] );
                                if ( --index < 0 )
                                        index = ( long ) FilterLength -1;
                        }
                        // Now update current element in Y(i) vector
                        l_Y[b_index] = l_num;
                        // Store the filtered data
                        data[x] = l_Y[b_index];
                        // current element index in the X(j) and Y(i) need to be incremented
                        // and wrap around
                        if ( ++b_index == FilterLength )
                                b_index = 0;
                } //  end processing filtering
	}// end filter function

       /*
        * This filter behaves like Matlab MA Filter
        */
        template<typename T> inline void
        MovingAverageFilter::operator () ( std::vector<T>& data, const size_t movingAvgPoints) const
        {
                // The FIFO that will hold the a_data for MA Filter @ 50 Hz
                std::vector<T> l_A (movingAvgPoints,0.0 );
                std::vector<T> l_B (movingAvgPoints,0.0 );
                l_A[0]=1.0;
                l_B[0]=1.0/movingAvgPoints;

                for ( long i=1; i < movingAvgPoints; i++ )
                {
                        l_B[i] = l_B[0];
                        l_A[i] = 0.0;
                }
                // Now filter
                filter(data, l_A, l_B);
        }//end function
	
	template<typename T> inline void
	Convolution::operator () ( const std::vector<T>& data1,
	                 const std::vector<T>& data2,
	                 std::vector<T>& result ) const//must be length1+length2-1
	{
		const long convLength = data1.length()+ data2.length()-1;
		if(result.length() < convLength) return;
		std::vector<T> l_A ( convLength,0.0 );
		std::vector<T> l_B ( convLength,0.0 );
		for ( long x=0;x<convLength;x++ )
		{
			if ( x < data1.length() )
			{
				l_B[x] = data1[x];
				result[x] = data2[x];
			}
			else if ( x < data2.length() )
			{
				result[x] = data2[x];
			}
			else
			{
				result[x] = 0;
			}
		}
		l_A[0]=1;
		filter( result, l_A, l_B );
	}

	template<typename T> inline void
	HanningCorrection::operator () ( std::vector<T>& data,
	                              const long& correctionLength) const
	{
		const long hann_len = correctionLength/2;
		for ( long x = 1;x < hann_len;x++ )
		{
			double hann_coef = 0.5* ( 1 - cos ( 2*M_PI*x/hann_len ) );
			data[x-1] = a_data[x-1] * hann_coef;
			data[correctionLength-x] =
			    data[correctionLength-x] * hann_coef;
		}
	}

	// This code calculates Ar Coefficients
	// returns the error terms variance Ep
	// Error terms wgn
	// Computed Ar coefficients a_Ar
	// Order of the auto-regressive model max_arorder
	template<typename T> inline const T
	ARBurg::operator () ( const std::vector<T>& a_data,
	                     std::vector<T>& a_wgn,
	                     std::vector<T>& a_Ar,
	                     const long& max_arorder) const
	{
		std::vector<T>	Ao ( max_arorder+1,0.0 );
		std::vector<T>	Bo ( max_arorder+1,0.0 );
		a_Ar.resize ( max_arorder+1,0.0 );

		for ( long i=0;i <= ( long ) a_Ar.capacity(); i++ )
			a_Ar[i] = 0.0;
		const long a_length =  a_data.length(); 
		// Forward Prediction Error - used recursively
		std::vector<T>	Fe ( a_length );

		// Backward Prediction Error - used recursively
		std::vector<T>	Be ( a_length );

		T Eo = 0.0; // variance
		for ( long n = 0; n < a_length; n++ )
		{
			Eo += a_data[n] * a_data[n];
			a_wgn[n] = Fe[n] = Be[n] = a_data[n];
		}
		// MSE
		Eo /= a_length;
		T den = 0.0 ;

		for ( long m = 1; m <= max_arorder; m++ )
		{
			Ao[0] = a_Ar[0] = 1;
			den = Ao[m] = 0.0; // same as Km

			for ( long n = m ; n < a_length; n++ )
			{
				// Numerator
				Ao[m] += Fe[n] * Be[n - 1];
				den += Be[n - 1] * Be[n - 1] +
				       Fe[n] * Fe[n];
			}

			Ao[m] = Ao[m] * ( -2 ) / den;

			a_Ep = Eo;
			Eo = ( 1 - Ao[m] * Ao[m] ) * Eo;

			// now sets AR coefficients for order m
			for ( long i = 1; i <= m - 1; i++ )
			{
				Ao[i] =
				    a_Ar[i] + Ao[m] * a_Ar[m - i];
			}
			double temp = 0;
			double Befactor = 0;
			for ( long n = 0; n < a_length - m; n++ )
			{
				temp = Be[n];
				Be[n] = Befactor + Ao[m] * Fe[n];
				Fe[n] = Fe[n] + Ao[m] * Befactor;
				Befactor = temp;
			}
			// rest of them has zero error
			for ( long n = a_length - m; n < a_length; n++ )
			{
				Fe[n] = Be[n] = 0.0;
			}

			for ( long i=0; i <= m; i++ )
			{
				a_Ar[i] = Ao[i];
			}
		}// end max arorder
		Bo[0] = 1;
		filter ( a_wgn, &Bo, &Ao); // inverse filtering
		hncorr ( a_wgn, a_length );
		return a_Ep;
	} //  end function p_ArBurg

	template<typename T> inline const T
	ARCeps::operator () ( const std::vector<T>& a_Ar, // [MAX_ARORDER+1]
	                     std::vector<T>& a_ceps,
	                     const long& a_order,
	                     const long& dataLen ) const
	{
		a_ceps[0] = 0.0;

		T l_Sumc = fabs ( a_ceps[0] );

		for ( long i = 1; i <= a_order; i++ )
		{
			a_ceps[i] = a_Ar[i];
			for ( long k = 1; k < i; k++ )
			{
				a_ceps[i] +=
				    double ( k ) / double ( i ) * a_ceps[k] * a_Ar[i-k];
			}
			a_ceps[i] *= -1;
			l_Sumc += fabs ( a_ceps[i] );
		}

		for ( long i = a_order+1; i <= dataLen; i++ )
		{
			a_ceps[i] = 0.0;
			for ( long k = 1; k < i; k++ )
			{
				a_ceps[i] += ( ( 1 - double ( k ) / double ( i ) )
				               * a_ceps[k] * a_Ar[k] );
			}
			a_ceps[i] *= -1;
			l_Sumc += fabs ( a_ceps[i] );
		}
		return l_Sumc;
	}//  end function p_ArCeps

}// math
}// boost