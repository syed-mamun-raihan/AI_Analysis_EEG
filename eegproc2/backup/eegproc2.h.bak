/***************************************************************************
*   Copyright (C) 2005 by Syed Mamun Raihan   *
*   sraihan@uwo.ca   *
*                                                                         *
***************************************************************************/
#ifndef EEGPROC2_H
#define EEGPROC2_H

//-----------------------
// constants
// Actual a_order used in polynomial fit
#define POLYNOMIAL_ORDER 2
#define MAX_ORDER POLYNOMIAL_ORDER
// Max a_order for polynomial fit
#define MAX_ARORDER 40
#define MIN_ARORDER 10
#define MAX_FILTER_POINTS 5
// to parse the input text
#define DATA_READ_LEN 32
#define MAX_BUFFER_LEN 1024
#ifndef M_PI
#define M_PI 3.141592654
#endif
// Currently system handles only 250 Hz base frequency and its multiples
#define FRAME_LENGTH 1000
// Changing this will change the base frequency so you must change the
// a_decimation rate accordingly
#define SAMPLE_PERIOD 4

// Used only in splitting raw data
#define SAMPLE_SKIP   5
#define BASE_FREQ 250
#define MA_POINTS 4

#define REF 21
#define NO_MONTAGES 6
//----------------
//preprocess
#define RAW_TXT 			"raw.txt"
#define DC_TXT				"dc.txt"
#define BLW_TXT				"blw.txt"
#define MA_TXT				"ma.txt"
#define BUTTER_TXT			"butter6.txt"
#define HANNCOREC_TXT		"hanncorec.txt"
#define FINAL_TXT			"final.txt"
#define X_TXT				"X.txt"
#define Y_TXT				"Y.txt"
#define RESIDUALX_TXT		"residualX.txt"
#define XINTERPOLATE_TXT	"Xinterpolate.txt"
#define ALPHA_TXT			"alpha.txt"
#define SPEC_TXT			"spec.txt"
#define CEPS_TXT			"ceps.txt"
#define SFR_TXT				"sfr.txt"
#define ORDERS_TXT			"orders.txt"
#define LENGTHS_TXT			"lengths.txt"
#define ALPHAOUT_TXT		"alphaout.txt"
#define SPECOUT_TXT			"specout.txt"
#define CEPSOUT_TXT			"cepsout.txt"
#define SFROUT_TXT			"sfrout.txt"
#define ORDERSOUT_TXT		"ordersout.txt"
#define LENGTHSOUT_TXT		"lengthsout.txt"

//--------------------------------------------
namespace eegproc2
{
//The system uses simple COMMAND and SINGLETON pattern
//!@author Syed Mamun Raihan

	struct Channel_Struct{
		const long channel;
		const char* montage;
	};

//!  Channels vs Montages
static const
	Channel_Struct
	g_channels[] =
		{ {1, "C3"},
		{2, "C4"},
		{3, "Cz"},
		{4, "F3"},//
		{5, "F4"}, //
		{6, "F7"},
		{7, "F8"},
		{8, "Fz"},
		{9, "Fp1"},
		{10, "Fp2"},
		{11, "Fpz"},
		{12, "O1"},//
		{13, "O2"},//
		{14, "P3"},//
		{15, "P4"},//
		{16, "Pz"},
		{17, "T3"},//
		{18, "T4"},//
		{19, "T5"},
		{20, "T6"},
		{21, "Oz"}
		};

static const long noOfChannels = sizeof(g_channels)/sizeof(g_channels[0]);

// As per Vespa et. al.
static const long
	g_montages[][2] = {
			{5,18},//F4T4
		      	{18,15},//T4P4
		      	{15,13},//P4O2
		      	{4,17},//F3T3
		      	{17,14},//T3P3
		      	{14,12}//P3O1
			  };

	enum Operation{
		OTHER = 0,
		TREND, // Ceps, RA, Spec
		RAW,
		DUMP,
		SINGLE,
		SPLIT};

/// class Arguments
///This class process arguments in all cases except for splitting purpose
/// To split a processed data it just samples directly
/// Utility class
class Arguments{
public:
	void ProcessGenericArgs(const long& argc,
				char *argv[]);

	void ProcessSplitDirect(const long& argc,
				char *argv[]);
	// "debug" == true
	bool m_boolDebug;
	// Type of Operations: Cepstrum, RA, Spectrum, Raw, Dump, Single

	Operation m_operationType;
	// Input file
	char* m_fileName;
	// decimation == 2 for 500 Hz
	long m_decimation;
	// factor == 1000 for mV
	long m_vFactor;

	long m_noOfCols;

	long* m_ignores;

	long m_noOfIgnores;

	long m_noOfsamples;
};

/// class RawEEGData
class RawEEGData
	{
	public:
		//!Public entry functions
		//@{
		/*!
		@name Process
		@brief A Function to process Cepstrum/Spectrum/RA analysis
		@param void
		@return void
		*/
		void Process(void);

