#include <stdio.h>
#if defined(WIN32)
#include <conio.h>
#endif
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif
#include "angelscript.h"

namespace TestBasic        { void Test(double *time); }
namespace TestBasic2       { void Test(double *time); }
namespace TestCall         { void Test(double *time); }
namespace TestCall2        { void Test(double *time); }
namespace TestFib          { void Test(double *time); }
namespace TestInt          { void Test(double *time); }
namespace TestIntf         { void Test(double *time); }
namespace TestMthd         { void Test(double *time); }
namespace TestString       { void Test(double *time); }
namespace TestStringPooled { void Test(double *time); }
namespace TestString2      { void Test(double *time); }
namespace TestThisProp     { void Test(double *time); }
namespace TestVector3      { void Test(double *time); }
namespace TestAssign       { void Test(double *times); }
namespace TestArray        { void Test(double *time); }

// Times for 2.28.1 WIP
double testTimesOrig[19] = 
{
1.375,  // Basic
0.266,  // Basic2
0.500,  // Call
0.953,  // Call2
2.250,  // Fib
0.297,  // Int
2.281,  // Intf
2.031,  // Mthd
1.797,  // String
0.891,  // String2
1.172,  // StringPooled
0.797,  // ThisProp
0.438,  // Vector3
0.625,  // Assign.1
0.813,  // Assign.2
0.609,  // Assign.3
1.094,  // Assign.4
1.094,  // Assign.5
0.922   // Array
};

// Times for 2.28.1 WIP 1 (localized optimizations)
double testTimesOrig2[19] = 
{
1.406,  // Basic
0.250,  // Basic2
0.500,  // Call
0.953,  // Call2
2.391,  // Fib
0.328,  // Int
2.125,  // Intf
2.109,  // Mthd
1.766,  // String
0.875,  // String2
1.172,  // StringPooled
0.797,  // ThisProp
0.453,  // Vector3
0.625,  // Assign.1
0.813,  // Assign.2
0.625,  // Assign.3
0.922,  // Assign.4
0.906,  // Assign.5
0.906   // Array
};

double testTimesBest[19];
double testTimes[19];

void DetectMemoryLeaks()
{
#if defined(_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
	_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT,_CRTDBG_FILE_STDERR);
#endif
}

