;=========================== begin_copyright_notice ============================
;
; Copyright (C) 2024 Intel Corporation
;
; SPDX-License-Identifier: MIT
;
;============================ end_copyright_notice =============================

; Check __builtin_IB_get_address_mode is replaced with implicit arg %smpAddress
; for bindless sampler (offset is or'ed with 1).

; RUN: igc_opt -igc-image-func-resolution -S %s -o %t.ll
; RUN: FileCheck %s --input-file=%t.ll

%opencl.image2d_t = type opaque
%spirv.Sampler = type opaque

declare i32 @__builtin_IB_get_address_mode(i32)

define i32 @foo(%spirv.Sampler addrspace(2)* %sampler, i32 %smpAddress) nounwind {
  %1 = ptrtoint %spirv.Sampler addrspace(2)* %sampler to i64
  %2 = or i64 %1, 1
  %3 = trunc i64 %2 to i32
  %id = call i32 @__builtin_IB_get_address_mode(i32 %3)
  ret i32 %id
}

!igc.input.ir = !{!100}
!100 = !{!"ocl", i32 1, i32 2}

!igc.functions = !{!0}
!0 = !{i32 (%spirv.Sampler addrspace(2)*, i32)* @foo, !1}
!1 = !{!2, !3}
!2 = !{!"function_type", i32 0}
!3 = !{!"implicit_arg_desc", !4}
!4 = !{i32 30, !5}
!5 = !{!"explicit_arg_num", i32 0}

; CHECK:         ret i32 %smpAddress

; CHECK-NOT:     call i32 @__builtin_IB_get_address_mode(i32 %sampler)