		/*!
		@name ProcessDump
		@brief To dump data with baseline wander removed
		@param void
		@return void
		*/
		void ProcessDump(void);

		/*!
		@name ProcessSingle
		@brief To dump data with baseline wander removed
		@param void
		@return void
		*/
		void ProcessSingle(void);

		/*!
		@name ProcessRaw
		@brief A Function to dump unprocessed data no analysis
		@param void
		@return void
		*/
		void ProcessRaw(void);

		//! public construtors and destructors
		//@{
		/*!
		RawEEGData
		@return
		*/
		RawEEGData(const Arguments& args);

		/*!
		~RawEEGData is public destructor
		@return
		*/
		~RawEEGData();
		//@}

		void finalize(void);

		const Arguments& m_args;

		/*!
		@name m_A
		@var it is public static constants
		@brief butterworth filter constants
		it generated using
		[b,a]=butter(6,[.001,.25],'bandpass')
		*/
		static	const double m_A[] ;

		/*!
		@name m_B
		@var it is public static constants
		@brief butterworth filter constants
		it generated using
		[b,a]=butter(6,.2,'low')
		*/
		static	const double m_B[];
		/*!
		@name m_FilterLength
		@var it is public static constants
		@brief butterworth filter constants' length
		*/
		static const long m_FilterLength;

		// see write
		std::ofstream m_headerFile, m_timestampFile;

		// performance
		unsigned long long m_rowCounters;

		double m_meanFPE;

		double m_meanOrder;

		double m_meanLength;

	protected:
		/*!
		@name p_PreProcess
		@brief A Function to preprocess raw data
		@param a_data: is two dimensional array holds channel data
		@return void
		This functions is constant if it is release version
		otherwise non const and dump sample result of each sub
		process to the output file
		*/
		void p_PreProcess(double a_data[FRAME_LENGTH])const;

		//!Protected preprocess utility function
		//@{
		/*!
		@name p_DcRemove
		@brief A Function to remove dc
		@param inputFrame - 1D array
		@return void
		This function is tightly coupled with this process
		It is not designed as a generic utility
		*/
		void p_DcRemove (double a_data[FRAME_LENGTH])const;

		/*!
		@name p_RemoveBLW
		@brief Search and look for its use ! :)
		@param inputFrame 1D array upon which function will operate
		@return void
		This function is tightly coupled with this process
		It is not designed as a generic utility
		-----------------------------
		This function uses 2nd order polynomial to remove
		baseline wander
		*/
		void p_RemoveBLW(double a_data[FRAME_LENGTH])const;

		/*!
		@name p_FilterByFFT
		@brief Search and look for its use ! :)
		@param inputFrame 1D array upon which function will operate
		@return void
		This function is tightly coupled with this process
		It is not designed as a generic utility
		-----------------------------
		This function uses FFT to remove Low and High Frequency
		*/
		void p_FilterByFFT(double a_data[BASE_FREQ])const;

		void p_HanningCorrection(double a_data[FRAME_LENGTH],
					const long& a_length)const;
		/// Not in use
		/*!
		@name p_MAFilter
		@brief A Moving Average Filter Function
		@param a_data - 1D array
		@param a_noelements - No of elements to be avaraged
		@return void
		This function is tightly coupled with this process
		It is not designed as a generic utility
		*/
		void p_MAFilter(double a_data[],const long& a_noelements)const;
		/*
		*Use the above version instead
		*/
		void p_MA(double a_data[],const long& a_noelements)const;

		/*!
		@name p_ProcessMontages
		@brief A Function to create montages from channels
		@param void
		@return void
		*/
		void p_ProcessMontages(double a_montage[NO_MONTAGES][FRAME_LENGTH],
					double a_data[REF][FRAME_LENGTH])const;

		/*!
		@name p_Filter
		@brief Search and look for its use ! :)
		@param a_data 1D array upon which function will operate
		@param *a_A Filter coefficients
		@param *a_B Filter coefficients
		@param a_FilterLength Filter coefficients length
		@return void
		This function implements Generic Filter algorithm
		Filter constants a_A and a_B must be of same a_length
		*/
		void p_Filter(double* a_data,
				const long& len,
				const double* a_A,
				const double* a_B,
				const long& )const;
		//@}

		//! Functions that execute specific commands
		//@{
		/*!
		@name p_SumOfCepstrums
		@brief Search and look for its use ! :)
		@param a_data 1D array upon which function will operate
		@param perform used to calculate system performance data
		@return l_sumc := double
		*/
		double p_SumOfCepstrums(const double a_data[FRAME_LENGTH],
					double a_wgn[FRAME_LENGTH],
					double* a_AR,
					double& a_Ep,
					double& a_length,
					long& a_order,
					double& a_perform)const;

