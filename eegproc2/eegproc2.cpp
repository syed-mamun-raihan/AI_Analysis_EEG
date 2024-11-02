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
#include <cstring>
#include <sstream>
#include <cmath>
#include <memory>
#include <ctime>
#include <complex>
#include <vector>
#include <typeinfo>
#include "fftw3.h"
//#include <function>

#include "eegproc2.h"

#if defined(WIN32)

static void my_fftw_write_char ( char c, void *f ) 
        { fputc ( c, ( FILE * ) f );}
    #define fftw_export_wisdom_to_file(f) \
            fftw_export_wisdom(my_fftw_write_char, (void*) (f))
    #define fftwf_export_wisdom_to_file(f) \
            fftwf_export_wisdom(my_fftw_write_char, (void*) (f))
    #define fftwl_export_wisdom_to_file(f) \
            fftwl_export_wisdom(my_fftw_write_char, (void*) (f))

static int my_fftw_read_char ( void *f ) { return fgetc ( ( FILE * ) f );}
    #define fftw_import_wisdom_from_file(f) \
            fftw_import_wisdom(my_fftw_read_char, (void*) (f))
    #define fftwf_import_wisdom_from_file(f) \
            fftwf_import_wisdom(my_fftw_read_char, (void*) (f)) 
    #define fftwl_import_wisdom_from_file(f) \
            fftwl_import_wisdom(my_fftw_read_char, (void*) (f))

#endif

using namespace std;
using namespace eegproc2;

namespace eegproc2 {
    //@{
    // used while debugging only
    // DCD_PERIOD * SAMPLE_PERIOD = 10 Minute
#define DCD_PERIOD 600

#define INIT_DEBUG_PRINT()	static bool firstTime = true;

    void p_DebugWrite ( const TARRAY& a_data,
                        const char* a_fileName,
                        const size_t& a_length,
                        const int& mode = 0 )
    {
        ofstream m_dbgFile;
        if ( mode==0 )
            m_dbgFile.open ( a_fileName );
        else
            m_dbgFile.open ( a_fileName, std::ios::ate );

        if ( !m_dbgFile.is_open () ) {
            cout << "\nError opening\t" << a_fileName << "\n";
            return;
        }
        for ( size_t x = 0; x < a_length; x++ )
            m_dbgFile << a_data[x] << "\t";
        m_dbgFile << "\n";
        m_dbgFile.flush();
        m_dbgFile.close();
    }
}
//@}

#define EXEC_DEBUG_PRINT(data,length,file) \
		if (firstTime == true) \
		{\
			cout<<"writing\t" << file <<endl; \
			eegproc2::p_DebugWrite(data,file,length);\
		}
#define FINALIZE_DEBUG_PRINT() firstTime = false;

#define EXEC_PRINT(data,length,file) \
		eegproc2::p_DebugWrite(data,file,length, -1);// append data

#define TEST_IGNORE_CHANNEL(x,y) \
    bool l_skip; \
    l_skip = false; \
    const std::vector<long>& rIgnores = *(ProcControl::get()->m_args.Ignores);\
    for(long j=0; j < *(ProcControl::get()->m_args.NoOfIgnores);j++){ \
            if(x == rIgnores[j] || \
                    y == rIgnores[j]){ \
                    l_skip = true; \
            } \
    } \
    if(l_skip)continue;

// This is the extrapolated value
#define INTERPOLATE_FEATURE(x,val) \
		postObj.polyfit (x_cord, \
			x, \
			*(m_args.NoOfSamples), \
			1, l_result); \
		val =  \
		l_result[1]*x_cord[*(m_args.NoOfSamples)]/2 +l_result[0];\

#define AIC_FUNC(var, order, length) (length*log(double(var))+2*order)
#define FPE_FUNC(var, order, length) \
                fabs(((1+ order/length)/(1-order/length))*var)
#define MDL_FUNC(var, order, length) \
                fabs(var*log(double(order+1)) + var*log((double)length))

void usage()
{
    cout
    << "\nHelp on command line option\n"
    << "================================================\n"
    << "<debug/release> <operation> <file> <decimation> "
    << "<uV/mV> <totals> <Ignores>\n"
    << "================================================\n"
    << "<file>: file name with directory & extension\n"
    << "<Operation>: Trend, Dump, Raw, Single \n"
    << "<decimation>: rate at which samples should be "
    << "decimated like, 1 in every 2 samples is 2\n"
    << "<uV/mV>if uV then use 1 otherwise use appropriate a_mFactor\n"
    << "<totals>: no of total channels (max: "<< noOfChannels <<")\n"
    << "<a_Ignores>: channels to ignore\n\n"
    << "\n------------------Split Help --------------\n"
    << "<file1> <date> <hour> <channels to sample> "
    << "\n===========================================\n"
    << "\nWARNING! In many options no check has been made\n\n";
    exit ( 0 );
}

time_t g_startTime;
time_t g_endTime;

int
main (int argc, char *argv[] )
{
    const long lc_minArgs = 2;
    if ( argc < lc_minArgs ) {
        usage();
        // call dialog
    }

    Arguments l_args;

    const char* lc_Ops = argv[2];

    // ProcessArguments
    if ( std::strcmp ( lc_Ops,"Trend" ) == 0 ||
         std::strcmp ( lc_Ops,"Dump" ) == 0 ||
         std::strcmp ( lc_Ops,"Raw" ) == 0 ||
         std::strcmp ( lc_Ops,"Single" ) == 0 ) {
        l_args.ProcessGenericArgs ( argc, argv );
    }//---------
    else if ( strcmp ( lc_Ops,"Split" ) == 0 ) {
        l_args.ProcessSplitDirect ( argc, argv );
        exit ( 0 );
    }//----------
    else {
        cout << "invalid operation: "<< lc_Ops << endl;
        exit ( 0 );
    }

    ProcControl* lpProc = ProcControl::get ( &l_args );

    // Do the Job
    // Sum of Spectrum or Cepstrum or Relative Alpha using AR Model
    if ( *(lpProc->m_args.OperationType) == TREND ) {
        lpProc->Process ();
    }//----------
    else if ( strcmp ( lc_Ops,"Dump" ) == 0 ) {
        lpProc->ProcessDump ();
    }//------------
    else if ( strcmp ( lc_Ops,"Raw" ) == 0 ) {
        lpProc->ProcessRaw ();
    }//----------
    else if ( strcmp ( lc_Ops,"Single" ) == 0 ) {
        lpProc->ProcessSingle ();
    }//-------------
    else {
        cout << "invalid a_operation: "<< lc_Ops << endl;
        //		exit(0);
    }

    lpProc->finalize();

    return EXIT_SUCCESS;
}

