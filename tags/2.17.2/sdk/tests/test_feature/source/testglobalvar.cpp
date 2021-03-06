#include "utils.h"
using std::string;

static const char * const TESTNAME = "TestGlobalVar";
static const char *script1 = "float global = func() * g_f * 2.0f;";
static const char *script2 = "float global = 1.0f;";

static void func(asIScriptGeneric *gen)
{
	gen->SetReturnFloat(3.0f);
}

static float cnst = 2.0f;
static std::string g_str = "test";

static const char *script3 =
"float f = 2;                 \n"
"string str = \"test\";       \n"
"void TestGlobalVar()         \n"
"{                            \n"
"  float a = f + g_f;         \n"
"  string s = str + g_str;    \n"
"  g_f = a;                   \n"
"  f = a;                     \n"
"  g_str = s;                 \n"
"  str = s;                   \n"
"}                            \n";

static const char *script4 =
"const double gca=12;   \n"
"const double gcb=5;    \n"
"const double gcc=35.2; \n"
"const double gcd=4;    \n"
"double a=12;   \n"
"double b=5;    \n"
"double c=35.2; \n"
"double d=4;    \n"
"void test()          \n"
"{                    \n"
"  print(gca+\"\\n\");  \n"
"  print(gcb+\"\\n\");  \n"
"  print(gcc+\"\\n\");  \n"
"  print(gcd+\"\\n\");  \n"
"  print(a+\"\\n\");  \n"
"  print(b+\"\\n\");  \n"
"  print(c+\"\\n\");  \n"
"  print(d+\"\\n\");  \n"
"const double lca=12;   \n"
"const double lcb=5;    \n"
"const double lcc=35.2; \n"
"const double lcd=4;    \n"
"double la=12;   \n"
"double lb=5;    \n"
"double lc=35.2; \n"
"double ld=4;    \n"
"  print(lca+\"\\n\");  \n"
"  print(lcb+\"\\n\");  \n"
"  print(lcc+\"\\n\");  \n"
"  print(lcd+\"\\n\");  \n"
"  print(la+\"\\n\");  \n"
"  print(lb+\"\\n\");  \n"
"  print(lc+\"\\n\");  \n"
"  print(ld+\"\\n\");  \n"
"}                    \n";

static const char *script5 =
"uint OFLAG_BSP = uint(1024);";

static const char *script6 = 
"string @handle = @object; \n"
"string  object = \"t\";   \n";

void print(asIScriptGeneric *gen)
{
	std::string s = ((CScriptString*)gen->GetArgAddress(0))->buffer;

	if( s != "12\n" && 
		s != "5\n" &&
		s != "35.2\n" &&
		s != "4\n" )
		printf("Error....\n");

//	printf(s.c_str());
}