		/*!
		@name p_SumOfSpectrum
		@brief Search and look for its use ! :)
		@param a_data 1D array upon which function will operate
		@param a_avgFPE used to calculate average final prediction error
		@param a_montage search and look for its uses in this function!
		@return Full spectral energy
		*/
		double p_SumOfSpectrum(const double* a_AR,
					const double& a_Ep,
					const double& a_length,
					const long& a_order,
					double& a_frameRA)const;

		//@}

		double p_wgnSFR(double a_data[FRAME_LENGTH],
				const long& a_length)const;

		//!Protected file IO functions
		//@{
		/*!
		@name p_WriteDump
		@brief A Function to write used in dumping raw, preprocessed a_data
		@param a_data: is two dimensional array, processed data for dumping
		to a file
		@return void
		*/
		void p_WriteDump(const double a_data[NO_MONTAGES][FRAME_LENGTH],
			const char a_marker[FRAME_LENGTH][2][DATA_READ_LEN]);

		/*!
		@name p_WriteSingle
		@brief A Function to write used in dumping raw, preprocessed a_data
		@param a_data: is two dimensional array: Raw Data
		@return void
		*/
		void p_WriteSingle(const double a_data[NO_MONTAGES][FRAME_LENGTH],
					const char a_marker[FRAME_LENGTH][2][DATA_READ_LEN]);

		/*!
		@name p_Write
		@brief A Function to write cepstrum analysis to text file
		@param a_avg_FPE: is average final prediction error
		@return void
		*/
		void p_Write(//const double a_slopeCeps[NO_MONTAGES],
			//	const double a_slopeSpec[NO_MONTAGES],
			//	const double a_slopeRA[NO_MONTAGES],
			//	const double a_slopeSFR[NO_MONTAGES],
			//	const double a_slopeOrder[NO_MONTAGES],
			//	const double a_slopeLength[NO_MONTAGES],
			const double a_avgFPE[NO_MONTAGES],
			const double a_order[NO_MONTAGES],
			const double a_length[NO_MONTAGES],
			const char a_marker[FRAME_LENGTH][2][DATA_READ_LEN]);

		/*!
		@name p_Read
		@brief A Function to read 2D raw data from text file
		@param a_data: is two dimensional array holds channel data
		@param a_marker: 3-D marker timestamp
		@return void
		This function read consecutive samples for all channels
		incase of debug version it dumps sample header of the file
		to the output file
		*/
		long p_Read(double a_data[REF][FRAME_LENGTH],
			char a_marker[FRAME_LENGTH][2][DATA_READ_LEN]);
		//@}

		///! Utility functions to support executing commands
		//@{
		// used while debugging only
		inline void p_DebugWrite(const double* a_data,
				const char* a_fileName,
				const long& a_length,
				const int& mode = 0)const;

		/*!
		@name p_GetOptimumAR
		@brief Search and look for its use ! :)
		@param a_data 1D array upon which function will operate
		@return void
		This function is tightly coupled with this process
		It is not designed as a generic utility
		-----------------------------
		This function looks for optimum segment length
		*/
		long p_GetOptimumAR(const double a_data[FRAME_LENGTH],
					double a_wgn[FRAME_LENGTH],
					double* a_AR,
					double& a_Ep,
					long& a_order,
					double& a_perform)const;

		/*!
		@name p_PolyVal
		@brief Search and look for its use ! :)
		@param a_X[] 1D array for which values need to be predicted
		@param a_Y[] 1D array of predicted values for each a_X[]
		@param a_length length of a_X[]
		@param a_order order of the polynomial
		@param a_coef[] polynomial coeffiecients
		@return void
		This function is tightly coupled with this process
		It is not designed as a generic utility
		*/
		void p_PolyVal(const double a_X[],
			double a_Y[],
			const long& a_length,
			const long& a_order,
			const double a_result[])const;

		/*!
		@name p_PolyFit
		@brief Search and look for its use ! :)
		@param a_X[] 1D array for which values need to be predicted
		@param a_Y[] 1D array of values for each a_X[]
		@param a_length length of a_X[]
		@param a_order order of the polynomial
		@param a_result[] polynomial coeffiecients
		@return void
		This function is tightly coupled with this process
		It is not designed as a generic utility
		*/
		void p_PolyFit(const double a_X[],
			const double a_Y[],
			const long& a_length,
			const long& a_order,
			double a_result[])const;

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
		const long
		p_ArBurg(const double a_data[FRAME_LENGTH],
				double a_wgn[FRAME_LENGTH],
				double a_Ar[MAX_ARORDER],
				double& a_fpeData,
				double& a_Ep_init,
				const long& a_length) const;
		//@}

		/*!
		@name p_ArCeps
		@param a_data 1D array upon which function will operate
		@param a_ceps used to return cepstrum coefficients
		*/
  		double
		p_ArCeps(const double a_Ar[MAX_ARORDER+1],
			double a_ceps[MAX_ARORDER+2], const long& a_order) const;

		//@}

		// unique long identifies a sample
		unsigned long long p_rowTags;

		// see read
		std::ifstream p_inputFile;
   	}; // end class
}// end namespace

#endif