namespace eegproc2 {
    //[B,A]=butter(6,[1/500,32/250],'bandpass');
    const double
    Filter::m_A[] =
    {1.0000000000000000e+00,      -1.0459065385738768e+01,
        5.0203456282232167e+01,      -1.4625680924207003e+02,
        2.8805743211776132e+02,      -4.0410932773826829e+02,
        4.1409415456473630e+02,      -3.1230999839763689e+02,
        1.7206761928351239e+02,      -6.7540301089880714e+01,
        1.7928832509939827e+01,      -2.8898974592977531e+00,
        2.1390455471046044e-01
    };

    const double
    Filter::m_B[] =
    { 3.0050926219308165e-05,      0.0000000000000000e+00,
        -1.8030555731584898e-04,     0.0000000000000000e+00,
        4.5076389328962241e-04,      0.0000000000000000e+00,
        -6.0101852438616322e-04,     0.0000000000000000e+00,
        4.5076389328962241e-04,      0.0000000000000000e+00,
        -1.8030555731584898e-04,     0.0000000000000000e+00,
        3.0050926219308165e-05
    };

    const short
    Filter::m_FilterLength = 13;

    void
    Chebyshev::operator () ( TARRAY& a_data, // return value
                             const long& order, // 10
                             const double& epsilon, // 0.5
                             const long& Fc, // cutoff frequency: 125;
                             const long& a_FilterLength ) const
    {
        //STEP: Create a Chebyshev filter function in freq domain
        const double epsilon_sq=epsilon*epsilon; //0.5
        TARRAY c ( a_FilterLength,0.0 );
        TARRAY c1 ( a_FilterLength,0.0 );
        TARRAY c2 ( a_FilterLength,0.0 );

        // initialize
        for ( long f=0;f<=a_FilterLength/2;f++ ) {
            c1[f]=1;
            c2[f]= double ( f ) /Fc;
        }
        for ( long j=2;j < order;j++ ) {
            for ( long f=0;f<=a_FilterLength/2;f++ ) {
                c[f] = 2* ( double ( f ) /Fc ) *c2[f] - c1[f];
                c1[f] = c2[f];
                c2[f] = c[f];
            }
        }
        fftw_complex* H = new fftw_complex[a_FilterLength];// filter response
        for ( long f=0;f <= a_FilterLength/2; f++ ) {
            const double Fx = ( f/Fc ) * ( f/Fc ) *c[f]*c[f];
            H[f][0] = 1/sqrt ( 1+epsilon_sq*Fx );
            H[f][1] = 0.0; // imaginary part
        }
        for ( long f=2*a_FilterLength-1;f>a_FilterLength;f-- ) {
            H[f][0] = H[500-f][0]; // Symmetric
            H[f][1] = 0.0; // imaginary part
        }
        //STEP: use fftw to convert it into time domain
        fftw_plan inverse;
        fftw_complex* h = new fftw_complex[a_FilterLength];

        /* Set up the fftw plans. */
        inverse = fftw_plan_dft_1d ( a_FilterLength, H, h,
                                     FFTW_BACKWARD, FFTW_ESTIMATE );
        /* IFFT. */
        fftw_execute ( inverse );

        /* Convert the output to real. and return */
        for ( long x = 0; x < 2*a_FilterLength; x++ ) {
            a_data[x] = h[x][0]/a_FilterLength; // using only the real one
        }
        delete[] h; h=NULL;
        delete[] H;H=NULL;
    }

    // Filter constants a_A and a_B must be of same a_length
    void
    Filter::operator () ( TARRAY& a_data,
                          const long& len,
                          const TARRAY* _A,
                          const TARRAY* _B,
                          const long& a_FilterLength ) const
    {
        TARRAY l_X ( a_FilterLength,0.0 );// FIFO
        TARRAY l_Y ( a_FilterLength,0.0 );//FIFO
        TARRAY a_A ( a_FilterLength,0.0 );
        TARRAY a_B ( a_FilterLength,0.0 );

        // initialize the circular FIFO buffers
        if ( _A == 0 && _B == 0 ) {
            a_A.resize ( Filter::m_FilterLength );
            a_B.resize ( Filter::m_FilterLength );
            for ( long i=0; i < Filter::m_FilterLength;i++ ) {
                a_A[i] = Filter::m_A[i];
                a_B[i] = Filter::m_B[i];
            }
        } else {
            a_A = *_A;
            a_B = *_B;
        }

        long b_index = 0;// b vector index
        long index = 0; // temp index
        double l_num = 0.0;
        for ( long x = 0; x < len;x++ ) {
            // Update current element in X(j) vector
            l_X[b_index] = a_data[x];
            // temporary index based on current b_index
            // that will be used to loop through coefficients and X(j)
            // Y(i) vector
            index = b_index;
            // becuase in difference equation B(j) starts with zero
            l_num = a_B[0]*l_X[index];
            // wrap around index to implement circular buffer
            if ( --index < 0 )
                index = ( long ) a_FilterLength -1;
            // using difference equation
            for ( long i=1; i < a_FilterLength; i++ ) {
                l_num += ( a_B[i]*l_X[index] - a_A[i]*l_Y[index] );
                // decrement index and wrap around again
                if ( --index < 0 )
                    index = ( long ) a_FilterLength -1;
            }
            // Now update current element in Y(i) vector
            l_Y[b_index] = l_num;
            // Store the filtered data
            a_data[x] = l_Y[b_index];
            // current element index in the X(j) and Y(i) need to be incremented
            // and wrap around
            if ( ++b_index == a_FilterLength )
                b_index = 0;
        } //  end processing filtering
    }//end function


    void
    PolyVal::operator ()( const TARRAY& a_X,
                          TARRAY& a_Y,
                          const long& a_length,
                          const long &a_order,
                          const TARRAY& a_result ) const
    {
        for ( long i = 0; i < a_length; i++ ) {
            a_Y[i] = 0;
            double
            p = 1;
            for ( long ord = 0; ord < a_order + 1; ord++ ) {
                a_Y[i] += a_result[ord] * p;
                p    *= a_X[i];
            }
        }
    }//end function

