/*========================== begin_copyright_notice ============================

Copyright (C) 2017-2023 Intel Corporation

SPDX-License-Identifier: MIT

============================= end_copyright_notice ===========================*/

#ifndef _KERNEL_ANNOTATIONS_H_
#define _KERNEL_ANNOTATIONS_H_

#include <string>
#include "AdaptorOCL/ocl_igc_shared/executable_format/patch_list.h"

namespace iOpenCL
{

enum POINTER_ADDRESS_SPACE
{
    KERNEL_ARGUMENT_ADDRESS_SPACE_INVALID,

    KERNEL_ARGUMENT_ADDRESS_SPACE_GLOBAL,
    KERNEL_ARGUMENT_ADDRESS_SPACE_CONSTANT,
    KERNEL_ARGUMENT_ADDRESS_SPACE_PRIVATE,
    KERNEL_ARGUMENT_ADDRESS_SPACE_LOCAL,
    KERNEL_ARGUMENT_ADDRESS_SPACE_DEVICE_QUEUE,

    ADDRESS_SPACE_INTERNAL_DEFAULT_DEVICE_QUEUE,
    ADDRESS_SPACE_INTERNAL_EVENT_POOL,
    ADDRESS_SPACE_INTERNAL_PRINTF,

    NUM_KERNEL_ARGUMENT_ADDRESS_SPACE
};

enum TYPE_FORMAT
{
    TYPE_FORMAT_INVALID,

    TYPE_FORMAT_FLOAT,
    TYPE_FORMAT_SINT,
    TYPE_FORMAT_UINT,

    NUM_TYPE_FORMATS
};


struct KernelAnnotation
{
    virtual ~KernelAnnotation() = default;
};


// Generated by the frontend
struct KernelArgumentAnnotation : KernelAnnotation
{
    DWORD ArgumentNumber;
};


// Generated by frontend - completed by backend
struct PointerArgumentAnnotation : KernelArgumentAnnotation
{
    POINTER_ADDRESS_SPACE  AddressSpace;

    bool   IsStateless;
    DWORD  BindingTableIndex;
    DWORD  PayloadPosition;
    DWORD  PayloadSizeInBytes;
    DWORD  LocationIndex;
    DWORD  LocationCount;
    DWORD  SecondPayloadSizeInBytes;
    bool   IsEmulationArgument;
    bool   IsBindlessAccess;
    bool   HasStatefulAccess;

    static bool compare( const PointerArgumentAnnotation* a, const PointerArgumentAnnotation* b )
    {
        return ( a->PayloadPosition < b->PayloadPosition );
    }
};

// Should be used for __local address space pointer arguments, as it
// contains rather different fields from PointerArgumentAnnotation
struct LocalArgumentAnnotation : KernelArgumentAnnotation
{
    DWORD  Alignment;
    DWORD  PayloadPosition;
    DWORD  PayloadSizeInBytes;
    DWORD  LocationIndex;
    DWORD  LocationCount;
};


// Generated by frontend - completed by backend
struct PointerInputAnnotation : KernelAnnotation
{
    POINTER_ADDRESS_SPACE  AddressSpace;

    bool   IsStateless;
    DWORD  BindingTableIndex;
    DWORD  PayloadPosition;
    DWORD  PayloadSizeInBytes;
    DWORD  ArgumentNumber;
    bool   HasStatefulAccess;
};


struct PrivateInputAnnotation : PointerInputAnnotation {
    DWORD PerThreadPrivateMemorySize;
};

// Generated by frontend - completed by backend
struct ConstantArgumentAnnotation : KernelArgumentAnnotation
{
    DWORD  TypeSize;
    DWORD  Offset;

    DWORD  PayloadPosition;
    DWORD  PayloadSizeInBytes;
    DWORD  LocationIndex;
    DWORD  LocationCount;
    bool   IsEmulationArgument;
};


// Generated by frontend - completed by backend
struct ConstantInputAnnotation : KernelAnnotation
{
    // ConstantInputAnnotations, while not being argument annotations,
    // may refer to arguments.
    DWORD ArgumentNumber;

    DWORD TypeSize;
    DWORD ConstantType;
    DWORD Offset;