bool TestGlobalVar()
{
	bool ret = false;
	asIScriptEngine *engine;
	COutStream out;
	int r;

	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	RegisterScriptString_Generic(engine);

	engine->RegisterGlobalFunction("float func()", asFUNCTION(func), asCALL_GENERIC);
	engine->RegisterGlobalProperty("float g_f", &cnst);
	engine->RegisterGlobalProperty("string g_str", &g_str);
	engine->RegisterGlobalFunction("void print(string &in)", asFUNCTION(print), asCALL_GENERIC);

	asIScriptModule *mod = engine->GetModule("a", asGM_ALWAYS_CREATE);
	mod->AddScriptSection(TESTNAME, script1, strlen(script1), 0);
	// This should fail, since we are trying to call a function in the initialization
	if( mod->Build() >= 0 )
	{
		printf("%s: build erronously returned success\n", TESTNAME);
		ret = true;
	}

	mod->AddScriptSection("script", script2, strlen(script2), 0);
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
	if( mod->Build() < 0 )
	{
		printf("%s: build failed\n", TESTNAME);
		ret = true;
	}

	mod->AddScriptSection("script", script3, strlen(script3), 0);
	if( mod->Build() < 0 )
	{
		printf("%s: build failed\n", TESTNAME);
		ret = true;
	}

	engine->ExecuteString("a", "TestGlobalVar()");

	float *f = (float*)mod->GetAddressOfGlobalVar(mod->GetGlobalVarIndexByDecl("float f"));
	string *str = (string*)mod->GetAddressOfGlobalVar(mod->GetGlobalVarIndexByDecl("string str"));

	float fv = *f; UNUSED_VAR(fv);
	string strv = *str;

	mod->ResetGlobalVars();

	f = (float*)mod->GetAddressOfGlobalVar(mod->GetGlobalVarIndexByDecl("float f"));
	str = (string*)mod->GetAddressOfGlobalVar(mod->GetGlobalVarIndexByDecl("string str"));

	if( !CompareDouble(*f, 2) || *str != "test" )
	{
		printf("%s: Failed to reset the module\n", TESTNAME);
		ret = true;
	}

	// Use another module so that we can test that the variable id is correct even for multiple modules
	mod = engine->GetModule("b", asGM_ALWAYS_CREATE);
	mod->AddScriptSection("script", script4);
	if( mod->Build() < 0 )
	{
		printf("%s: build failed\n", TESTNAME);
		ret = true;
	}

	int c = engine->GetModule("b")->GetGlobalVarCount();
	if( c != 8 ) ret = true;
	double d;
	d = *(double*)engine->GetModule("b")->GetAddressOfGlobalVar(0); 
	if( !CompareDouble(d, 12) ) ret = true;
	d = *(double*)engine->GetModule("b")->GetAddressOfGlobalVar(engine->GetModule("b")->GetGlobalVarIndexByName("gcb")); 
	if( !CompareDouble(d, 5) ) ret = true;
	d = *(double*)engine->GetModule("b")->GetAddressOfGlobalVar(engine->GetModule("b")->GetGlobalVarIndexByDecl("const double gcc")); 
	if( !CompareDouble(d, 35.2) ) ret = true;
	d = *(double*)engine->GetModule("b")->GetAddressOfGlobalVar(3); 
	if( !CompareDouble(d, 4) ) ret = true;
	
	engine->ExecuteString("b", "test()");

	engine->Release();

	//--------------------

	asIScriptArray *gPacketData = 0;
	unsigned int gPacketLength = 0;

	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	r = engine->RegisterGlobalProperty("uint gPacketLength", &gPacketLength); assert( r >= 0 );
	r = engine->RegisterGlobalProperty("uint8[] @gPacketData", &gPacketData); assert( r >= 0 );
	engine->Release();

	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	r = engine->RegisterGlobalProperty("uint8[] @gPacketData", &gPacketData); assert( r >= 0 );
	r = engine->RegisterGlobalProperty("uint gPacketLength", &gPacketLength); assert( r >= 0 );
	engine->Release();

	//-----------------------
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
	mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
	mod->AddScriptSection("script", script5);
	r = mod->Build(); 
	if( r < 0 )
		ret = true;
	engine->Release();

	//--------------------------
	// Make sure GetGlobalVarPointer is able to handle objects and pointers correctly
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	RegisterScriptString(engine);
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
	mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
	mod->AddScriptSection("script", script6);
	r = mod->Build();
	if( r < 0 )
		ret = true;
	else
	{
		CScriptString *object = (CScriptString*)engine->GetModule(0)->GetAddressOfGlobalVar(engine->GetModule(0)->GetGlobalVarIndexByName("object"));
		CScriptString **handle = (CScriptString**)engine->GetModule(0)->GetAddressOfGlobalVar(engine->GetModule(0)->GetGlobalVarIndexByName("handle"));
		if( *handle != object )
			ret = true;
		if( object->buffer != "t" )
			ret = true;
	}
	engine->Release();

	//----------------------
	// Global object handles initialized with other global variables
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
		engine->RegisterGlobalFunction("void assert(bool)", asFUNCTION(Assert), asCALL_GENERIC);

		const char *script =
			"class b {}         \n"
			"b a;               \n"
			"b @h = @a;         \n"
			"b@[] v = {@a, @h}; \n";

		asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script);
		r = mod->Build();
		if( r < 0 )
			ret = true;
		else
		{
			r = engine->ExecuteString(0, "assert(@a == @h); assert(v.length() == 2); assert(@v[0] == @v[1]);");
			if( r != asEXECUTION_FINISHED )
				ret = true;
		}		

		engine->Release();
	}

	//-----------
	// It's not valid to attempt registering global properties with values
	{
		CBufferedOutStream bout;
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(CBufferedOutStream,Callback), &bout, asCALL_THISCALL);
		int r = engine->RegisterGlobalProperty("const int value = 3345;", 0);
		if( r >= 0 )
			ret = true;
		if( bout.buffer != "Property (1, 17) : Error   : Expected '<end of file>'\n" )
		{
			ret = true;
			printf(bout.buffer.c_str());
		}
		engine->Release();
	}

	//-------------
	// It is possible to access global variables through scope operator if local variables have the same name
	{
		COutStream out;
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
		engine->RegisterGlobalFunction("void assert(bool)", asFUNCTION(Assert), asCALL_GENERIC); 
		const char *script = 
			"float g = 0; \n";
		asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script);
		mod->Build();
		r = engine->ExecuteString(0, "float g = 0; g = 1; ::g = 2; assert( g == 1 ); assert( ::g == 2 );");
		if( r != asEXECUTION_FINISHED )
		{
			ret = true;
		}
		engine->Release();
	}

	//-----------------
	// It is possible to turn off initialization of global variables
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->RegisterObjectType("object", sizeof(int), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_PRIMITIVE);
		engine->RegisterObjectBehaviour("object", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(0), asCALL_GENERIC);

		// Without this, the application would crash as the engine tries
		// to initialize the variable with the invalid function pointer
		engine->SetEngineProperty(asEP_INIT_GLOBAL_VARS_AFTER_BUILD, false);

		const char *script = "object o;";
		asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection("", script);
		r = mod->Build();
		if( r < 0 )
			ret = true;

		engine->Release();
	}

	//-----------------------
	// variables of primitive type should be initialized before variables of complex type
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);
		engine->RegisterGlobalFunction("void assert(bool)", asFUNCTION(Assert), asCALL_GENERIC);

		const char *script = "class MyObj { MyObj() { a = g_a; b = g_b; } int a; int b; } \n"
			                 "int g_a = 314 + g_b; \n" // This forces g_a to be compiled after g_b
							 "MyObj obj(); \n"         // obj should be compiled last
							 "int g_b = 42; \n";

		asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection("", script);
		r = mod->Build();
		if( r < 0 )
			ret = true;

		r = engine->ExecuteString(0, "assert( obj.a == 314+42 ); \n"
			                         "assert( obj.b == 42 ); \n");
		if( r != asEXECUTION_FINISHED )
		{
			ret = true;
		}

		engine->Release();
	}

	//-----------------------
	// variables of object type that access other global objects in constructor will throw null-pointer exception
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);
		engine->RegisterGlobalFunction("void assert(bool)", asFUNCTION(Assert), asCALL_GENERIC);

		const char *script = "class A { void Access() {} } \n"
			                 "class B { B() { g_a.Access(); g_c.Access(); } } \n"
			                 "A g_a; \n"
							 "B g_b; \n" // g_b accesses both g_a and g_c
							 "A g_c; \n";

		asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection("", script);
		r = mod->Build();
		if( r >= 0 )
			ret = true;

		engine->Release();
	}

	return ret;
}