    void
    PolyFit::operator () ( const TARRAY& a_X,
                           const TARRAY& a_Y,
                           const long& a_length,
                           const long& a_order,
                           TARRAY& a_result ) const
    {
        TARRAY  elements ( 2 * a_order + 1, 0.0 );
        elements[0] = ( double ) a_length;

        std::vector<TARRAY*> a_A ( a_order+1,new TARRAY ( a_order+2,0.0 ) );

        for ( TARRAY::iterator i = a_result.begin();
            i != a_result.end(); i++ )
            *i = 0;

        //  create column vector and x_(K-1)
        for ( long k = 0; k < a_length; k++ ) {
            const double &
            yy = a_Y[k];
            const double &
            xx = a_X[k];
            double
            p = 1;  // col vector

            for ( long r = 0; r < a_order + 1; r++ ) {
                // a_B[0] = a_B[0]+Y1* 1, a_B[1] = a_B[1] + Y1 * a_X[1]
                ( *a_A[r] ) [a_order + 1] += yy * p;
                // a_B[2] = a_B[2] + a_Y[1]*a_X[1]*a_X[1]
                p = p * xx;
            }

            //Compute the sum of powers of x_(K-1)
            p = xx;
            for ( long i = 1; i < 2 * a_order + 1; i++ ) {
                elements[i] += p;
                p = p * xx;
            }
        }

        //Determine the matrix entries

        for ( long row = 0; row < a_order + 1; row++ )
            for ( long col = 0; col < a_order + 1; col++ )
                ( *a_A[row] ) [col] = elements[row + col];

        //  Solve the linear system of M_ORDER + 1 equations

        std::vector<long>
        Row ( a_order + 1, 0 ); // Field with row-number

        //Initialize the polonger vector

        for ( long i = 0; i < a_order + 1; i++ )
            Row[i] = i;

        // Start LU factorization

        for ( long p = 0; p < a_order + 1; p++ ) {
            // Find pivot element

            for ( long k = p + 1; k < a_order + 1; k++ ) {
                if ( fabs ( ( *a_A[Row[k]] ) [p] ) > 
                        fabs ( ( *a_A[Row[p]] ) [p] ) ) {
                    long    t = Row[p];
                    Row[p] = Row[k];
                    Row[k] = t;
                }
            }   // simulate row xchange

            if ( ( *a_A[Row[p]] ) [p] == 0 ) {
                cout << "\nA[" << Row[p] << "][" << p
                << "]:\t" << ( *a_A[Row[p]] ) [p]
                << "\nThe matrix is SINGULAR !";
                return;
            }
            // Form multiplier

            for ( long k = p + 1; k < a_order + 1; k++ ) {
                // scale the ith column of the matrix
                // a_A[Row[p][p] is the pivot element
                ( *a_A[Row[k]] ) [p] = 
                        ( *a_A[Row[k]] ) [p] / ( *a_A[Row[p]] ) [p];

                // Eliminate
                for ( long c = p + 1; c < a_order + 2; c++ ) {
                    ( *a_A[Row[k]] ) [c] -=
                    ( *a_A[Row[k]] ) [p] * ( *a_A[Row[p]] ) [c];
                }
                //         a_A[Row[K]][P] = 0;
            }
        }       // End of  L*U  factorization routine
        // now need forward substitution
        for ( long i = 0; i < a_order + 1; i++ ) {
            for ( long j = i + 1; j < a_order + 2; j++ ) {
                ( *a_A[Row[i]] ) [j] /= ( *a_A[Row[i]] ) [i];
            }
            //              a_A[Row[i]][i] = 1;
        }

        // now back substitution
        long    factorCol = 0;
        for ( long k = a_order; k >= 0; k-- ) {
            const double&
            pivot = ( *a_A[Row[k]] ) [a_order + 1];

            factorCol++;
            for ( long j = a_order - factorCol; j >= 0; j-- ) {
                const double& a_mFactor =
                ( *a_A[Row[j]] ) [a_order + 1 - factorCol];
                for ( long col = 0; col < factorCol; col++ )
                    ( *a_A[Row[j]] ) [a_order + 1 - col] -= a_mFactor * pivot;
            }
        }

        for ( long i = 0; i < a_order + 1; i++ )
            a_result[i] = ( *a_A[Row[i]] ) [a_order + 1];//end for
    }// end function

    ///////////
    void Arguments::ProcessSplitDirect ( const long& argc,
                                         char *argv[] )
    {
        const long lc_minArgs = 6;
        const long lc_maxArgs = 20;

        long channels[lc_maxArgs-lc_minArgs];
        double channels_data[lc_maxArgs-lc_minArgs];

        if ( argc < lc_minArgs ||
             argc > lc_maxArgs ) {
            usage();
        }

        long noOfChannels = argc-lc_minArgs;

        const char* g_date = argv[4];
        const char* g_hour = argv[5];

        if ( noOfChannels > 0 ) {
            for ( long i=0; i < noOfChannels; i++ ) {
                channels[i] = ( long ) atof ( argv[lc_minArgs + i] );
                cout << "Sampling : "<< channels[i] << endl;
            }
        } else {
            channels[0] = 0;
            noOfChannels = 1;
        }

        ifstream l_inputFile ( argv[3] );
        // check if successfully opened  Patient data
        if ( !l_inputFile.is_open () ) {
            cout << "Error opening " << argv[3];
            exit ( 1 );
        } else {
            cout << "\nProcessing " << argv[3] << endl;
        }

        ofstream l_outputFile ( "sample.txt", ios::app );

        char
        l_buffer[MAX_BUFFER_LEN];

        long rowIndex = 0;
        long sampleIndex = 0;

        cout << "-" << g_date << "-" << g_hour << "\n";

        while ( !l_inputFile.eof () ) {
            l_inputFile.getline ( l_buffer, MAX_BUFFER_LEN );

            if ( rowIndex < FRAME_LENGTH &&
                 sampleIndex == 0 ) {
                // this array will contain channelData a_data
                istringstream input ( l_buffer );
                // reading
                char l_date[32], l_hour[32];
                input >> l_date >> l_hour;

                // writing
                if ( strcmp ( g_date,l_date ) == 0 &&
                     g_hour[0] == l_hour[0] &&
                     g_hour[1] == l_hour[1] ) {

                    for ( long i=0; i < NO_MONTAGES; i++ ) {
                        input >> channels_data[i];
                    }

                    for ( long i=0; i < noOfChannels; i++ ) {
                        l_outputFile << channels_data[channels[i]] << "\t";
                    }

                    l_outputFile << "\n";
                }// end if date/hour matched sampling
            }// end if with data writing
            if ( sampleIndex >= SAMPLE_SKIP ) {
                sampleIndex = 0;
                rowIndex = 0;
            } else if ( rowIndex >=FRAME_LENGTH ) {
                rowIndex = 0;
                sampleIndex++;
            } else {
                rowIndex++;
            }
        }//end while
        l_inputFile.close();
        l_outputFile.close();
    }