int main(int argc, char **argv)
{
	DetectMemoryLeaks();

	printf("Performance test");
#ifdef _DEBUG 
	printf(" (DEBUG)");
#endif
	printf("\n");
	printf("AngelScript %s\n", asGetLibraryVersion()); 

	int n;
	for( n = 0; n < 19; n++ )
		testTimesBest[n] = 1000;

	for( n = 0; n < 3; n++ )
	{
		TestBasic::Test(&testTimes[0]); printf("."); fflush(stdout);
		TestBasic2::Test(&testTimes[1]); printf("."); fflush(stdout);
		TestCall::Test(&testTimes[2]); printf("."); fflush(stdout);
		TestCall2::Test(&testTimes[3]); printf("."); fflush(stdout);
		TestFib::Test(&testTimes[4]); printf("."); fflush(stdout);
		TestInt::Test(&testTimes[5]); printf("."); fflush(stdout);
		TestIntf::Test(&testTimes[6]); printf("."); fflush(stdout);
		TestMthd::Test(&testTimes[7]); printf("."); fflush(stdout);
		TestString::Test(&testTimes[8]); printf("."); fflush(stdout);
		TestString2::Test(&testTimes[9]); printf("."); fflush(stdout);
		TestStringPooled::Test(&testTimes[10]); printf("."); fflush(stdout);
		TestThisProp::Test(&testTimes[11]); printf("."); fflush(stdout);
		TestVector3::Test(&testTimes[12]); printf("."); fflush(stdout);
		TestAssign::Test(&testTimes[13]); printf("."); fflush(stdout);
		TestArray::Test(&testTimes[18]); printf("."); fflush(stdout);

		for( int t = 0; t < 19; t++ )
		{
			if( testTimesBest[t] > testTimes[t] )
				testTimesBest[t] = testTimes[t];
		}

		printf("\n");
	}
	
	printf("Basic          %.3f    %.3f    %.3f%s\n", testTimesOrig[ 0], testTimesOrig2[ 0], testTimesBest[ 0], testTimesBest[ 0] < testTimesOrig2[ 0] ? " +" : " -"); 
	printf("Basic2         %.3f    %.3f    %.3f%s\n", testTimesOrig[ 1], testTimesOrig2[ 1], testTimesBest[ 1], testTimesBest[ 1] < testTimesOrig2[ 1] ? " +" : " -"); 
	printf("Call           %.3f    %.3f    %.3f%s\n", testTimesOrig[ 2], testTimesOrig2[ 2], testTimesBest[ 2], testTimesBest[ 2] < testTimesOrig2[ 2] ? " +" : " -"); 
	printf("Call2          %.3f    %.3f    %.3f%s\n", testTimesOrig[ 3], testTimesOrig2[ 3], testTimesBest[ 3], testTimesBest[ 3] < testTimesOrig2[ 3] ? " +" : " -"); 
	printf("Fib            %.3f    %.3f    %.3f%s\n", testTimesOrig[ 4], testTimesOrig2[ 4], testTimesBest[ 4], testTimesBest[ 4] < testTimesOrig2[ 4] ? " +" : " -"); 
	printf("Int            %.3f    %.3f    %.3f%s\n", testTimesOrig[ 5], testTimesOrig2[ 5], testTimesBest[ 5], testTimesBest[ 5] < testTimesOrig2[ 5] ? " +" : " -"); 
	printf("Intf           %.3f    %.3f    %.3f%s\n", testTimesOrig[ 6], testTimesOrig2[ 6], testTimesBest[ 6], testTimesBest[ 6] < testTimesOrig2[ 6] ? " +" : " -"); 
	printf("Mthd           %.3f    %.3f    %.3f%s\n", testTimesOrig[ 7], testTimesOrig2[ 7], testTimesBest[ 7], testTimesBest[ 7] < testTimesOrig2[ 7] ? " +" : " -"); 
	printf("String         %.3f    %.3f    %.3f%s\n", testTimesOrig[ 8], testTimesOrig2[ 8], testTimesBest[ 8], testTimesBest[ 8] < testTimesOrig2[ 8] ? " +" : " -"); 
	printf("String2        %.3f    %.3f    %.3f%s\n", testTimesOrig[ 9], testTimesOrig2[ 9], testTimesBest[ 9], testTimesBest[ 9] < testTimesOrig2[ 9] ? " +" : " -"); 
	printf("StringPooled   %.3f    %.3f    %.3f%s\n", testTimesOrig[10], testTimesOrig2[10], testTimesBest[10], testTimesBest[10] < testTimesOrig2[10] ? " +" : " -"); 
	printf("ThisProp       %.3f    %.3f    %.3f%s\n", testTimesOrig[11], testTimesOrig2[11], testTimesBest[11], testTimesBest[11] < testTimesOrig2[11] ? " +" : " -"); 
	printf("Vector3        %.3f    %.3f    %.3f%s\n", testTimesOrig[12], testTimesOrig2[12], testTimesBest[12], testTimesBest[12] < testTimesOrig2[12] ? " +" : " -"); 
	printf("Assign.1       %.3f    %.3f    %.3f%s\n", testTimesOrig[13], testTimesOrig2[13], testTimesBest[13], testTimesBest[13] < testTimesOrig2[13] ? " +" : " -"); 
	printf("Assign.2       %.3f    %.3f    %.3f%s\n", testTimesOrig[14], testTimesOrig2[14], testTimesBest[14], testTimesBest[14] < testTimesOrig2[14] ? " +" : " -"); 
	printf("Assign.3       %.3f    %.3f    %.3f%s\n", testTimesOrig[15], testTimesOrig2[15], testTimesBest[15], testTimesBest[15] < testTimesOrig2[15] ? " +" : " -"); 
	printf("Assign.4       %.3f    %.3f    %.3f%s\n", testTimesOrig[16], testTimesOrig2[16], testTimesBest[16], testTimesBest[16] < testTimesOrig2[16] ? " +" : " -"); 
	printf("Assign.5       %.3f    %.3f    %.3f%s\n", testTimesOrig[17], testTimesOrig2[17], testTimesBest[17], testTimesBest[17] < testTimesOrig2[17] ? " +" : " -"); 
	printf("Array          %.3f    %.3f    %.3f%s\n", testTimesOrig[18], testTimesOrig2[18], testTimesBest[18], testTimesBest[18] < testTimesOrig2[18] ? " +" : " -"); 

	printf("--------------------------------------------\n");
	printf("Press any key to quit.\n");
#if defined(WIN32)
	while(!_getch());
#endif
	return 0;
}
