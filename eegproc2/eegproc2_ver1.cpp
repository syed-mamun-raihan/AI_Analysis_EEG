/***************************************************************************
 *   Copyright (C) 2006 by Syed Mamun Raihan   				   *
 *   $Author$ Syed Mamun Raihan						   *
 *    sraihan@uwo.ca   							   *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <memory>
#include <ctime>
#include <complex>
#include "fftw3.h"

#include "eegproc2.h"

#if defined(WIN32)

static void my_fftw_write_char(char c, void *f) { fputc(c, (FILE *) f); }
#define fftw_export_wisdom_to_file(f) fftw_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwf_export_wisdom_to_file(f) fftwf_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwl_export_wisdom_to_file(f) fftwl_export_wisdom(my_fftw_write_char, (void*) (f))

static int my_fftw_read_char(void *f) { return fgetc((FILE *) f); }
#define fftw_import_wisdom_from_file(f) fftw_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwf_import_wisdom_from_file(f) fftwf_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwl_import_wisdom_from_file(f) fftwl_import_wisdom(my_fftw_read_char, (void*) (f))

#endif

using namespace std;
using namespace eegproc2;

// DCD_PERIOD * SAMPLE_PERIOD = 10 Minute
#define DCD_PERIOD 150

#define INIT_DEBUG_PRINT()	static bool firstTime = true;

#define EXEC_DEBUG_PRINT(data,length,file) \
		if (firstTime && m_args.m_boolDebug == true) \
		{\
			cout<<"writing\t" << file <<endl; \
			this->p_DebugWrite(data,file,length);\
		}
#define FINALIZE_DEBUG_PRINT() firstTime = false;

#define EXEC_PRINT(data,length,file) \
		this->p_DebugWrite(data,file,length, -1);// append data

#define TEST_IGNORE_CHANNEL(x,y) \
			bool l_skip; \
			l_skip = false; \
			for(long j=0; j < m_args.m_noOfIgnores;j++){ \
				if(x == m_args.m_ignores[j] || \
					y == m_args.m_ignores[j]){ \
					l_skip = true; \
				} \
			} \
			if(l_skip)continue;
// This is the extrapolated value
#define INTERPOLATE_FEATURE(x,val) \
		this->p_PolyFit (x_cord, \
					x, \
					m_args.m_noOfsamples, \
					1, l_result); \
		val =  \
		l_result[1]*x_cord[m_args.m_noOfsamples]/2 +l_result[0];\

void usage(){
	cout
	<< "\nHelp on command line option\n"
	<< "================================================\n"
	<< "<debug/release> <operation> <file> <decimation> "
	<< "<uV/mV> <totals> <ignores>\n"
	<< "================================================\n"
	<< "<file>: file name with directory & extension\n"
	<< "<Operation>: Trend, Dump, Raw, Single \n"
	<< "<decimation>: rate at which samples should be "
	<< "decimated like, 1 in every 2 samples is 2\n"
	<< "<uV/mV>if uV then use 1 otherwise use appropriate a_mvFactor\n"
	<< "<totals>: no of total channels (max: "<< noOfChannels <<")\n"
	<< "<a_ignores>: channels to ignore\n\n"
	<< "\n------------------Split Help --------------\n"
	<< "<file1> <date> <hour> <channels to sample> "
	<< "\n===========================================\n"
	<< "\nWARNING! In many options no check has been made\n\n";
	exit(0);
}

time_t g_startTime;
time_t g_endTime;

int
main (long argc, char *argv[])
{
	const long lc_minArgs = 2;
	if(argc < lc_minArgs){
		usage();
		// call dialog
	}

	Arguments l_args;

	const char* lc_Ops = argv[2];

	// ProcessArguments
	if(strcmp(lc_Ops,"Trend") == 0 ||
		strcmp(lc_Ops,"Dump") == 0 ||
		strcmp(lc_Ops,"Raw") == 0 ||
		strcmp(lc_Ops,"Single") == 0)
	{
		l_args.ProcessGenericArgs(argc, argv);
	}//---------
	else if(strcmp(lc_Ops,"Split") == 0){
		l_args.ProcessSplitDirect(argc, argv);
		return 0;
	}//----------
	else {
		cout << "invalid operation: "<< lc_Ops << endl;
		exit(0);
	}

	RawEEGData l_objData (l_args);

	// Do the Job
	// Sum of Spectrum or Cepstrum or Relative Alpha using AR Model
	if(l_objData.m_args.m_operationType == TREND)
	{
		l_objData.Process ();
	}//----------
	else if(strcmp(lc_Ops,"Dump") == 0){
		l_objData.ProcessDump ();
	}//------------
	else if(strcmp(lc_Ops,"Raw") == 0){
		l_objData.ProcessRaw ();
	}//----------
	else if(strcmp(lc_Ops,"Single") == 0){
		l_objData.ProcessSingle ();
	}//-------------
	else{
		cout << "invalid a_operation: "<< lc_Ops << endl;
		exit(0);
	}

	l_objData.finalize();

	return EXIT_SUCCESS;
}

namespace eegproc2
{
//[B,A]=butter(6,[1/500,32/250],'bandpass');

	const double
	RawEEGData::m_A[] =
	{1.0000000000000000e+00,	  -1.0459065385738768e+01,
	   5.0203456282232167e+01,	  -1.4625680924207003e+02,
	   2.8805743211776132e+02,	  -4.0410932773826829e+02,
	   4.1409415456473630e+02,	  -3.1230999839763689e+02,
	   1.7206761928351239e+02,	  -6.7540301089880714e+01,
	   1.7928832509939827e+01,	  -2.8898974592977531e+00,
	   2.1390455471046044e-01};

	const double
	RawEEGData::m_B[] =
	{ 3.0050926219308165e-05,	   0.0000000000000000e+00,
	  -1.8030555731584898e-04,	   0.0000000000000000e+00,
	   4.5076389328962241e-04,	   0.0000000000000000e+00,
	  -6.0101852438616322e-04,	   0.0000000000000000e+00,
	   4.5076389328962241e-04,	   0.0000000000000000e+00,
	  -1.8030555731584898e-04,	   0.0000000000000000e+00,
	   3.0050926219308165e-05};

	const long
	RawEEGData::m_FilterLength =
			sizeof(m_A)/sizeof(m_A[0]);

	void Arguments::ProcessSplitDirect(const long& argc,
		char *argv[])
	{
		const long lc_minArgs = 6;
		const long lc_maxArgs = 20;

		long channels[lc_maxArgs-lc_minArgs];
		double channels_data[lc_maxArgs-lc_minArgs];

		if(argc < lc_minArgs ||
			argc > lc_maxArgs){
			usage();
		}

		long noOfChannels = argc-lc_minArgs;

		const char* g_date = argv[4];
		const char* g_hour = argv[5];

		if(noOfChannels > 0){
			for(long i=0; i < noOfChannels; i++){
				channels[i] = (long) atof(argv[lc_minArgs + i]);
				cout << "Sampling : "<< channels[i] << endl;
			}
		}else{
			channels[0] = 0;
			noOfChannels = 1;
		}

		ifstream l_inputFile(argv[3]);
		// check if successfully opened  Patient data
		if (!l_inputFile.is_open ())
		{
			cout << "Error opening " << argv[3];
			exit (1);
		}else{
			cout << "\nProcessing " << argv[3] << endl;
		}

		ofstream l_outputFile("sample.txt", ios::app);

		char
		l_buffer[MAX_BUFFER_LEN];

		long rowIndex = 0;
		long sampleIndex = 0;

		cout << "-" << g_date << "-" << g_hour << "\n";

		while (!l_inputFile.eof ())
		{
			l_inputFile.getline (l_buffer, MAX_BUFFER_LEN);

			if(rowIndex < FRAME_LENGTH &&
				sampleIndex == 0) {
				// this array will contain channelData a_data
				istringstream input (l_buffer);
				// reading
				char l_date[32], l_hour[32];
				input >> l_date >> l_hour;

				// writing
				if(strcmp(g_date,l_date) == 0 &&
					g_hour[0] == l_hour[0] &&
					g_hour[1] == l_hour[1]){

						for(long i=0; i < NO_MONTAGES; i++){
							input >> channels_data[i];
						}

						for(long i=0; i < noOfChannels; i++){
							l_outputFile << channels_data[channels[i]] << "\t";
						}

						l_outputFile << "\n";
				}// end if date/hour matched sampling
			}// end if with data writing
			if(sampleIndex >= SAMPLE_SKIP){
				sampleIndex = 0;
				rowIndex = 0;
			}else if(rowIndex >=FRAME_LENGTH){
				rowIndex = 0;
				sampleIndex++;
			}else{
				rowIndex++;
			}
		}//end while
		l_inputFile.close();
		l_outputFile.close();
	}

	void Arguments::ProcessGenericArgs(const long& argc,
							char *argv[]){
		const long lc_minArgs = 7;
		if(argc < lc_minArgs){
			usage();
		}

		m_boolDebug = (strcmp(argv[1],"debug")==0)?true:false;

		if(strcmp(argv[2],"Trend") == 0){
			m_operationType = TREND;
		}else if(strcmp(argv[2],"Dump") == 0){
			m_operationType = DUMP;
		}else if(strcmp(argv[2],"Raw") == 0){
			m_operationType = RAW;
		}else if(strcmp(argv[2],"Single") == 0){
			m_operationType = SINGLE;
		}else if(strcmp(argv[2],"Split") == 0){
			m_operationType = SPLIT;
			this->ProcessSplitDirect(argc, argv);
			return;
		}//----------

		m_fileName = argv[3];

		m_decimation = (long) atof(argv[4]);

		m_vFactor = (long)atof(argv[5]);

		m_noOfCols = (long) atof(argv[6]);

		m_noOfIgnores = argc - lc_minArgs;

		if(m_noOfIgnores > 0){
			m_ignores = new long[argc - lc_minArgs];
			for(long i=0; i < m_noOfIgnores; i++){
				m_ignores[i] = (long) atof(argv[lc_minArgs+i]);
				cout << "Ignoring : "<< m_ignores[i] << endl;
			}
		}else{
			m_ignores = new long(-1);
		}

		if(m_operationType == TREND)
		{
			m_noOfsamples =(long)DCD_PERIOD;
		}else{
			m_noOfsamples = 1;
		}
	}

		/*!
		@name RawEEGData
		@brief A Function to clean member resources upon call
		@return
		*/

      RawEEGData::RawEEGData
		  (const Arguments& a_args):m_args(a_args)
	{
		p_rowTags = 0;

		p_inputFile.open (m_args.m_fileName);
		// check if successfully opened  Patient data
		if (!p_inputFile.is_open ())
		{
			cout << "Error opening\t" <<m_args.m_fileName<<"\t";
			exit (1);
		}else{
			cout << "\nProcessing:\t" << m_args.m_fileName << endl;
		}

		m_timestampFile.open ("timestamp.txt", ios::app);
		m_headerFile.open ("header.txt", ios::app);

		// check if successful
		if (	!m_headerFile.is_open () ||
			!m_timestampFile.is_open ())
		{
			cout << "Error creating/opening output file";
			exit (1);
		}
		m_headerFile << "\n";
		m_timestampFile << "\n";
		// Create output file heading
		for (long i = 0;
			 i < NO_MONTAGES; i++)
		{
			const long x = g_montages[i][0];
			const long y = g_montages[i][1];
			// l_skip ignorechannels
			TEST_IGNORE_CHANNEL(x,y)

			m_headerFile
			<< g_channels[x-1].montage << "-"
			<< g_channels[y-1].montage << "\t" ;
		}

		m_headerFile << "\n";
		m_headerFile.flush();

		m_rowCounters = 0;
		m_meanFPE =0.0;
		m_meanOrder = 0.0;
		m_meanLength = 0.0;

		// start timer
		time (&g_startTime);
	}

		/*!
		@name ~RawEEGData
		@brief A Function to clean member resources upon call
		@return
		*/

	RawEEGData::~RawEEGData (void)
	{
	}

	void
	RawEEGData::finalize(void){
		time (&g_endTime);

		m_headerFile << "\n% Time taken in second: "
							<< g_endTime - g_startTime;

		m_headerFile << "\n% Resolution(# Samples): "
							<< m_args.m_noOfsamples;

		m_headerFile << "\n% Max AR Order: " << MAX_ARORDER;

		m_headerFile << "\n% Max Frame Length: " << FRAME_LENGTH;

		m_headerFile << "\n% Max Sample Period: "
							<< SAMPLE_PERIOD;

		m_headerFile << "\n% Decimation rate: "
							<< m_args.m_decimation;

		m_headerFile << "\n% (1:= uV 1000:= mV): "
							<< m_args.m_vFactor ;


		if(m_args.m_operationType == TREND ){

			if (m_rowCounters == 0) m_rowCounters = 1;

			m_headerFile << "\n% mean Ep: "
				<< (m_meanFPE / double(m_rowCounters *
						NO_MONTAGES * m_args.m_noOfsamples)) ;

			m_headerFile << "\n% mean Order: "
				<< (m_meanOrder / double(m_rowCounters *
					NO_MONTAGES * m_args.m_noOfsamples)) <<"\n";

			m_headerFile
			<< "\n% mean Length: "
			<< m_meanLength / (double(m_rowCounters) *
				double(NO_MONTAGES) * double(m_args.m_noOfsamples))<<"\n";
		}

		m_headerFile.flush();
		m_headerFile.close();
		p_inputFile.close ();

		m_timestampFile.flush();
		m_timestampFile.close();
		cout << "\ndone:\n";
		cout.flush();
	}
		/*!
		@name p_WriteDump
		@brief A Function to write used in dumping raw, preprocessed a_data
		@param a_data: is two dimensional array: Raw Data
		@return void
		*/
	void
	RawEEGData::p_WriteDump(const double a_data[NO_MONTAGES][FRAME_LENGTH],
					const char a_marker[FRAME_LENGTH][2][DATA_READ_LEN])
	{
		ofstream outFile("Dump.txt");
		for(long row=0; row < FRAME_LENGTH; row++){
			outFile << a_marker[row][0] << "\t"
							<< a_marker[row][1] << "\t";
			for (long col = 0;
				col < NO_MONTAGES; col++)
			{
				const long x = g_montages[col][0];// source a_montage channel
				const long y = g_montages[col][1]; //  REF a_montage channel
				// l_skip ignorechannels
				TEST_IGNORE_CHANNEL(x,y)
				outFile << a_data[col][row]<< "\t";
			}
			outFile << "\n";
		}
	}

		/*!
		@name p_WriteSingle
		@brief A Function to write used in dumping raw, preprocessed a_data
		@param a_data: is two dimensional array: Raw Data
		@return void
		*/
	void
	RawEEGData::p_WriteSingle(const double a_data[NO_MONTAGES][FRAME_LENGTH],
					const char a_marker[FRAME_LENGTH][2][DATA_READ_LEN])
	{
		ofstream outFile("Single.txt");
		for(long row=0; row < FRAME_LENGTH; row++){
			m_timestampFile << a_marker[row][0] << "\t"
							<< a_marker[row][1] << "\n";
			// execute
			for(long i=0;
				i < NO_MONTAGES;i++)
			{
				const long x = g_montages[i][0];// source a_montage
				const long y = g_montages[i][1]; //  REF a_montage

				// l_skip ignorechannels
				TEST_IGNORE_CHANNEL(x,y)
				outFile << a_data[i][row]<< "\t";

			}
			outFile << "\n";
		}
	}

		/*!
		@name p_Write
		@brief A Function to write cepstrum analysis to text file
		@param a_slope: is two dimensional array holds (mx+c)value
		@param a_avg_FPE: is average final prediction error
		@return void
		*/

	void
	RawEEGData::p_Write(const double a_avgFPE[NO_MONTAGES],
			const double a_order[NO_MONTAGES],
			const double a_length[NO_MONTAGES],
			const char a_marker[FRAME_LENGTH][2][DATA_READ_LEN])
	{
		m_timestampFile << a_marker[0][0] << "\t"
			<< a_marker[0][1] << "\n";

		this->m_rowCounters++;

		// execute
		for(long i=0;
			i < NO_MONTAGES;i++)
		{
			const long x = g_montages[i][0];// source a_montage
			const long y = g_montages[i][1]; //  REF a_montage

			// l_skip ignorechannels
			TEST_IGNORE_CHANNEL(x,y)

			this->m_meanFPE += a_avgFPE[i];
			this->m_meanOrder += a_order[i];
			this->m_meanLength += a_length[i];
		}
		m_timestampFile.flush();
	}

		/*!
		@name p_Read
		@brief A Function to read 2D raw data from text file
		@param a_data: is two dimensional array holds channel data
		@return void
		This functions is constant if it is release version
		otherwise non const and dump sample result of each sub
		process to the output file
		*/
	long
	RawEEGData::p_Read (double a_data[NO_MONTAGES][FRAME_LENGTH],
						char a_marker[FRAME_LENGTH][2][DATA_READ_LEN])
	{
		char
			l_buffer[MAX_BUFFER_LEN];
		long
			index = 0;
		unsigned long long
			l_oldTag = 0;
		bool
			l_jumpCounter = false;
		double
			l_data[REF][FRAME_LENGTH];

		while (!p_inputFile.eof () && index < FRAME_LENGTH)
		{
			p_inputFile.getline (l_buffer, MAX_BUFFER_LEN);

			// make sure first character is % for a comment
			if (l_buffer[0] != '%')
			{
				// This line ensures 4 sec sampling by toggling jump counter
				if(m_args.m_decimation == 2 && l_jumpCounter == true){
						l_jumpCounter = false;
						continue;
				}else{
					l_jumpCounter = true;
				}
				// this array will contain channelData a_data
				istringstream input (l_buffer);
				// formatted row marker from text l_buffer
				input >> a_marker[index][0] >> a_marker[index][1] >> p_rowTags;
				// now read consecutive 20 channelData
				for (long i = 1; i <= m_args.m_noOfCols; i++)
				{
					char
						l_dataBuffer[DATA_READ_LEN];
					input >> l_dataBuffer;
					TEST_IGNORE_CHANNEL(i,i) // redundant but simple
					if (strcmp (l_dataBuffer, "AMPSAT") == 0
					    || strcmp (l_dataBuffer, "SHORT") == 0
					    || strcmp (l_dataBuffer, "OFF") == 0)
					{
						index = -1;
						break;	// drop the line
					}
					l_data[i - 1][index] =
						double(atof (l_dataBuffer)) * this->m_args.m_vFactor;
					// may need math.h in windows
				}	// end for capturing 20 channel a_data

			}	//endif
			else{
				if(m_args.m_boolDebug == true){
					cout << l_buffer << "\n";
				}
				continue;
			}
			if ((index > 0 && p_rowTags == (l_oldTag + m_args.m_decimation)) || index == 0)
			{
				l_oldTag = p_rowTags;	// continue to read
			}
			else
			{
				index = 0;
				l_jumpCounter = false;
				continue;	// drop the frame
			}

			++index;

		}		//end while and FRAME_LENGTH
		// set the number of item read in this pass
		if(index < FRAME_LENGTH) return index; // terminate program
		this->p_ProcessMontages(a_data,l_data);
		return index;
	}

	inline void
	RawEEGData::p_DebugWrite(const double *a_data,
				const char* a_fileName,
				const long& a_length,
				const int& mode)const
	{
		ofstream m_dbgFile;
		if(mode==0)
			m_dbgFile.open(a_fileName);
		else
			m_dbgFile.open(a_fileName, std::ios::ate);

		if (!m_dbgFile.is_open ())
		{
			cout << "\nError opening\t" << a_fileName << "\n";
			return;
		}
		for (long x = 0; x < a_length; x++)
			m_dbgFile << a_data[x] << "\t";
		m_dbgFile << "\n";
		m_dbgFile.flush();
		m_dbgFile.close();
	}

		/*!
		@name p_PreProcess
		@brief A Function to preprocess raw data
		@param a_data: is two dimensional array holds channel data
		@return void
		This functions is constant if it is release version
		otherwise non const and dump sample result of each sub
		process to the output file
		*/

	void
	RawEEGData::p_PreProcess(double a_data[FRAME_LENGTH])const
	{
		// This frame is ready for further processing
		INIT_DEBUG_PRINT()

		EXEC_DEBUG_PRINT(a_data, FRAME_LENGTH, RAW_TXT)

		this->p_DcRemove (a_data);

		EXEC_DEBUG_PRINT(a_data, FRAME_LENGTH, DC_TXT)

		this->p_RemoveBLW (a_data);

		EXEC_DEBUG_PRINT(a_data, FRAME_LENGTH, BLW_TXT)

		const long a_noelements = MA_POINTS;
		this->p_MAFilter (a_data, a_noelements );

		EXEC_DEBUG_PRINT(a_data, FRAME_LENGTH, MA_TXT)

		for(int i=0;i<FRAME_LENGTH;i+=BASE_FREQ)
			this->
			p_Filter(&a_data[i],BASE_FREQ, m_A, m_B, m_FilterLength);

		EXEC_DEBUG_PRINT(a_data, FRAME_LENGTH, BUTTER_TXT)

		for(int i=0;i<FRAME_LENGTH;i+=BASE_FREQ)
			this->
			p_HanningCorrection(&a_data[i], BASE_FREQ);

		EXEC_DEBUG_PRINT(a_data, FRAME_LENGTH,HANNCOREC_TXT)

		for(int i=0;i<FRAME_LENGTH;i+=BASE_FREQ)
			this->p_FilterByFFT(&a_data[i]);

		EXEC_DEBUG_PRINT(a_data, FRAME_LENGTH, FINAL_TXT)

		FINALIZE_DEBUG_PRINT()
	}// end preprocess

	void
	RawEEGData::p_ProcessMontages(double a_montage[NO_MONTAGES][FRAME_LENGTH],
					double a_data[REF][FRAME_LENGTH])const
	{
		// Create Montages
		for(long i=0;
			i < NO_MONTAGES;i++)
		{
			const long x = g_montages[i][0]-1;// source a_montage
			const long y = g_montages[i][1]-1;//  REF a_montage

			for(long j=0;j< FRAME_LENGTH; j++)
			{
				const double ref = a_data[y][j];
				a_montage[i][j] = a_data[x][j] - ref;
			}

			INIT_DEBUG_PRINT()
			EXEC_DEBUG_PRINT(a_data[x], FRAME_LENGTH, X_TXT)
			EXEC_DEBUG_PRINT(a_data[y], FRAME_LENGTH, Y_TXT)
			FINALIZE_DEBUG_PRINT()

		}// end for loop
	}// end p_ProcessMontages


	/*!
	@name Process
	@brief A Function to process Cepstrum analysis
	@param void
	@return void
	*/
