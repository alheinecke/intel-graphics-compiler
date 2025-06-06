/*========================== begin_copyright_notice ============================

Copyright (C) 2023 Intel Corporation

SPDX-License-Identifier: MIT

============================= end_copyright_notice ===========================*/
// REQUIRES: dg2-supported, llvm-16-plus
// RUN: ocloc compile -file %s -device dg2 -options "-igc_opts 'EnableOpaquePointersBackend=1' -cl-std=CL2.0" 2>&1 | FileCheck %s

// CHECK: warning: incompatible pointer types

void func(generic int* generic_ptr)
{
    global int* global_ptr = to_global(generic_ptr);
    atomic_store(global_ptr, 1);
}

kernel void test(local int* ptr)
{
    func(ptr);
}
