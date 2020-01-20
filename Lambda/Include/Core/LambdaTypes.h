#pragma once

#ifdef ERROR
	#undef ERROR
#endif

namespace Lambda
{
	typedef char				int8;
	typedef short				int16;
	typedef int					int32;
	typedef long long			int64;
	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned int		uint32;
	typedef unsigned long long	uint64;

	enum EMessageBox
	{
		MESSAGE_BOX_UNKNOWN		= 0,
		MESSAGE_BOX_OK			= (1 << 1),
		MESSAGE_BOX_ERRORICON	= (1 << 2),
	};
}