    DWORD PayloadPosition;
    DWORD PayloadSizeInBytes;
    DWORD LocationIndex;
    DWORD LocationCount;
};


// Generated by frontend - completed by backend
struct ImageArgumentAnnotation : KernelArgumentAnnotation
{
    IMAGE_MEMORY_OBJECT_TYPE  ImageType;
    bool   Writeable;
    bool   IsFixedBindingTableIndex;
    DWORD  BindingTableIndex;
    DWORD  LocationIndex;
    DWORD  LocationCount;
    DWORD  PayloadPosition;
    bool   AccessedByIntCoords;
    bool   AccessedByFloatCoords;
    bool   IsBindlessAccess;
    bool   IsEmulationArgument;
};

struct SamplerInputAnnotation : KernelAnnotation
{
    SAMPLER_OBJECT_TYPE  SamplerType;

    DWORD  SamplerTableIndex;

    bool                            NormalizedCoords;
    SAMPLER_MAPFILTER_TYPE          MagFilterType;
    SAMPLER_MAPFILTER_TYPE          MinFilterType;
    SAMPLER_MIPFILTER_TYPE          MipFilterType;
    SAMPLER_TEXTURE_ADDRESS_MODE    TCXAddressMode;
    SAMPLER_TEXTURE_ADDRESS_MODE    TCYAddressMode;
    SAMPLER_TEXTURE_ADDRESS_MODE    TCZAddressMode;
    SAMPLER_COMPARE_FUNC_TYPE       CompareFunc;

    float                           BorderColorR;
    float                           BorderColorG;
    float                           BorderColorB;
    float                           BorderColorA;
};

// Generated by frontend - completed by backend
struct SamplerArgumentAnnotation : KernelArgumentAnnotation
{
    // Generated by the front end
    SAMPLER_OBJECT_TYPE  SamplerType;

