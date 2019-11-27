#pragma once
#if defined(LAMBDA_PLAT_MACOS)

int main(int argc, const char* argv[])
{
	LEngineParams params = {};
	params.ppCmdArgs		= nullptr;
	params.CmdArgsCount		= 0;
	return LambdaMain(params);
}
#endif