    Arguments::Arguments()
    :Debug( false ),
    Decimation( 0 ),
    Factor( 0 ),
    NoOfCols( 0 ),
    NoOfIgnores( 0 ),
    NoOfSamples( 0 )
    {
    }

    Arguments::~Arguments()
    {
    }

    void
    Arguments::ProcessGenericArgs ( const long& argc,
                                    char *argv[] )
    {
        const long lc_minArgs = 7;
        if ( argc < lc_minArgs ) {
            usage();
        }

        Debug = ( ( strcmp ( argv[1],"debug" ) ==0 ) ?true:false );

        if ( strcmp ( argv[2],"Trend" ) == 0 ) {
            OperationType =  TREND ;
        } else if ( strcmp ( argv[2],"Dump" ) == 0 ) {
            OperationType = DUMP ;
        } else if ( strcmp ( argv[2],"Raw" ) == 0 ) {
            OperationType = RAW ;
        } else if ( strcmp ( argv[2],"Single" ) == 0 ) {
            OperationType = SINGLE ;
        } else if ( strcmp ( argv[2],"Split" ) == 0 ) {
            OperationType = SPLIT ;
            this->ProcessSplitDirect ( argc, argv );
            return;
        }//----------

        FileName = argv[3] ;

        Decimation = ( long ) atof ( argv[4] );

        Factor = ( long ) atof ( argv[5] );

        NoOfCols =  ( long ) atof ( argv[6] );

        NoOfIgnores = ( argc - lc_minArgs );

        std::vector<long>& rIgnores = 
                                const_cast<std::vector<long>&> (*(Ignores));
        if ( *NoOfIgnores > 0 ) {
            rIgnores.resize ( *NoOfIgnores );
            for ( long i=0; i < *NoOfIgnores; i++ ) {
                rIgnores[i] = ( long ) atof ( argv[lc_minArgs+i] );
                cout << "Ignoring : "<< rIgnores[i] << endl;
            }
        } else {
            rIgnores[0] = -1;
        }

        if ( *OperationType == TREND ) {
            NoOfSamples =  ( long ) DCD_PERIOD ;
        } else {
            NoOfSamples = 1 ;
        }
    }

    void
    EEGReader::init ( const Arguments& a_args )
    {
        this->RowTags = 0;

        p_inputFile.open ( (*a_args.FileName).c_str() );
        // check if successfully opened  Patient data
        if ( !p_inputFile.is_open () ) {
            cout << "Error opening\t" <<(*a_args.FileName) <<"\t";
            exit ( 1 );
        } else {
            cout << "\nProcessing:\t" << (*a_args.FileName) << endl;
        }
    }

    void
    EEGWriter::init()
    {
        RowCounters = 0;
        m_timestampFile.open ( "timestamp.txt", ios::app );
        m_headerFile.open ( "header.txt", ios::app );

        // check if successful
        if ( !m_headerFile.is_open () ||
             !m_timestampFile.is_open () ) {
            cout << "Error creating/opening output file";
            exit ( 1 );
        }
        m_headerFile << "\n";
        m_timestampFile << "\n";
        // Create output file heading
        for ( long i = 0;
            i < NO_MONTAGES; i++ ) {
            const long x = g_montages[i][0];
            const long y = g_montages[i][1];
            // l_skip ignorechannels
            TEST_IGNORE_CHANNEL ( x,y )

            m_headerFile
            << g_channels[x-1].montage << "-"
            << g_channels[y-1].montage << "\t" ;
        }

        m_headerFile << "\n";
        m_headerFile.flush();

    }

    /*!
    @name ProcControl
    @brief A Function to clean member resources upon call
    @return
    */

    ProcControl::ProcControl
    ( const Arguments& a_args ) :m_args ( a_args )
    {
        // start timer
        time ( &g_startTime );
        reader.init ( a_args );
        MeanFPE = 0.0 ;
        MeanOrder = 0.0 ;
        MeanLength = 0.0 ;
    }

    /*!
    @name ~ProcControl
    @brief A Function to clean member resources upon call
    @return
    */

    ProcControl::~ProcControl ( void )
    {
    }

    void
    EEGWriter::finalize ( const Arguments& a_args )
    {
        m_headerFile << "\n% Time taken in second: "
        << g_endTime - g_startTime;

        m_headerFile << "\n% Resolution(# Samples): "
        << *(a_args.NoOfSamples);

        m_headerFile << "\n% Max AR Order: " << MAX_ARORDER;

        m_headerFile << "\n% Max Frame Length: " << FRAME_LENGTH;

        m_headerFile << "\n% Max Sample Period: "
        << SAMPLE_PERIOD;

        m_headerFile << "\n% Decimation rate: "
        << *(a_args.Decimation);

        m_headerFile << "\n% (1:= uV 1000:= mV): "
        << *(a_args.Factor) ;

        if ( *(a_args.OperationType) == TREND ) {

            if ( *(RowCounters) == 0 ) RowCounters = 1;

            m_headerFile << "\n% mean Ep: "
            << *(ProcControl::get()->MeanFPE) / double (*RowCounters) *
            NO_MONTAGES * (*(a_args.NoOfSamples)) ;

            m_headerFile << "\n% mean Order: "
            << *(ProcControl::get()->MeanOrder) / (double(*(RowCounters)) *
                                 NO_MONTAGES * (*(a_args.NoOfSamples))) <<"\n";

            m_headerFile
            << "\n% mean Length: "
            << *(ProcControl::get()->MeanLength) / (double (*(RowCounters)) *
                     double ( NO_MONTAGES ) * (*(a_args.NoOfSamples) ) ) <<"\n";
        }

        m_headerFile.flush();
        m_headerFile.close();
        m_timestampFile.flush();
        m_timestampFile.close();
    }

    void
    EEGReader::finalize ( void )
    {
        p_inputFile.close ();
    }

