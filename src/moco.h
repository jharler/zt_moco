/**************************************************************************************************
** file: src\moco.h
**
** Primary functionality of moco (mo)del (co)nverter
**************************************************************************************************/

#ifndef __moco_h__
#define __moco_h__

// headers ========================================================================================================================================================================================

#include "game.h"


// types/enums/defines ============================================================================================================================================================================

enum MocoErrorType_Enum
{
	MocoErrorType_Success,
	MocoErrorType_InputFileNotFound,
	MocoErrorType_aiImportFileFailed,
	MocoErrorType_OutputFileWriteError,

	MocoErrorType_MAX,
};

// structs/classes ================================================================================================================================================================================

struct MocoConvertOptions
{
	bool   apply_root_transform = false;
	ztMat4 root_transform       = ztMat4::identity;

	bool  from_blender = false;
};

// external variables =============================================================================================================================================================================

// function prototypes ============================================================================================================================================================================

int mocoGetErrorMessage(MocoErrorType_Enum error_type, char *buffer, int buffer_size);

bool mocoConvertFile(MocoConvertOptions *options, const char *file_in, const char *file_out, MocoErrorType_Enum *error);


// inline functions ===============================================================================================================================================================================

#endif // include guard