void
RawEEGData::Process(void)
{
	double x_cord[DCD_PERIOD];
	// cepstrum for each frame
	double ceps[NO_MONTAGES][DCD_PERIOD];
	double spec[NO_MONTAGES][DCD_PERIOD];
	double ra[NO_MONTAGES][DCD_PERIOD];
	double sfr[NO_MONTAGES][DCD_PERIOD];
	double lstOrder[NO_MONTAGES][DCD_PERIOD];
	double lstLength[NO_MONTAGES][DCD_PERIOD];

	// consecutive input a_data
	double l_data[NO_MONTAGES][FRAME_LENGTH];
	char  l_marker[FRAME_LENGTH][2][DATA_READ_LEN];

	long
		second = 0;
	while (!p_inputFile.eof () &&
			this->p_Read (l_data, l_marker) == FRAME_LENGTH)
	{
		double l_arorder[NO_MONTAGES];
		double l_avgFPE[NO_MONTAGES];
		double l_length[NO_MONTAGES];

		if(second == 0){
			memset(l_arorder,0,sizeof(l_arorder));
			memset(l_avgFPE,0,sizeof(l_avgFPE));
			memset(l_length,0,sizeof(l_length));
		}//end if

		for(long i=0;
			i < NO_MONTAGES;i++)
		{
			const long x = g_montages[i][0];// source a_montage
			const long y = g_montages[i][1]; //  REF a_montage

			// l_skip ignorechannels
			TEST_IGNORE_CHANNEL(x,y)
			this->p_PreProcess(l_data[i]);
			// Now do the analysis
			double 	Ao[MAX_ARORDER + 1];
			double perform=0.0;
			double l_Ep = 0.0;
			double l_wgn[FRAME_LENGTH];
			long order = 0;
			double length=0.0;

			ceps[i][second] =
			this->
			p_SumOfCepstrums(l_data[i],
					l_wgn,
					Ao,
					l_Ep,
					length,
					order,
					perform);

			l_avgFPE[i] += perform;
			l_arorder[i] += order;
			l_length[i] += length;

			// Provides both RA and spectrum
			spec[i][second] =
			this->
			p_SumOfSpectrum(Ao,
					l_Ep,
					length,
					order,
					ra[i][second]);

			const long wgn_length = long((length / SAMPLE_PERIOD) *
					FRAME_LENGTH);
			sfr[i][second] =
			this->
			p_wgnSFR(l_wgn,
				wgn_length);

			lstOrder[i][second] = order;
			lstLength[i][second]= length;

			INIT_DEBUG_PRINT()
			EXEC_DEBUG_PRINT(l_wgn,wgn_length,RESIDUALX_TXT)
			FINALIZE_DEBUG_PRINT()
		}

		const double periodOffset = double(SAMPLE_PERIOD)/2;

		if (second == (m_args.m_noOfsamples -1) )
		{
			// initialized first element
			x_cord[0] = periodOffset;
			// Results
			double l_slopeCeps[NO_MONTAGES];
			double l_slopeSpec[NO_MONTAGES];
			double l_slopeRA[NO_MONTAGES];
			double l_slopeSFR[NO_MONTAGES];
			double l_slopeOrder[NO_MONTAGES];
			double l_slopeLength[NO_MONTAGES];

			INIT_DEBUG_PRINT()
			EXEC_DEBUG_PRINT(x_cord, m_args.m_noOfsamples,
							XINTERPOLATE_TXT)

			for (long i = 0;
					i < NO_MONTAGES; i++)
			{
				const long x = g_montages[i][0];// source a_montage
				const long y = g_montages[i][1]; //  REF a_montage

				// l_skip ignorechannels
				TEST_IGNORE_CHANNEL(x,y)
				EXEC_DEBUG_PRINT(ra[i], m_args.m_noOfsamples, ALPHA_TXT)
				EXEC_DEBUG_PRINT(spec[i], m_args.m_noOfsamples, SPEC_TXT)
				EXEC_DEBUG_PRINT(ceps[i], m_args.m_noOfsamples, CEPS_TXT)
				EXEC_DEBUG_PRINT(sfr[i], m_args.m_noOfsamples, SFR_TXT)
				EXEC_DEBUG_PRINT(lstOrder[i], m_args.m_noOfsamples, ORDERS_TXT)
				EXEC_DEBUG_PRINT(lstLength[i], m_args.m_noOfsamples, LENGTHS_TXT)
				FINALIZE_DEBUG_PRINT()

				double l_result[2] = { 0.0, 0.0 };
				// Next RA
				INTERPOLATE_FEATURE(ra[i], l_slopeRA[i])
				// Next ceps
				INTERPOLATE_FEATURE(ceps[i], l_slopeCeps[i])
				// Next Spec
				INTERPOLATE_FEATURE(spec[i], l_slopeSpec[i])
				// Next SFR
				INTERPOLATE_FEATURE(sfr[i], l_slopeSFR[i])
				// Next orders
				INTERPOLATE_FEATURE(lstOrder[i], l_slopeOrder[i])
				// Next lengths
				INTERPOLATE_FEATURE(lstLength[i], l_slopeLength[i])
			}// end for
			// now write them to text fil
			EXEC_PRINT(l_slopeCeps, NO_MONTAGES, CEPSOUT_TXT)
			EXEC_PRINT(l_slopeSpec, NO_MONTAGES, SPECOUT_TXT)
			EXEC_PRINT(l_slopeRA, NO_MONTAGES, ALPHAOUT_TXT)
			EXEC_PRINT(l_slopeSFR, NO_MONTAGES, SFROUT_TXT)
			EXEC_PRINT(l_slopeOrder, NO_MONTAGES, ORDERSOUT_TXT)
			EXEC_PRINT(l_slopeLength, NO_MONTAGES, LENGTHSOUT_TXT)

			this->p_Write(l_avgFPE, l_arorder, l_length, l_marker);
			second = 0;
		} // if
		else
		{// This code ensures proper timing
			// axis - X axis
			x_cord[second] = (double)p_rowTags;
			if (second > 0)
			{
				x_cord[second] -= x_cord[0];
				x_cord[second] = (double)floor(x_cord[second] / double(FRAME_LENGTH)
							* double(SAMPLE_PERIOD) + periodOffset);
			}
			second++;
		} // else
	}// end while
}// end process function


		/*!
		@name ProcessDump
		@brief A Function to dump preprocess data no analysis
		@param void
		@return void
		*/

	void
	RawEEGData::ProcessDump(void)
	{
		// consecutive input a_data
		double l_data[NO_MONTAGES][FRAME_LENGTH];
		char  l_marker[FRAME_LENGTH][2][DATA_READ_LEN];
		while (!p_inputFile.eof () &&
				this->p_Read (l_data, l_marker) == FRAME_LENGTH)
		{
			// execute
			for(long i=0;
				i < NO_MONTAGES;i++)
			{
				const long x = g_montages[i][0];// source a_montage
				const long y = g_montages[i][1]; //  REF a_montage

				// l_skip ignorechannels
				TEST_IGNORE_CHANNEL(x,y)
				this->p_PreProcess(l_data[i]);
			}
			this->p_WriteDump(l_data, l_marker);
		}
	}

		/*!
		@name ProcessSingle
		@brief A Function to dump preprocess data no analysis
		@param void
		@return void
		*/

	void
	RawEEGData::ProcessSingle(void)
	{
		// consecutive input a_data
		double l_data[REF][FRAME_LENGTH];
		char  l_marker[FRAME_LENGTH][2][DATA_READ_LEN];
		while (!p_inputFile.eof () &&
				this->p_Read (l_data, l_marker) == FRAME_LENGTH)
		{
			// execute
			for(long i=0;
				i < NO_MONTAGES;i++)
			{
				const long x = g_montages[i][0];// source a_montage
				const long y = g_montages[i][1]; //  REF a_montage

				// l_skip ignorechannels
				TEST_IGNORE_CHANNEL(x,y)
				this->p_PreProcess(l_data[i]);
			}

			this->p_WriteSingle(l_data, l_marker);
		}
	}
		/*!
		@name ProcessRaw
		@brief A Function to dump unprocessed data no analysis
		@param void
		@return void
		*/

	void
	RawEEGData::ProcessRaw(void)
	{
		// consecutive input a_data
		double l_data[NO_MONTAGES][FRAME_LENGTH];
		char  l_marker[FRAME_LENGTH][2][DATA_READ_LEN];
		while (!p_inputFile.eof () &&
			this->p_Read (l_data, l_marker) == FRAME_LENGTH)
		{
			this->p_WriteDump(l_data, l_marker);
		}
	}


	void
	RawEEGData::p_DcRemove (double a_data[FRAME_LENGTH]) const
	{
		double	sum = 0.0;

		for (long index = 0;index < FRAME_LENGTH; index++)
			sum += a_data[index];

		const double	average = sum / double(FRAME_LENGTH);
		// correct the dc value in the frame
		for (long index = 0;index < FRAME_LENGTH; index++)
			a_data[index] -= average;

	}			// end function dcRemove

	
	void
	RawEEGData::p_PolyVal (const double a_X[],
					double a_Y[],
				     	const long& a_length,
					const long &a_order,
				     	const double a_result[]) const
	{
		for (long i = 0; i < a_length; i++)
		{
			a_Y[i] = 0;
   double
				p = 1;
			for (long ord = 0; ord < a_order + 1; ord++)
			{
				a_Y[i] += a_result[ord] * p;
				p    *=	a_X[i];
			}
		}
	}//end function


	void
	RawEEGData::p_PolyFit (const double a_X[],
				   const double a_Y[],
				   const long &a_length,
				   const long &a_order,
				   double a_result[])const
	{
		if (a_order > MAX_ORDER)
		{
			cout << "\nMaximum Order: " << MAX_ORDER;
			exit (1);
		}

		double	elements[2 * MAX_ORDER + 1];
		memset (elements, 0, sizeof (elements));

		elements[0] = (double)a_length;

		double	a_A[MAX_ORDER + 1][MAX_ORDER + 2];
		memset (a_A, 0, sizeof (a_A));
		memset(a_result, 0 , sizeof(a_result));

		//  create column vector and x_(K-1)
		for (long k = 0; k < a_length; k++)
		{
			const double &
				yy = a_Y[k];
			const double &
				xx = a_X[k];
			double
				p = 1;	// col vector

			for (long r = 0; r < a_order + 1; r++)
			{
				// a_B[0] = a_B[0]+Y1* 1, a_B[1] = a_B[1] + Y1 * a_X[1]
				a_A[r][a_order + 1] += yy * p;
				// a_B[2] = a_B[2] + a_Y[1]*a_X[1]*a_X[1]
				p = p * xx;
			}

			//Compute the sum of powers of x_(K-1)
			p = xx;
			for (long i = 1; i < 2 * a_order + 1; i++)
			{
				elements[i] += p;
				p = p * xx;
			}
		}

		//Determine the matrix entries

		for (long row = 0; row < a_order + 1; row++)
			for (long col = 0; col < a_order + 1; col++)
				a_A[row][col] = elements[row + col];

		//  Solve the linear system of M_ORDER + 1 equations

		long
			Row[MAX_ORDER + 1];	// Field with row-number

		//Initialize the polonger vector

		for (long i = 0; i < a_order + 1; i++)
			Row[i] = i;

		// Start LU factorization

		for (long p = 0; p < a_order + 1; p++)
		{
			// Find pivot element

			for (long k = p + 1; k < a_order + 1; k++)
			{
				if (fabs (a_A[Row[k]][p]) > fabs (a_A[Row[p]][p]))
				{
					long
						t = Row[p];
					Row[p] = Row[k];
					Row[k] = t;
				}
			}	// simulate row xchange

			if (a_A[Row[p]][p] == 0)
			{
				cout << "\nA[" << Row[p] << "][" << p
					 << "]:\t" << a_A[Row[p]][p]
					 << "\nThe matrix is SINGULAR !";
				return;
			}
			// Form multiplier

			for (long k = p + 1; k < a_order + 1; k++)
			{
				// scale the ith column of the matrix
				// a_A[Row[p][p] is the pivot element
				a_A[Row[k]][p] = a_A[Row[k]][p] / a_A[Row[p]][p];

				// Eliminate
				for (long c = p + 1; c < a_order + 2; c++)
				{
					a_A[Row[k]][c] -=
						a_A[Row[k]][p] * a_A[Row[p]][c];
				}
//         a_A[Row[K]][P] = 0;
			}
		}		// End of  L*U  factorization routine
// now need forward substitution
		for (long i = 0; i < a_order + 1; i++)
		{
			for (long j = i + 1; j < a_order + 2; j++)
			{
				a_A[Row[i]][j] /= a_A[Row[i]][i];
			}
//              a_A[Row[i]][i] = 1;
		}

// now back substitution
		long
			factorCol = 0;
		for (long k = a_order; k >= 0; k--)
		{
			const double &
				pivot = a_A[Row[k]][a_order + 1];

			factorCol++;
			for (long j = a_order - factorCol; j >= 0; j--)
			{
				const double &
					a_mvFactor =
					a_A[Row[j]][a_order + 1 - factorCol];
				for (long col = 0; col < factorCol; col++)
				{
					a_A[Row[j]][a_order + 1 - col] -=
						a_mvFactor * pivot;
				}
			}
		}

		for (long i = 0; i < a_order + 1; i++)
		{
			a_result[i] = a_A[Row[i]][a_order + 1];
		}
	}// end function


	void
	RawEEGData::p_RemoveBLW (double a_data[FRAME_LENGTH])const
	{
		double*	x_cord = new double[FRAME_LENGTH];
		const double offset = 0.5/double(FRAME_LENGTH);
		for (long i = 0; i < FRAME_LENGTH; i++){
			x_cord[i] =
			(double(i)/FRAME_LENGTH)*double(SAMPLE_PERIOD) + offset;
		}

		INIT_DEBUG_PRINT()
		EXEC_DEBUG_PRINT(x_cord, FRAME_LENGTH, "Xblw.txt")
		FINALIZE_DEBUG_PRINT()

		double	a_result[MAX_ORDER + 1];
		p_PolyFit (x_cord, a_data, FRAME_LENGTH,
			       POLYNOMIAL_ORDER, a_result);

		double*	correctedChannelData = new double[FRAME_LENGTH];
		p_PolyVal (x_cord, correctedChannelData,FRAME_LENGTH,
				 POLYNOMIAL_ORDER, a_result);

		// correct the low freq noise
		for (long index = 0; index < FRAME_LENGTH; index++)
			a_data[index] -=
				correctedChannelData[index];

		delete[] correctedChannelData; correctedChannelData = NULL;
		delete[] x_cord; x_cord = NULL;
	}//end function

	void
	RawEEGData::p_HanningCorrection(double a_data[FRAME_LENGTH],
					const long int& a_length) const
	{
		// Removing edges with 5% hanning taper
		long hann_len = a_length/10+1;
		for(long x = 1;x < long(hann_len/2);x++){
			double hann_coef = 0.5*(1 - cos(2*M_PI*x/hann_len));
			a_data[x-1] = a_data[x-1] * hann_coef;
			a_data[FRAME_LENGTH-x] =
				a_data[FRAME_LENGTH-x] * hann_coef;
		}
	}

	/*!
	@name p_FilterByFFT
	@brief Search and look for its use ! :)
	@param a_data 1D array upon which function will operate
	@return void
	This function uses FFT to remove Low and High Frequency
	*/
	void
	RawEEGData::p_FilterByFFT(double a_data[BASE_FREQ])const
	{
		FILE *wisdom;
		static fftw_plan p1, p2;
		static fftw_complex h[BASE_FREQ], H[BASE_FREQ];

		/* Keep the arrays and plans around from last time, since this
		is a very common case. Reallocate them if they change. */
		INIT_DEBUG_PRINT()
		if (firstTime) {
			/* Get any accumulated wisdom. */
			wisdom = fopen("wisfile", "r");
			if (wisdom) {
				fftw_import_wisdom_from_file(wisdom);
				fclose(wisdom);
			}

			/* Set up the fftw plans. */
			p1 = fftw_plan_dft_1d(BASE_FREQ, h, H,
								FFTW_FORWARD,FFTW_MEASURE );
			p2 = fftw_plan_dft_1d(BASE_FREQ, H, h,
								FFTW_BACKWARD, FFTW_MEASURE);
			/* Save the wisdom. */
			wisdom = fopen("wisfile", "w");
			fftw_export_wisdom_to_file(wisdom);
			fclose(wisdom);
		}
		// do not need to recreate the plan
		FINALIZE_DEBUG_PRINT()

		/* Convert the input to complex. Also compute the mean. */
		for (long x = 0; x < BASE_FREQ; x++) {
			h[x][0] = a_data[x];
			h[x][1] = 0.0;
		}
		/* FFT. */
		fftw_execute(p1);

		// Filter high & Very lowFrequency component
		for (long x = 0; x < 2; x++) {
			H[x][0] = 0.0;
			H[x][1] = 0.0;
		}

		for (long x = 32; x < BASE_FREQ; x++) {
			H[x][0] = 0.0;
			H[x][1] = 0.0;
		}

		fftw_execute(p2);

		/* Convert the output to real. */
		for(long j=0;j<BASE_FREQ;j++){
			a_data[j] = h[j][0]/BASE_FREQ;
		}
	}

