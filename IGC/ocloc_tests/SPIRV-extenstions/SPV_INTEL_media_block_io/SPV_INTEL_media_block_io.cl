// Tests targets spirv-extension: SPV_INTEL_media_block_io
// REQUIRES: dg2-supported, regkeys, llvm-16-plus
// RUN: ocloc compile -file %s -options "-cl-std=CL2.0 -igc_opts 'PrintToConsole=1 PrintBefore=EmitPass'" -internal_options "-cl-intel-use-bindless-mode -cl-intel-use-bindless-advanced-mode" -device dg2 2>&1 | FileCheck %s --check-prefix=CHECK-LLVM

uchar __attribute__((overloadable)) intel_sub_group_media_block_read_uc(int2 src_offset, int width, int height, read_only image2d_t image);
uchar2 __attribute__((overloadable)) intel_sub_group_media_block_read_uc2(int2 src_offset, int width, int height, read_only image2d_t image);
uchar4 __attribute__((overloadable)) intel_sub_group_media_block_read_uc4(int2 src_offset, int width, int height, read_only image2d_t image);
uchar8 __attribute__((overloadable)) intel_sub_group_media_block_read_uc8(int2 src_offset, int width, int height, read_only image2d_t image);
uchar16 __attribute__((overloadable)) intel_sub_group_media_block_read_uc16(int2 src_offset, int width, int height, read_only image2d_t image);

ushort __attribute__((overloadable)) intel_sub_group_media_block_read_us(int2 src_offset, int width, int height, read_only image2d_t image);
ushort2 __attribute__((overloadable)) intel_sub_group_media_block_read_us2(int2 src_offset, int width, int height, read_only image2d_t image);
ushort4 __attribute__((overloadable)) intel_sub_group_media_block_read_us4(int2 src_offset, int width, int height, read_only image2d_t image);
ushort8 __attribute__((overloadable)) intel_sub_group_media_block_read_us8(int2 src_offset, int width, int height, read_only image2d_t image);
ushort16 __attribute__((overloadable)) intel_sub_group_media_block_read_us16(int2 src_offset, int width, int height, read_only image2d_t image);

uint __attribute__((overloadable)) intel_sub_group_media_block_read_ui(int2 src_offset, int width, int height, read_only image2d_t image);
uint2 __attribute__((overloadable)) intel_sub_group_media_block_read_ui2(int2 src_offset, int width, int height, read_only image2d_t image);
uint4 __attribute__((overloadable)) intel_sub_group_media_block_read_ui4(int2 src_offset, int width, int height, read_only image2d_t image);
uint8 __attribute__((overloadable)) intel_sub_group_media_block_read_ui8(int2 src_offset, int width, int height, read_only image2d_t image);

uchar __attribute__((overloadable)) intel_sub_group_media_block_read_uc(int2 src_offset, int width, int height, read_write image2d_t image);
uchar2 __attribute__((overloadable)) intel_sub_group_media_block_read_uc2(int2 src_offset, int width, int height, read_write image2d_t image);
uchar4 __attribute__((overloadable)) intel_sub_group_media_block_read_uc4(int2 src_offset, int width, int height, read_write image2d_t image);
uchar8 __attribute__((overloadable)) intel_sub_group_media_block_read_uc8(int2 src_offset, int width, int height, read_write image2d_t image);
uchar16 __attribute__((overloadable)) intel_sub_group_media_block_read_uc16(int2 src_offset, int width, int height, read_write image2d_t image);

ushort __attribute__((overloadable)) intel_sub_group_media_block_read_us(int2 src_offset, int width, int height, read_write image2d_t image);
ushort2 __attribute__((overloadable)) intel_sub_group_media_block_read_us2(int2 src_offset, int width, int height, read_write image2d_t image);
ushort4 __attribute__((overloadable)) intel_sub_group_media_block_read_us4(int2 src_offset, int width, int height, read_write image2d_t image);
ushort8 __attribute__((overloadable)) intel_sub_group_media_block_read_us8(int2 src_offset, int width, int height, read_write image2d_t image);
ushort16 __attribute__((overloadable)) intel_sub_group_media_block_read_us16(int2 src_offset, int width, int height, read_write image2d_t image);

uint __attribute__((overloadable)) intel_sub_group_media_block_read_ui(int2 src_offset, int width, int height, read_write image2d_t image);
uint2 __attribute__((overloadable)) intel_sub_group_media_block_read_ui2(int2 src_offset, int width, int height, read_write image2d_t image);
uint4 __attribute__((overloadable)) intel_sub_group_media_block_read_ui4(int2 src_offset, int width, int height, read_write image2d_t image);
uint8 __attribute__((overloadable)) intel_sub_group_media_block_read_ui8(int2 src_offset, int width, int height, read_write image2d_t image);