    void
    ProcControl::finalize ( void )
    {
        time ( &g_endTime );
        reader.finalize();
        writer.finalize ( m_args );
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
    PreProcess::p_WriteDump ( const TARRAY*const a_data,
                              const TARRAY_MARKER& a_marker )
    {
        ofstream outFile ( "Dump.txt" );
        for ( long row=0; row < FRAME_LENGTH; row++ ) {
            outFile << a_marker[row].date << "\t"
            << a_marker[row].time << "\t";
            for ( long col = 0; col < NO_MONTAGES; col++ ) {
                const long x = g_montages[col][0];// source a_montage channel
                const long y = g_montages[col][1]; //  REF a_montage channel
                // l_skip ignorechannels
                TEST_IGNORE_CHANNEL ( x,y )
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
    PreProcess::p_WriteSingle ( const TARRAY*const a_data,
                                const TARRAY_MARKER& a_marker )
    {
        ofstream outFile ( "Single.txt" );
        for ( long row=0; row < FRAME_LENGTH; row++ ) {
            // execute
            for ( long i=0; i < NO_MONTAGES; i++ ) {
                const long x = g_montages[i][0];// source a_montage
                const long y = g_montages[i][1]; //  REF a_montage

                // l_skip ignorechannels
                TEST_IGNORE_CHANNEL ( x,y )
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
    EEGWriter::p_Write ( const TARRAY a_avgFPE,
                         const TARRAY a_order,
                         const TARRAY a_length,
                         TARRAY_MARKER& a_marker )
    {
        m_timestampFile << a_marker[0].date << "\t"
        << a_marker[0].time << "\n";

        this->RowCounters = *(this->RowCounters)+1;

        // execute
        for ( long i=0; i < NO_MONTAGES; i++ ) {
            const long x = g_montages[i][0];// source a_montage
            const long y = g_montages[i][1]; //  REF a_montage

            // l_skip ignorechannels
            TEST_IGNORE_CHANNEL ( x,y )
            ProcControl* proc = ProcControl::get();
            proc->MeanFPE = *(proc->MeanFPE) + a_avgFPE[i];
            proc->MeanOrder = *(proc->MeanOrder) + a_order[i];
            proc->MeanLength = *(proc->MeanLength) + a_length[i];
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
    EEGReader::p_Read ( TARRAY* a_data,
                        TARRAY_MARKER& a_marker )
    {
        char
        l_buffer[MAX_BUFFER_LEN];
        long
        index = 0;
        unsigned long long
        l_oldTag = 0;
        bool
        l_jumpCounter = false;
        TARRAY
        l_data[REF];

        a_marker.resize ( FRAME_LENGTH );

        while ( !p_inputFile.eof () && index < FRAME_LENGTH ) {
            p_inputFile.getline ( l_buffer, MAX_BUFFER_LEN );

            // make sure first character is % for a comment
            if ( l_buffer[0] != '%' ) {
                // This line ensures 4 sec sampling by toggling jump counter
                if ( *(ProcControl::get()->m_args.Decimation) == 
                                2 && l_jumpCounter == true ) {
                    l_jumpCounter = false;
                    continue;
                } else {
                    l_jumpCounter = true;
                }
                // this array will contain channelData a_data
                istringstream input ( l_buffer );
                // formatted row marker from text l_buffer
                input >> a_marker[index].date >> a_marker[index].time 
                      >> (*(this->RowTags));
                // now read consecutive 20 channelData
                for ( long i = 1; i <= *(ProcControl::get()->m_args.NoOfCols); 
                        i++ ) {
                    char
                    l_dataBuffer[DATA_READ_LEN];

                    input >> l_dataBuffer;

                    TEST_IGNORE_CHANNEL ( i,i ) // redundant but simple

                    if ( strcmp ( l_dataBuffer, "AMPSAT" ) == 0
                         || strcmp ( l_dataBuffer, "SHORT" ) == 0
                         || strcmp ( l_dataBuffer, "OFF" ) == 0 ) {
                        index = -1;
                        break;  // drop the line
                    }

                    if ( l_data[i-1].size() != FRAME_LENGTH ) 
                        l_data[i-1].resize ( FRAME_LENGTH );

                    l_data[i - 1][index] =
                    double ( atof ( l_dataBuffer ) ) * 
                            ( *(ProcControl::get()->m_args.Factor) );
                    // may need math.h in windows
                }   // end for capturing 20 channel a_data

            }   //endif
            else {
                if ( *(ProcControl::get()->m_args.Debug) == true ) {
                    cout << l_buffer << "\n";
                }
                continue;
            }
            if ( ( index > 0 && *(this->RowTags) == ( l_oldTag + 
                    *(ProcControl::get()->m_args.Decimation) ) ) || 
                    index == 0 ) {
                l_oldTag = *(this->RowTags);    // continue to read
            } else {
                index = 0;
                l_jumpCounter = false;
                continue;   // drop the frame
            }

            ++index;

        }       //end while and FRAME_LENGTH
        // set the number of item read in this pass
        if ( index < FRAME_LENGTH ) return index; // terminate program
        this->p_ProcessMontages ( a_data,   l_data );
        return index;
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
    PreProcess::p_PreProcess ( TARRAY& a_data ) const
    {
        // This frame is ready for further processing
        INIT_DEBUG_PRINT()
        const long l_data_length = ( long ) a_data.size();

        EXEC_DEBUG_PRINT ( a_data,l_data_length, RAW_TXT )

        this->p_DcRemove ( a_data, l_data_length );

        EXEC_DEBUG_PRINT ( a_data, l_data_length, DC_TXT )

        this->p_RemoveTrend ( a_data, l_data_length, 1.0 );

        EXEC_DEBUG_PRINT ( a_data, l_data_length, BLW_TXT )

        const long a_noelements = MA_POINTS;

        maFilter ( a_data, l_data_length, a_noelements );

        EXEC_DEBUG_PRINT ( a_data, l_data_length, MA_TXT )

        filter ( a_data, l_data_length ); // use default

        EXEC_DEBUG_PRINT ( a_data, l_data_length, BUTTER_TXT )

        FINALIZE_DEBUG_PRINT()
    }// end preprocess

    void
    EEGReader::p_ProcessMontages ( TARRAY* a_montage,
                                   TARRAY* a_data ) const
    {
        // Create Montages
        for ( long i=0;
            i < NO_MONTAGES;i++ ) {
            const long x = g_montages[i][0]-1;// source a_montage
            const long y = g_montages[i][1]-1;//  REF a_montage

            if ( a_montage[i].size() != a_data[x].capacity() )
                a_montage[i].resize ( a_data[x].capacity() );

            for ( size_t j=0;j<a_data[x].capacity(); j++ ) {
                const double ref = a_data[y][j];
                a_montage[i][j] = a_data[x][j] - ref;
            }

            INIT_DEBUG_PRINT()
            EXEC_DEBUG_PRINT ( a_data[x], a_data[x].capacity(), X_TXT )
            EXEC_DEBUG_PRINT ( a_data[y], a_data[y].capacity(), Y_TXT )
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
    ProcControl::Process ( void )
    {
        TARRAY x_cord ( DCD_PERIOD );
        // cepstrum for each frame
        TARRAY ceps[NO_MONTAGES];
        TARRAY spec[NO_MONTAGES];
        TARRAY ra[NO_MONTAGES];
        TARRAY sfr[NO_MONTAGES];
        TARRAY lstOrder[NO_MONTAGES];
        TARRAY lstLength[NO_MONTAGES];

        // consecutive input a_data
        TARRAY l_data[NO_MONTAGES];
        TARRAY_MARKER  l_marker;

        long
        second = 0;
        while ( !reader.p_inputFile.eof () &&
                reader.p_Read ( l_data,
                                l_marker ) == FRAME_LENGTH ) {
            TARRAY l_arorder ( NO_MONTAGES );
            TARRAY l_avgFPE ( NO_MONTAGES );
            TARRAY l_length ( NO_MONTAGES );

            for ( long i=0;
                i < NO_MONTAGES;i++ ) {
                const long x = g_montages[i][0];// source a_montage
                const long y = g_montages[i][1]; //  REF a_montage

                if ( ceps[i].size() != DCD_PERIOD ) 
                    ceps[i].resize ( DCD_PERIOD );
                if ( spec[i].size() != DCD_PERIOD ) 
                    spec[i].resize ( DCD_PERIOD );
                if ( ra[i].size() != DCD_PERIOD ) 
                    ra[i].resize ( DCD_PERIOD );
                if ( sfr[i].size() != DCD_PERIOD ) 
                    sfr[i].resize ( DCD_PERIOD );
                if ( lstOrder[i].size() != DCD_PERIOD ) 
                    lstOrder[i].resize ( DCD_PERIOD );
                if ( lstLength[i].size() != DCD_PERIOD )
                    lstLength[i].resize ( DCD_PERIOD );

                // l_skip ignorechannels
                TEST_IGNORE_CHANNEL ( x,y )
                preObj.p_PreProcess ( l_data[i] );
                // Now do the analysis
                TARRAY  Ao ( MAX_ARORDER + 1 );
                double l_Ep = 0.0;
                TARRAY l_wgn ( FRAME_LENGTH );
                long order =MAX_ARORDER;
                long length = ( long ) l_data[i].capacity();

                static const PostProcess postProc;

                ceps[i][second] =
                postProc.
                p_SumOfCepstrums ( l_data[i],
                                   l_wgn,
                                   Ao,
                                   l_Ep,
                                   length,
                                   order );

                l_avgFPE[i] += l_Ep;
                l_arorder[i] += order;
                l_length[i] += length;

                // Provides both RA and spectrum
                spec[i][second] =
                postProc.
                p_SumOfSpectrum ( Ao,
                                  l_Ep,
                                  ra[i][second],
                                  length,
                                  order );

                const long wgn_length = long ( ( length / SAMPLE_PERIOD ) *
                                               FRAME_LENGTH );
                sfr[i][second] =
                postProc.p_wgnSFR ( l_wgn,
                                    wgn_length );

                lstOrder[i][second] = order;
                lstLength[i][second]= length;

                INIT_DEBUG_PRINT()
                EXEC_DEBUG_PRINT ( l_wgn,wgn_length,RESIDUALX_TXT )
                FINALIZE_DEBUG_PRINT()
            }

            const double periodOffset = double ( SAMPLE_PERIOD ) /2;

            if ( second == (*(m_args.NoOfSamples) -1 ) ) {
                // initialized first element
                x_cord[0] = periodOffset;
                // Results
                TARRAY l_slopeCeps ( NO_MONTAGES );
                TARRAY l_slopeSpec ( NO_MONTAGES );
                TARRAY l_slopeRA ( NO_MONTAGES );
                TARRAY l_slopeSFR ( NO_MONTAGES );
                TARRAY l_slopeOrder ( NO_MONTAGES );
                TARRAY l_slopeLength ( NO_MONTAGES );

                INIT_DEBUG_PRINT()
                EXEC_DEBUG_PRINT ( x_cord, *(m_args.NoOfSamples),
                                   XINTERPOLATE_TXT )

                for ( long i = 0;
                    i < NO_MONTAGES; i++ ) {
                    const long x = g_montages[i][0];// source a_montage
                    const long y = g_montages[i][1]; //  REF a_montage

                    // l_skip ignorechannels
        TEST_IGNORE_CHANNEL ( x,y )
        EXEC_DEBUG_PRINT ( ra[i], *(m_args.NoOfSamples), ALPHA_TXT )
        EXEC_DEBUG_PRINT ( spec[i], *(m_args.NoOfSamples), SPEC_TXT )
        EXEC_DEBUG_PRINT ( ceps[i], *(m_args.NoOfSamples), CEPS_TXT )
        EXEC_DEBUG_PRINT ( sfr[i], *(m_args.NoOfSamples), SFR_TXT )
        EXEC_DEBUG_PRINT ( lstOrder[i], *(m_args.NoOfSamples), ORDERS_TXT )
        EXEC_DEBUG_PRINT ( lstLength[i], *(m_args.NoOfSamples), LENGTHS_TXT )
        FINALIZE_DEBUG_PRINT()

                    TARRAY l_result ( 2, 0.0 );
                    // Next RA
                    INTERPOLATE_FEATURE ( ra[i], l_slopeRA[i] )
                    // Next ceps
                    INTERPOLATE_FEATURE ( ceps[i], l_slopeCeps[i] )
                    // Next Spec
                    INTERPOLATE_FEATURE ( spec[i], l_slopeSpec[i] )
                    // Next SFR
                    INTERPOLATE_FEATURE ( sfr[i], l_slopeSFR[i] )
                    // Next orders
                    INTERPOLATE_FEATURE ( lstOrder[i], l_slopeOrder[i] )
                    // Next lengths
                    INTERPOLATE_FEATURE ( lstLength[i], l_slopeLength[i] )
                }// end for
                // now write them to text fil
                EXEC_PRINT ( l_slopeCeps, NO_MONTAGES, CEPSOUT_TXT )
                EXEC_PRINT ( l_slopeSpec, NO_MONTAGES, SPECOUT_TXT )
                EXEC_PRINT ( l_slopeRA, NO_MONTAGES, ALPHAOUT_TXT )
                EXEC_PRINT ( l_slopeSFR, NO_MONTAGES, SFROUT_TXT )
                EXEC_PRINT ( l_slopeOrder, NO_MONTAGES, ORDERSOUT_TXT )
                EXEC_PRINT ( l_slopeLength, NO_MONTAGES, LENGTHSOUT_TXT )

                writer.p_Write ( l_avgFPE, l_arorder, l_length, l_marker );
                second = 0;
            } // if
            else {
                // This code ensures proper timing
                // axis - X axis
                x_cord[second] = double(*(reader.RowTags));
                if ( second > 0 ) {
                    x_cord[second] -= x_cord[0];
                    x_cord[second] = ( double ) floor ( x_cord[second] / 
                                                        double ( FRAME_LENGTH )
                                                      * double ( SAMPLE_PERIOD )
                                                        + periodOffset );
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
    ProcControl::ProcessDump ( void )
    {
        // consecutive input a_data
        TARRAY l_data[NO_MONTAGES];
        TARRAY_MARKER  l_marker;
        while ( !reader.p_inputFile.eof () &&
                reader.p_Read ( l_data,
                                l_marker ) == FRAME_LENGTH ) {
            // execute
            for ( long i=0;
                i < NO_MONTAGES;i++ ) {
                const long x = g_montages[i][0];// source a_montage
                const long y = g_montages[i][1]; //  REF a_montage

                // l_skip ignorechannels
                TEST_IGNORE_CHANNEL ( x,y )
                preObj.p_PreProcess ( l_data[i] );
            }
            preObj.p_WriteDump ( l_data,
                                 l_marker );
        }
    }

    /*!
    @name ProcessSingle
    @brief A Function to dump preprocess data no analysis
    @param void
    @return void
    */

    void
    ProcControl::ProcessSingle ( void )
    {
        // consecutive input a_data
        TARRAY l_data[REF];
        TARRAY_MARKER  l_marker;
        while ( !reader.p_inputFile.eof () &&
                reader.p_Read ( l_data, l_marker ) == FRAME_LENGTH ) {
            // execute
            for ( long i=0;
                i < NO_MONTAGES;i++ ) {
                const long x = g_montages[i][0];// source a_montage
                const long y = g_montages[i][1]; //  REF a_montage

                // l_skip ignorechannels
                TEST_IGNORE_CHANNEL ( x,y )
                preObj.p_PreProcess ( l_data[i] );
            }

            preObj.p_WriteSingle ( l_data, l_marker );
        }
    }
    /*!
    @name ProcessRaw
    @brief A Function to dump unprocessed data no analysis
    @param void
    @return void
    */
    void
    ProcControl::ProcessRaw ( void )
    {
        // consecutive input a_data
        TARRAY l_data[NO_MONTAGES];
        TARRAY_MARKER  l_marker;
        while ( !reader.p_inputFile.eof () &&
                reader.p_Read ( l_data, l_marker ) == FRAME_LENGTH ) {
            preObj.p_WriteDump ( l_data, l_marker );
        }
    }


    void
    PreProcess::p_DcRemove ( TARRAY& a_data, const size_t& length ) const
    {
        double  sum = 0.0;

        for ( size_t index = 0;index < length; index++ )
            sum += a_data[index];

        const double    average = sum / double ( length );
        // correct the dc value in the frame
        for ( size_t index = 0;index < length; index++ )
            a_data[index] -= average;

    }           // end function dcRemove

    void
    PreProcess::p_RemoveTrend ( TARRAY& a_data,
                                const long& length,
                                const double& period ) const
    {
        TARRAY  x_cord ( length );
        const double offset = 0.5/double ( length );
        for ( long i = 0; i < length; i++ ) {
            x_cord[i] =
            ( double ( i ) /length ) *double ( period ) + offset;
        }

        INIT_DEBUG_PRINT()
        EXEC_DEBUG_PRINT ( x_cord, length, "Xblw.txt" )
        FINALIZE_DEBUG_PRINT()

        TARRAY  a_result ( MAX_ORDER + 1 );
        polyfit ( x_cord, a_data, length,
                  POLYNOMIAL_ORDER, a_result );

        TARRAY  correctedChannelData ( length );
        polyval ( x_cord, correctedChannelData, length,
                  POLYNOMIAL_ORDER, a_result );

        // correct the low freq noise
        for ( long index = 0; index < length; index++ )
            a_data[index] -=
            correctedChannelData[index];
    }//end function

    double
    PostProcess::p_GetOptimumAR ( const TARRAY& a_data,
                                  TARRAY& a_wgn,
                                  TARRAY& a_AR,
                                  double& a_Ep,
                                  long& a_order,
                                  long& a_length ) const
    {
        // This is performance criteria, i.e. Variance
        TARRAY Ao ( a_order + 1 );
        TARRAY Bo ( a_order + 1 );
        a_Ep = arburg( a_data, Ao, Bo, a_order, a_wgn);
        //TARRAY newWgn = a_wgn.copy(0, a_length);


        long last_length = FRAME_LENGTH;

        double last_MDL = MDL_FUNC ( 1,a_order,last_length ); //it is always 1
        const double init_Ep = a_Ep;

        const double DELTA_LENGTH = 10;
        const double MINIMUM_LENGTH = 900;
        // Calculate Optimum AR parameters
        const long limit_test = long ( FRAME_LENGTH / DELTA_LENGTH );

        const long start_polong = long ( MINIMUM_LENGTH / DELTA_LENGTH );

        for ( long i = limit_test - 1;
            i > start_polong; i-- ) {
            // Store the last known good values
            for ( long j=0; j < MAX_ARORDER; j++ ) {
                a_AR[j] = Ao[j];
            }

            const long a_length = long ( i * DELTA_LENGTH );

            a_order = arburg ( a_data, a_wgn, Ao, MAX_ARORDER, a_Ep, a_length );

            const double perf = ( a_length / double ( FRAME_LENGTH ) ) * 
                                ( a_Ep/init_Ep );

            double cur_MDL = MDL_FUNC ( perf, a_order, a_length );

            if ( cur_MDL > last_MDL ) break;

            last_MDL = cur_MDL;

            last_length = a_length;
        }
        a_length = last_length;
        return last_length * double ( SAMPLE_PERIOD ) /double ( FRAME_LENGTH );
    }

    double
    PostProcess::p_SumOfSpectrum ( const TARRAY& a_AR,
                                   const double& a_Ep,
                                   double& a_frameRA,
                                   const double& a_length,
                                   const long& a_order ) const

    {
        const double MAX_FREQ = .128;
        const double MIN_FREQ = .008;
        const double DELTA_FREQ = .002;
        const long l_power_length =
        long ( ( MAX_FREQ - MIN_FREQ ) /DELTA_FREQ );
        double l_sumFreq=0.0;

        // 1/250, 20/250
        //		const double RA_BASE_MIN = .004;
        //		const double RA_BASE_MAX = .08;
        //		double l_ra_base = 0.0;
        // 6/250, 14/250
        const double RA_MIN_FREQ = .024;
        const double RA_MAX_FREQ = .056;
        const long l_ra_length =
        long ( ( RA_MAX_FREQ - RA_MIN_FREQ ) /DELTA_FREQ );
        double l_ra = 0.0;

        TARRAY l_power ( l_power_length );
        TARRAY l_freq ( l_power_length );

        long index = 0;
        double gmean_F = 0;
        double l_ra_gmean = 0;

        for ( double x = MIN_FREQ; x < MAX_FREQ; x+=DELTA_FREQ ) {
            double l_real = 1;
            double l_imaginary = 0;

            for ( long i = 1; i <= a_order; i++ ) {
                l_real += a_AR[i] * cos ( x * i * a_length );
                l_imaginary += a_AR[i] * sin ( x* i * a_length );
            }
            complex<double> l_cmplx_pwr ( l_real,l_imaginary );

            // Normalized power spectrum amplitude
            l_cmplx_pwr = ( a_Ep * a_length ) / ( l_cmplx_pwr * l_cmplx_pwr );

            const double curPower = abs ( l_cmplx_pwr );

            const double logPower = log ( curPower );

            l_power[index] = 10*log10 ( curPower );
            l_freq[index++] = x;

            l_sumFreq += curPower;

            if ( x >= RA_MIN_FREQ && x <= RA_MAX_FREQ ) {
                l_ra += curPower;
                l_ra_gmean += logPower;
            }
            l_sumFreq += curPower;
            gmean_F += log ( curPower );
        }
        l_sumFreq /= l_power_length;
        gmean_F /= l_power_length;
        l_ra /= l_ra_length;
        l_ra_gmean /= l_ra_length;

        INIT_DEBUG_PRINT()
        EXEC_DEBUG_PRINT ( l_power,l_power_length,"Pf.txt" )
        EXEC_DEBUG_PRINT ( l_freq,l_power_length,"freq.txt" )
        FINALIZE_DEBUG_PRINT()

        // dont use the ratio
        a_frameRA = ( 10*log10 ( exp ( l_ra_gmean ) /l_ra ) );
        return( 10*log10 ( exp ( gmean_F ) /l_sumFreq ) );
    }

    double
    PostProcess::p_wgnSFR ( TARRAY& a_data,
                            const long& a_length ) const
    {
        FILE *wisdom;
        static fftw_plan p1;
        static fftw_complex h[FRAME_LENGTH], H[FRAME_LENGTH];

        /* Keep the arrays and plans around from last time, since this
        is a very common case. Reallocate them if they change. */
        INIT_DEBUG_PRINT()
        if ( firstTime ) {
            /* Get any accumulated wisdom. */
            wisdom = fopen ( "wisfile", "r" );
            if ( wisdom ) {
                fftw_import_wisdom_from_file ( wisdom );
                fclose ( wisdom );
            }

            /* Set up the fftw plans. */
            p1 = fftw_plan_dft_1d ( FRAME_LENGTH, h, H,
                                    FFTW_FORWARD,FFTW_MEASURE );
            /* Save the wisdom. */
            wisdom = fopen ( "wisfile", "w" );
            fftw_export_wisdom_to_file ( wisdom );
            fclose ( wisdom );
        }
        // do not need to recreate the plan
        FINALIZE_DEBUG_PRINT()

        /* Convert the input to complex. Also compute the mean. */
        for ( long x = 0; x < FRAME_LENGTH; x++ ) {
            if ( x < a_length ) {
                h[x][0] = a_data[x];
                h[x][1] = 0.0;
            } else {
                h[x][0] = 0.0;
                h[x][1] = 0.0;
            }
        }

        /* FFT. */
        fftw_execute ( p1 );

        double l_sumFreq = 0.0;
        double gmean_F = 0.0;
        for ( long x = 0; x < FRAME_LENGTH; x++ ) {
            complex<double> l_cmplx_pwr ( H[x][0],H[x][1] );

            const double curPower = abs ( l_cmplx_pwr );

            l_sumFreq += curPower;
            gmean_F += log ( curPower );
        }
        l_sumFreq /= FRAME_LENGTH;
        gmean_F /= FRAME_LENGTH;
        return( 20*log10 ( exp ( gmean_F ) /l_sumFreq ) );
    }

    double
    PostProcess::p_SumOfCepstrums ( const TARRAY& a_data,
                                    TARRAY& a_wgn,
                                    TARRAY& a_AR,
                                    double& a_Ep,
                                    long& a_length,
                                    long& a_order ) const
    {
        // This is suppose to be long
        double l_length
        = this->
          p_GetOptimumAR ( a_data, a_wgn, a_AR, a_Ep, a_order, a_length );

        // Now Calculates Cepstrum coefficients upto LowestOrder+1
        INIT_DEBUG_PRINT()

        a_AR[0] = l_length;

        EXEC_DEBUG_PRINT ( a_AR, long ( a_order+1 ), "Ao.txt" )

        a_AR[0] = 1;

        TARRAY  l_Cep ( a_length + 2 ); // leave it as is

        double a_result = arceps ( a_AR, l_Cep, a_order+1, a_length );

        EXEC_DEBUG_PRINT ( l_Cep, long ( a_length + 2 ), "arceps.txt" )

        FINALIZE_DEBUG_PRINT()

        return a_result;
    }// end function

}// end namespace
