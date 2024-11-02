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
#define POLYNOMIAL_ORDER	 2
#define MAX_ORDER POLYNOMIAL_ORDER
// Max a_order for polynomial fit
#define MAX_ARORDER			10
#define MIN_ARORDER			10
// to parse the input text
#define DATA_READ_LEN		32
#define MAX_BUFFER_LEN		1024
#ifndef M_PI
	#define M_PI			3.141592654
#endif
// Changing this will change the base frequency so you must change the
// a_decimation rate accordingly
#define SAMPLE_PERIOD		1

// Used only in splitting raw data
#define SAMPLE_SKIP			5
#define BASE_FREQ			250
#define MA_POINTS			20
// Currently system handles only 250 Hz base frequency and its multiples
#define FRAME_LENGTH (BASE_FREQ*SAMPLE_PERIOD)

#define REF					21
#define NO_MONTAGES			6
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
	template<typename T> 
	class Property
	{
	private:
		T mData;
	public:
		Property(void){}
		Property(const T& inp){ mData = inp;; }
		const T& operator *() const { return mData;}
		T& operator *(){ return mData;}
		Property<T>& operator= (const T& inp) { mData = inp; }
	};


	typedef std::vector<double> TARRAY;

	struct TMARKER{
		std::string date;
		std::string time;
	};

	typedef std::vector<TMARKER> TARRAY_MARKER;

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
		Arguments(void);
		~Arguments(void);

		void ProcessGenericArgs(const long& argc,
					char *argv[]);

		void ProcessSplitDirect(const long& argc,
					char *argv[]);
		// "debug" == true
		Property<bool> Debug;
		// Type of Operations: Cepstrum, RA, Spectrum, Raw, Dump, Single

		Property<Operation> OperationType;
		// Input file
		Property<std::string> FileName;
		// decimation == 2 for 500 Hz
		Property<long> Decimation;
		// factor == 1000 for mV
		Property<long> Factor;

		Property<long> NoOfCols;

		Property<std::vector<long> > Ignores;

		Property<long> NoOfIgnores;

		Property<long> NoOfSamples;
	};//

	class Chebyshev
	{
	public:
            /*!
            @name p_Chebyshev
            @param a_data 1D array that will store filter data in time domain
            @param *a_A Filter coefficients
            @param *a_B Filter coefficients
            @param a_FilterLength length in time domain
            @return void
            */

		void
		operator () (TARRAY& a_data, // return value
				const long& order, // 10
				const double& epsilon, // 0.5
				const long& Fc, // cutoff frequency: 125;
				const long& a_FilterLength)const;

	};

	class PolyVal{
	public:
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
		void operator () (const TARRAY& a_X,
				TARRAY& a_Y,
				const long& a_length,
				const long& a_order,
				const TARRAY& a_result)const;
	};
	class PolyFit
	{
	public:
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
		void 
		operator () (const TARRAY& a_X,
				const TARRAY& a_Y,
				const long& a_length,
				const long& a_order,
				TARRAY& a_result)const;
	};


	class PostProcess{
	protected:
		ARBurg arburg;
		ARCeps arceps;
	public:
		PostProcess(){}
		/*!
		@name p_SumOfCepstrums
		@brief Search and look for its use ! :)
		@param a_data 1D array upon which function will operate
		@param perform used to calculate system performance data
		@return l_sumc := double
		*/
		double p_SumOfCepstrums(const TARRAY& a_data,
					TARRAY& a_wgn,
					TARRAY& a_AR,
					double& a_Ep,
					long& a_length,
					long& a_order)const;

		/*!
		@name p_SumOfSpectrum
		@brief Search and look for its use ! :)
		@param a_data 1D array upon which function will operate
		@param a_avgFPE used to calculate average final prediction error
		@param a_montage search and look for its uses in this function!
		@return Full spectral energy
		*/
		double p_SumOfSpectrum(const TARRAY& a_AR,
					const double& a_Ep,
					double& a_frameRA,
					const double& a_length,
					const long& a_order)const;

		//@}

		double p_wgnSFR(TARRAY& a_data,
				const long& a_length)const;

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
		double p_GetOptimumAR(const TARRAY& a_data,
					TARRAY& a_wgn,
					TARRAY& a_AR,
					double& a_Ep,
					long& a_length,
					long& a_order)const;
	public:
		PolyFit polyfit;
	};

	class PreProcess{
	protected:
		Filter filter; // used in pre
		PolyFit polyfit; // used in BLW
		PolyVal polyval;
		MovingAverageFilter maFilter;
	public:
		PreProcess(){}
		//@{
		/*!
		@name p_PreProcess
		@brief A Function to preprocess raw data
		@param a_data: is two dimensional array holds channel data
		@return void
		This functions is constant if it is release version
		otherwise non const and dump sample result of each sub
		process to the output file
		*/
		void p_PreProcess(TARRAY& a_data)const;

		/*!
		@name p_DcRemove
		@brief A Function to remove dc
		@param inputFrame - 1D array
		@return void
		This function is tightly coupled with this process
		It is not designed as a generic utility
		*/
		void p_DcRemove (TARRAY& a_data,
					const size_t& length)const;

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
		void p_RemoveTrend(TARRAY& a_data,
						const long& length,
						const double& period)const;

		//@}

		/*!
		@name p_WriteDump
		@brief A Function to write used in dumping raw, preprocessed 
                        a_data
		@param
		a_data: is two dimensional array := no_of_montage*frameLength
		a_marker: 3-d : frameLength*2*dataReadLength
		@return void
		*/
		void p_WriteDump(const TARRAY* const a_data,
			const TARRAY_MARKER& a_marker);

		/*!
		@name p_WriteSingle
		@brief A Function to write used in dumping raw, preprocessed 
                        a_data
		@param a_data: is two dimensional array: Raw Data
		@return void
		*/
		void p_WriteSingle(const TARRAY* const a_data,
			const TARRAY_MARKER& a_marker);
	};

	class EEGReader{
	public:
		void init(const Arguments& a_args);
		void finalize(void);
		// unique long identifies a sample
		Property<unsigned long long> RowTags;

		std::ifstream p_inputFile;

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
		long p_Read(TARRAY* a_data,
					TARRAY_MARKER& a_marker);
		/*!
		@name p_ProcessMontages
		@brief A Function to create montages from channels
		@param void
		@return void
		*/
		void p_ProcessMontages(TARRAY* a_montage,
					TARRAY* a_data)const;
	};

	class EEGWriter{
	public:
		void init(void);
		void finalize(const Arguments& a_args);
		// see write
		Property<unsigned long long> RowCounters;
		// see write
		std::ofstream m_headerFile, m_timestampFile;
		/*!
		@name p_Write
		@brief A Function to write cepstrum analysis to text file
		@param a_avg_FPE: is average final prediction error, 
                        size=no_of_montage
		@return void
		*/
		void p_Write(
			const TARRAY a_avgFPE,
			const TARRAY a_order,
			const TARRAY a_length,
			TARRAY_MARKER& a_marker);

	};

	/// class ProcControl
	class ProcControl
	{
		//! public construtors and destructors
		//@{
		/*!
		ProcControl
		@return
		*/
		ProcControl(const Arguments& args);

		/*!
		~ProcControl is public destructor
		@return
		*/
		~ProcControl();
		//@}

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

		static inline ProcControl* get(const Arguments* args = NULL)
		{
			static ProcControl s_procControl(*args);
			static bool firstTime = true;
			if(firstTime) {
				s_procControl.writer.init();
				firstTime = false;
			}
			return (&s_procControl);
		}

		void finalize(void);

		Property<double> MeanFPE;

		Property<double> MeanOrder;

		Property<double> MeanLength;

		const Arguments& m_args;

	protected:
		EEGReader reader;
		EEGWriter writer;
		PreProcess preObj;
		PostProcess postObj;
   	}; // end class
}// end namespace

#endif