void __attribute__((overloadable)) intel_sub_group_media_block_write_uc(int2 src_offset, int width, int height, uchar pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_uc2(int2 src_offset, int width, int height, uchar2 pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_uc4(int2 src_offset, int width, int height, uchar4 pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_uc8(int2 src_offset, int width, int height, uchar8 pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_uc16(int2 src_offset, int width, int height, uchar16 pixels, write_only image2d_t image);

void __attribute__((overloadable)) intel_sub_group_media_block_write_us(int2 src_offset, int width, int height, ushort pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_us2(int2 src_offset, int width, int height, ushort2 pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_us4(int2 src_offset, int width, int height, ushort4 pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_us8(int2 src_offset, int width, int height, ushort8 pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_us16(int2 src_offset, int width, int height, ushort16 pixels, write_only image2d_t image);

void __attribute__((overloadable)) intel_sub_group_media_block_write_ui(int2 src_offset, int width, int height, uint pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_ui2(int2 src_offset, int width, int height, uint2 pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_ui4(int2 src_offset, int width, int height, uint4 pixels, write_only image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_ui8(int2 src_offset, int width, int height, uint8 pixels, write_only image2d_t image);

void __attribute__((overloadable)) intel_sub_group_media_block_write_uc(int2 src_offset, int width, int height, uchar pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_uc2(int2 src_offset, int width, int height, uchar2 pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_uc4(int2 src_offset, int width, int height, uchar4 pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_uc8(int2 src_offset, int width, int height, uchar8 pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_uc16(int2 src_offset, int width, int height, uchar16 pixels, read_write image2d_t image);

void __attribute__((overloadable)) intel_sub_group_media_block_write_us(int2 src_offset, int width, int height, ushort pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_us2(int2 src_offset, int width, int height, ushort2 pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_us4(int2 src_offset, int width, int height, ushort4 pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_us8(int2 src_offset, int width, int height, ushort8 pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_us16(int2 src_offset, int width, int height, ushort16 pixels, read_write image2d_t image);

void __attribute__((overloadable)) intel_sub_group_media_block_write_ui(int2 src_offset, int width, int height, uint pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_ui2(int2 src_offset, int width, int height, uint2 pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_ui4(int2 src_offset, int width, int height, uint4 pixels, read_write image2d_t image);
void __attribute__((overloadable)) intel_sub_group_media_block_write_ui8(int2 src_offset, int width, int height, uint8 pixels, read_write image2d_t image);

#pragma OPENCL EXTENSION cl_intel_subgroups : enable
__kernel void intel_media_block_test(int2 edgeCoord, __read_only image2d_t src_luma_image,
                         __write_only image2d_t dst_luma_image) {
  // Byte sized read operations
  uchar uc =
      intel_sub_group_media_block_read_uc(edgeCoord, 4, 16, src_luma_image);
  uchar2 uc2 =
      intel_sub_group_media_block_read_uc2(edgeCoord, 4, 16, src_luma_image);
  uchar4 uc4 =
      intel_sub_group_media_block_read_uc4(edgeCoord, 4, 16, src_luma_image);
  uchar8 uc8 =
      intel_sub_group_media_block_read_uc8(edgeCoord, 4, 16, src_luma_image);
  uchar16 uc16 =
      intel_sub_group_media_block_read_uc16(edgeCoord, 4, 16, src_luma_image);

  // Word sized read operations
  ushort us =
      intel_sub_group_media_block_read_us(edgeCoord, 4, 16, src_luma_image);
  ushort2 us2 =
      intel_sub_group_media_block_read_us2(edgeCoord, 4, 16, src_luma_image);
  ushort4 us4 =
      intel_sub_group_media_block_read_us4(edgeCoord, 4, 16, src_luma_image);
  ushort8 us8 =
      intel_sub_group_media_block_read_us8(edgeCoord, 4, 16, src_luma_image);
  ushort16 us16 =
      intel_sub_group_media_block_read_us16(edgeCoord, 4, 16, src_luma_image);

  // Double Word (DWORD) sized read operations
  uint ui =
      intel_sub_group_media_block_read_ui(edgeCoord, 4, 16, src_luma_image);
  uint2 ui2 =
      intel_sub_group_media_block_read_ui2(edgeCoord, 4, 16, src_luma_image);
  uint4 ui4 =
      intel_sub_group_media_block_read_ui4(edgeCoord, 4, 16, src_luma_image);
  uint8 ui8 =
      intel_sub_group_media_block_read_ui8(edgeCoord, 4, 16, src_luma_image);

  // Byte sized write operations
  intel_sub_group_media_block_write_uc(edgeCoord, 4, 16, uc, dst_luma_image);
  intel_sub_group_media_block_write_uc2(edgeCoord, 4, 16, uc2, dst_luma_image);
  intel_sub_group_media_block_write_uc4(edgeCoord, 4, 16, uc4, dst_luma_image);
  intel_sub_group_media_block_write_uc8(edgeCoord, 4, 16, uc8, dst_luma_image);
  intel_sub_group_media_block_write_uc16(edgeCoord, 4, 16, uc16, dst_luma_image);

  // Word sized write operations
  intel_sub_group_media_block_write_us(edgeCoord, 4, 16, us, dst_luma_image);
  intel_sub_group_media_block_write_us2(edgeCoord, 4, 16, us2, dst_luma_image);
  intel_sub_group_media_block_write_us4(edgeCoord, 4, 16, us4, dst_luma_image);
  intel_sub_group_media_block_write_us8(edgeCoord, 4, 16, us8, dst_luma_image);
  intel_sub_group_media_block_write_us16(edgeCoord, 4, 16, us16, dst_luma_image);

  // Double word (DWORD) sized write operations
  intel_sub_group_media_block_write_ui(edgeCoord, 4, 16, ui, dst_luma_image);
  intel_sub_group_media_block_write_ui2(edgeCoord, 4, 16, ui2, dst_luma_image);
  intel_sub_group_media_block_write_ui4(edgeCoord, 4, 16, ui4, dst_luma_image);
  intel_sub_group_media_block_write_ui8(edgeCoord, 4, 16, ui8, dst_luma_image);
}

// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uchar
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uchar2
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uchar4
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uchar8
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uchar16
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_ushort
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_ushort2
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_ushort4
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_ushort8
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_ushort16
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uint
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uint2
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uint4
// CHECK-LLVM-DAG: __builtin_IB_media_block_read_uint8
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uchar
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uchar2
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uchar4
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uchar8
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uchar16
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_ushort
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_ushort2
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_ushort4
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_ushort8
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_ushort16
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uint
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uint2
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uint4
// CHECK-LLVM-DAG: __builtin_IB_media_block_write_uint8
