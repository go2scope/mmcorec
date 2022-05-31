#pragma once
#define VERSION_MAJOR_SDK							1
#define VERSION_MINOR_SDK							0
#define VERSION_REVISION_SDK						0

#define VERSION_SDK VERSION_MAJOR_SDK, VERSION_MINOR_SDK, VERSION_REVISION_SDK, 0

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define PACK_VER(x,y,z) ((x & 0xff) << 24) | ((y & 0xff) << 16) | (z & 0xffff)

#define VERSION_SDK_STR STR(VERSION_MAJOR_SDK) "." STR(VERSION_MINOR_SDK) "." STR(VERSION_REVISION_SDK) ".0"
#define G2S_VERSION PACK_VER(VERSION_MAJOR_SDK, VERSION_MINOR_SDK, VERSION_REVISION_SDK)

#define G2S_COMPANY									"Luminous Point LLC"
#define G2S_COPYRIGHT								"Copyright (C) 2022 by Luminous Point LLC"
#define G2S_PRODUCT									"MMCoreC"