    // Generated by the backend
    DWORD  SamplerTableIndex;
    DWORD  LocationIndex;
    DWORD  LocationCount;
    DWORD  PayloadPosition;
    bool   IsBindlessAccess;
    bool   IsEmulationArgument;
};

// Annotation for format string of printf
struct PrintfStringAnnotation : KernelAnnotation
{
    DWORD  Index;
    DWORD  StringSize;
    char  *StringData;
};

// Annotation for printf output buffer.
struct PrintfBufferAnnotation : KernelArgumentAnnotation
{
    DWORD  Index;
    DWORD  DataSize;
    DWORD  PayloadPosition;
};

// Annotation for sync buffer.
struct SyncBufferAnnotation : KernelArgumentAnnotation
{
    DWORD DataSize;
    DWORD PayloadPosition;
    bool  HasStatefulAccess;
};

struct RTGlobalBufferAnnotation : KernelArgumentAnnotation
{
    DWORD DataSize;
    DWORD PayloadPosition;
    bool  HasStatefulAccess;
};

// Generated by front end
struct KernelConstantRegisterAnnotation
{
    DWORD Index;
    DWORD Channel;
};

struct InitConstantAnnotation
{
    std::vector<unsigned char> InlineData;
    int Alignment;
    size_t AllocSize;
};

struct InitGlobalAnnotation
{
    std::vector<unsigned char> InlineData;
    int Alignment;
    size_t AllocSize;
};

struct ConstantPointerAnnotation
{
    unsigned PointerBufferIndex;
    unsigned PointerOffset;
    unsigned PointeeAddressSpace;
    unsigned PointeeBufferIndex;
};

struct GlobalPointerAnnotation
{
    unsigned PointerBufferIndex;
    unsigned PointerOffset;
    unsigned PointeeAddressSpace;
    unsigned PointeeBufferIndex;
};

struct ThreadPayload
{
    bool  HasLocalIDx = false;
    bool  HasLocalIDy = false;
    bool  HasLocalIDz = false;
    bool  HasGlobalIDOffset = false;
    bool  HasGroupID = false;
    bool  HasLocalID = false;
    bool  HasFlattenedLocalID = false;
    bool  CompiledForIndirectPayloadStorage = false;
    bool  UnusedPerThreadConstantPresent = false;
    bool  HasStageInGridOrigin = false;
    bool  HasStageInGridSize = false;
    uint32_t PassInlineDataSize = 0;
    uint32_t OffsetToSkipPerThreadDataLoad = 0;
    uint32_t OffsetToSkipSetFFIDGP = 0;
    bool     HasRTStackID = false;
    bool     generateLocalID = false;
    uint32_t emitLocalMask   = 0;
    uint32_t walkOrder       = 0;
    bool     tileY           = false;
};

struct ExecutionEnvironment
{
    DWORD  CompiledSIMDSize                           = 0;
    DWORD  CompiledSubGroupsNumber                    = 0;
    //legacy design:hold all ScratchSpaceUsage
    //new design:   hold spillfill+callstack+GTPin
    //Todo: rename it to m_PerThreadScratchSpaceSlot0
    DWORD  PerThreadScratchSpace                      = 0;
    //DWORD  PerThreadScratchUseGtpin                   = 0;
    //legacy design:not used
    //new design:   hold private memory used by shader if non-ZERO
    DWORD  PerThreadScratchSpaceSlot1                 = 0;
    // Size in bytes of the stateless memory requirement for allocating
    // private variables.
    // This field is added for zebin
    // The same information for path token based format is passed in
    // PrivateMemSizeAnnotation which will point to a PrivateInputAnnotation
    // that contains the size
    DWORD  PerThreadPrivateOnStatelessSize            = 0;
    //legacy design:not used
    //new design:   hold private memory used by shader if non-ZERO
    DWORD  SumFixedTGSMSizes                          = 0;
    bool   HasDeviceEnqueue                           = false;
    // for PVC+ targets this field preserves the number of barriers
    uint32_t HasBarriers                              = 0;
    bool   HasSample                                  = false;
    bool   IsSingleProgramFlow                        = false;
    //DWORD  PerSIMDLanePrivateMemorySize               = 0;
    bool   HasFixedWorkGroupSize                      = false;
    bool   HasReadWriteImages                         = false;
    bool   DisableMidThreadPreemption                 = false;
    bool   IsInitializer                              = false;
    bool   IsFinalizer                                = false;
    bool   SubgroupIndependentForwardProgressRequired = false;
    bool   CompiledForGreaterThan4GBBuffers           = false;
    DWORD  FixedWorkgroupSize[3]                      = {};
    DWORD  NumGRFRequired                             = 0;
    DWORD  WorkgroupWalkOrder[3]                      = {};
    bool   HasGlobalAtomics                           = false;
    bool   UseBindlessMode                            = false;
    uint64_t SIMDInfo                                 = 0;
    bool  HasDPAS                                     = false;
    bool  HasRTCalls                                  = false;
    DWORD StatelessWritesCount                        = 0;
    DWORD IndirectStatelessCount                      = 0;
    DWORD numThreads                                  = 0;
    bool  HasStackCalls                               = false;
    bool  RequireDisableEUFusion                      = false;
    DWORD PerThreadSpillMemoryUsage                   = 0;
    DWORD PerThreadPrivateMemoryUsage                 = 0;
    bool HasLscStoresWithNonDefaultL1CacheControls    = false;
};

struct KernelTypeProgramBinaryInfo
{
    DWORD Type;
    std::string KernelName;
};

struct KernelArgumentInfoAnnotation
{
    std::string AddressQualifier;
    std::string AccessQualifier;
    std::string ArgumentName;
    std::string TypeName;
    std::string TypeQualifier;
};

struct StartGASAnnotation
{
    DWORD  Offset;
    DWORD  gpuPointerSizeInBytes;
};

struct WindowSizeGASAnnotation
{
    DWORD  Offset;
};

struct PrivateMemSizeAnnotation
{
    DWORD  Offset;
};

typedef std::vector<PointerInputAnnotation*>::const_iterator PointerInputIterator;
typedef std::vector<PointerArgumentAnnotation*>::const_iterator PointerArgumentIterator;
typedef std::vector<LocalArgumentAnnotation*>::const_iterator LocalArgumentIterator;
typedef std::vector<ConstantInputAnnotation*>::const_iterator ConstantInputIterator;
typedef std::vector<ConstantArgumentAnnotation*>::const_iterator ConstantArgumentIterator;
typedef std::vector<SamplerInputAnnotation*>::const_iterator SamplerInputIterator;
typedef std::vector<SamplerArgumentAnnotation*>::const_iterator SamplerArgumentIterator;
typedef std::vector<ImageArgumentAnnotation*>::const_iterator ImageArgumentIterator;
typedef std::vector<InitConstantAnnotation*>::const_iterator InitConstantIterator;
typedef std::vector<KernelArgumentInfoAnnotation*>::const_iterator KernelArgumentInfoIterator;
typedef std::vector<PrintfStringAnnotation*>::const_iterator PrintfStringIterator;

}

#endif