#define AIC_FUNC(var, order, length) (length*log(double(var))+2*order)
#define FPE_FUNC(var, order, length) fabs(((1+ order/length)/(1-order/length))*var)
#define MDL_FUNC(var, order, length) fabs(var*log(double(order+1)) + var*log((double)length))


	long
	RawEEGData::p_GetOptimumAR(const double a_data[FRAME_LENGTH],
					double a_wgn[FRAME_LENGTH],
					double* a_AR,
					double& a_Ep,
					long& a_order,
					double& a_perform)const
	{
		// This is performance criteria, i.e. Variance
		double 	Ao[MAX_ARORDER + 1];

		a_order =
		this->p_ArBurg(a_data, a_wgn, Ao, a_perform, a_Ep, FRAME_LENGTH);

		long last_length = FRAME_LENGTH;

		double last_MDL = MDL_FUNC(1,a_order,last_length); //it is always 1
		const double init_Ep = a_Ep;

		const double DELTA_LENGTH = 10;
		const double MINIMUM_LENGTH = 900;
		// Calculate Optimum AR parameters
		const long limit_test = long(FRAME_LENGTH / DELTA_LENGTH);

		const long start_polong = long(MINIMUM_LENGTH / DELTA_LENGTH);

		for(long i = limit_test - 1;
				i > start_polong; i--)
		{
			// Store the last known good values
			for(long j=0; j < MAX_ARORDER; j++){
				a_AR[j] = Ao[j];
			}

			const long a_length = long(i * DELTA_LENGTH);

			a_order = this->p_ArBurg(a_data, a_wgn, Ao, a_perform, a_Ep, a_length);

			const double perf = (a_length / double(FRAME_LENGTH))*(a_Ep/init_Ep);

			double cur_MDL = MDL_FUNC(perf, a_order, a_length);

			if(cur_MDL > last_MDL) break;

			last_MDL = cur_MDL;

			last_length = a_length;
		}
		return last_length;
	}

	double
	RawEEGData::p_SumOfSpectrum(const double* a_AR,
					const double& a_Ep,
					const double& a_length,
					const long& a_order,
					double& a_frameRA)const

	{
		const double MAX_FREQ = .128;
		const double MIN_FREQ = .008;
		const double DELTA_FREQ = .002;
		const long l_power_length =
			long((MAX_FREQ - MIN_FREQ)/DELTA_FREQ);
		double l_sumFreq=0.0;

		// 1/250, 20/250
//		const double RA_BASE_MIN = .004;
//		const double RA_BASE_MAX = .08;
//		double l_ra_base = 0.0;
		// 6/250, 14/250
		const double RA_MIN_FREQ = .024;
		const double RA_MAX_FREQ = .056;
		const long l_ra_length =
			long((RA_MAX_FREQ - RA_MIN_FREQ)/DELTA_FREQ);
		double l_ra = 0.0;

		double* l_power = new double[l_power_length];
		double* l_freq = new double[l_power_length];

		long index = 0;
		double gmean_F = 0;
		double l_ra_gmean = 0;

		for(double x = MIN_FREQ; x < MAX_FREQ; x+=DELTA_FREQ){
			double l_real = 1;
			double l_imaginary = 0;

			for (long i = 1; i <= a_order; i++)
			{
				l_real += a_AR[i] * cos(x * i * a_length);
				l_imaginary += a_AR[i] * sin(x* i * a_length);
			}
			complex<double> l_cmplx_pwr(l_real,l_imaginary);

			// Normalized power spectrum amplitude
			l_cmplx_pwr = (a_Ep * a_length)/(l_cmplx_pwr * l_cmplx_pwr);

			const double curPower = abs(l_cmplx_pwr);

			const double logPower = log(curPower);

			l_power[index] = 10*log10(curPower);
			l_freq[index++] = x;

			l_sumFreq += curPower;

			if(x >= RA_MIN_FREQ && x <= RA_MAX_FREQ) {
				l_ra += curPower;
				l_ra_gmean += logPower;
			}
			l_sumFreq += curPower;
			gmean_F += log(curPower);
		}
		l_sumFreq /= l_power_length;
		gmean_F /= l_power_length;
		l_ra /= l_ra_length;
		l_ra_gmean /= l_ra_length;

		INIT_DEBUG_PRINT()
		EXEC_DEBUG_PRINT(l_power,l_power_length,"Pf.txt")
		EXEC_DEBUG_PRINT(l_freq,l_power_length,"freq.txt")
		FINALIZE_DEBUG_PRINT()

		delete[] l_power;l_power=NULL;
		delete[] l_freq; l_freq = NULL;
		// dont use the ratio
		a_frameRA = ( 10*log10(exp(l_ra_gmean)/l_ra));
		return ( 10*log10(exp(gmean_F)/l_sumFreq));
	}

	double
	RawEEGData::p_wgnSFR(double a_data[FRAME_LENGTH],
				const long& a_length)const
	{
		FILE *wisdom;
		static fftw_plan p1;
		static fftw_complex h[FRAME_LENGTH], H[FRAME_LENGTH];

		/* Keep the arrays and plans around from last time, since this
		is a very common case. Reallocate them if they change. */
		INIT_DEBUG_PRINT()
		if (firstTime) {
			/* Get any accumulated wisdom. */
			wisdom = fopen("wisfile", "r");
			if (wisdom) {
				fftw_import_wisdom_from_file(wisdom);
				fclose(wisdom);
			}

			/* Set up the fftw plans. */
			p1 = fftw_plan_dft_1d(FRAME_LENGTH, h, H,
								FFTW_FORWARD,FFTW_MEASURE );
			/* Save the wisdom. */
			wisdom = fopen("wisfile", "w");
			fftw_export_wisdom_to_file(wisdom);
			fclose(wisdom);
		}
		// do not need to recreate the plan
		FINALIZE_DEBUG_PRINT()

		/* Convert the input to complex. Also compute the mean. */
		for (long x = 0; x < FRAME_LENGTH; x++) {
			if(x < a_length){
				h[x][0] = a_data[x];
				h[x][1] = 0.0;
			}else{
				h[x][0] = 0.0;
				h[x][1] = 0.0;
			}
		}

		/* FFT. */
		fftw_execute(p1);

		double l_sumFreq = 0.0;
		double gmean_F = 0.0;
		for (long x = 0; x < FRAME_LENGTH; x++) {
			complex<double> l_cmplx_pwr(H[x][0],H[x][1]);

			const double curPower = abs(l_cmplx_pwr);

			l_sumFreq += curPower;
			gmean_F += log(curPower);
		}
		l_sumFreq /= FRAME_LENGTH;
		gmean_F /= FRAME_LENGTH;
		return( 20*log10(exp(gmean_F)/l_sumFreq));
	}

 double
	RawEEGData::p_SumOfCepstrums (const double a_data[FRAME_LENGTH],
					double a_wgn[FRAME_LENGTH],
					double* a_AR,
					double& a_Ep,
					double& a_length,
					long& a_order,
					double& a_perform)const
	{
		// This is suppose to be long
		a_length
		= double(this->
		p_GetOptimumAR(a_data, a_wgn, a_AR, a_Ep, a_order, a_perform))
		*double(SAMPLE_PERIOD)/double(FRAME_LENGTH);

	// Now Calculates Cepstrum coefficients upto LowestOrder+1
		INIT_DEBUG_PRINT()

		a_AR[0] = a_length;

		EXEC_DEBUG_PRINT(a_AR, long(a_order+1), "Ao.txt")

		a_AR[0] = 1;

		double	l_Cep[MAX_ARORDER + 2];	// leave it as is

		double a_result = p_ArCeps(a_AR, l_Cep, a_order+2);

		EXEC_DEBUG_PRINT(l_Cep, long(a_order+2), "arceps.txt")

		FINALIZE_DEBUG_PRINT()

		return a_result;
	}// end function

	// This code calculates Ar Coefficients
	// MAX_ARORDER is a const set to 12
	// fpeData is tha variance
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
		const double init_Eo = Eo;
		long	lowestOrder = MAX_ARORDER;

		// Monotonically punished by Order
		double last_fpe_criteria = AIC_FUNC(1,0,a_length); // Ep/MAX_EP
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
			cur_fpe_criteria = AIC_FUNC((Eo/init_Eo), m, a_length);

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
		Bo[0] = 1;
		this->p_Filter(a_wgn, a_length, Bo,a_Ar,lowestOrder); // inverse filtering

		this->p_HanningCorrection(a_wgn, a_length);

		a_fpeData = a_Ep;
		delete[] Fe; Fe = NULL;
		delete[] Be; Be = NULL;
		return lowestOrder;
	} //  end function p_ArBurg

	void
	CFilter::p_Conv(const double* a_data1, 
					const long& length1,
					const double* a_data2,
					const long& length2,
					double* a_result)//must be length1+length2-1
	{
		const long convLength = length1+length2-1;
		double* a_A=new double[convLength];
		double* a_B=new double[convLength];
		for(long x=0;x<convLength;x++){
			a_A[x]=0;
			if(x < length1){
				a_B[x]=a_data1[x];
				a_result[x] = a_data2[x];
			}else if(x < length2){
				a_B[x]=0;
				a_result[x] = a_data2[x];
			}else{
				a_B[x]=0;
				a_result[x] = 0;
			}
		}
		a_A[0]=1;
		this->p_Filter(a_result,convLength,a_A,a_B,convLength);
		delete[] a_A;
		delete[] a_B;
	}

	void
	CFilter::p_Chebyshev(double* a_data, // return value
				const long& order, // 10
				const double& epsilon, // 0.5
				const long& Fc, // cutoff frequency: 125;
				const long& a_FilterLength)
	{
	//STEP: Create a Chebyshev filter function in freq domain
		const double epsilon_sq=epsilon*epsilon; //0.5
		double *c = new double[a_FilterLength];
		double *c1= new double[a_FilterLength];
		double *c2= new double[a_FilterLength];
		// initialize
		for(long f=0;f<=a_FilterLength/2;f++){
			c1[f]=1;
			c2[f]= double(f)/Fc;
		}
		for(int j=2;j < order;j++){
			for(long f=0;f<=a_FilterLength/2;f++){
				c[f] = 2* (double(f)/Fc)*c2[f] - c1[f];
				c1[f] = c2[f];
				c2[f] = c[f];
			}
		}
		fftw_complex* H = new fftw_complex[a_FilterLength];// filter response
		for(long f=0;f<=a_FilterLength/2;f++){
			const double Fx = (f/Fc)*(f/Fc)*c[f]*c[f];
			H[f][0] = 1/sqrt(1+epsilon_sq*Fx);
			H[f][1] = 0.0; // imaginary part
		}
		for(long f=499;f>250;f--){
			H[f][0] = H[500-f][0]; // Symmetric
			H[f][1] = 0.0; // imaginary part
		}
	//STEP: use fftw to convert it into time domain
		fftw_plan inverse;
		fftw_complex* h = new fftw_complex[a_FilterLength];

		/* Set up the fftw plans. */
		inverse = fftw_plan_dft_1d(a_FilterLength, H, h,
							FFTW_BACKWARD, FFTW_ESTIMATE);
		/* IFFT. */
		fftw_execute(inverse);

		/* Convert the output to real. and return */
		for (long x = 0; x < 500; x++) {
			a_data[x] = h[x][0]/a_FilterLength; // using only the real one
		}
		delete[] c;
		delete[] c1;
		delete[] c2;
	}

	/*
	* This filter behaves like Matlab MA Filter
	*/
	void
	CFilter::p_MAFilter (double* a_data,
				const long& dataLen,// must be fourier length
				const long& a_noelements) const
	{
		// The FIFO that will hold the a_data for MA Filter @ 50 Hz
		double* l_A = new double[a_noelements];
		double* l_B = new double[a_noelements];
		l_A[0]=1.0;
		l_B[0]=1.0/a_noelements;

		for(long i=1; i < a_noelements; i++){
			l_B[i] = l_B[0];
			l_A[i] = 0.0;
		}
		// Now filter
		this->
		p_Filter(a_data, dataLen, l_A, l_B, a_noelements);

		delete[] l_A; l_A = NULL;
		delete[] l_B; l_B = NULL;
	}//end function

	// Filter constants a_A and a_B must be of same a_length
	void
	CFilter::p_Filter(double* a_data,
				const long& len,
				const double* a_A,
				const double* a_B,
				const long& a_FilterLength)
	{
		double* l_X=new double[a_FilterLength];
		double* l_Y=new double[a_FilterLength];

		// initialize the circular FIFO buffers
		for(long i=0; i < a_FilterLength;i++){
			l_X[i] = 0.0;
			l_Y[i] = 0.0;
		}

		long b_index = 0;// b vector index
		long index = 0; // temp index
		double l_num = 0.0;
		for(long x = 0; x < len;x++){
			// Update current element in X(j) vector
			l_X[b_index] = a_data[x]; 
			// temporary index based on current b_index
			// that will be used to loop through coefficients and X(j)
			// Y(i) vector
			index = b_index; 
			// becuase in difference equation B(j) starts with zero
			l_num = a_B[0]*l_X[index];
			// wrap around index to implement circular buffer
			if(--index < 0)
				index = a_FilterLength -1;
			// using difference equation
		 	for(long i=1; i < a_FilterLength; i++){
				l_num += (a_B[i]*l_X[index] - a_A[i]*l_Y[index]);
				// decrement index and wrap around again
				if(--index < 0)
					index = a_FilterLength -1;
			}
			// Now update current element in Y(i) vector
			l_Y[b_index] = l_num;
			// Store the filtered data
			a_data[x] = l_Y[b_index];
			// current element index in the X(j) and Y(i) need to be incremented
			// and wrap around 
			if(++b_index == a_FilterLength)
				b_index = 0;
		} //  end processing filtering
		delete[] l_X;l_X=NULL;
		delete[] l_Y;l_Y=NULL;
	}//end function

	double
	RawEEGData::p_ArCeps(const double a_Ar[MAX_ARORDER+1],
				double a_ceps[MAX_ARORDER+2],
				const long& a_order) const
	{
		a_ceps[0] = 0.0;

		double l_Sumc = fabs(a_ceps[0]);

		for (long i = 1; i <= a_order; i++)
		{
			a_ceps[i] = a_Ar[i];
			for (long k = 1; k < i; k++)
			{
				a_ceps[i] +=
				double(k) / double(i) * a_ceps[k] * a_Ar[i-k];
			}
			a_ceps[i] *= -1;
			l_Sumc += fabs (a_ceps[i]);
		}
		return l_Sumc;
	}
}// end namespace
