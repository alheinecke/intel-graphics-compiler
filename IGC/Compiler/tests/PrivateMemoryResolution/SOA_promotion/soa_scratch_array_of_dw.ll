;=========================== begin_copyright_notice ============================
;
; Copyright (C) 2023 Intel Corporation
;
; SPDX-License-Identifier: MIT
;
;============================ end_copyright_notice =============================

; REQUIRES: regkeys
;
; RUN: igc_opt --typed-pointers --platformtgllp --igc-private-mem-resolution --regkey EnablePrivMemNewSOATranspose=2,NewSOATransposeForOpenCL=0  -S %s | FileCheck %s
;

; CHECK-LABEL: @test
;;
;; prolog in entry block. Get buffer's perThreadOffset
;;
; CHECK:       [[T00:%.*]] = call i16 @llvm.genx.GenISA.simdLaneId()
; CHECK:       [[simdLaneId:%.*]] = zext i16 [[T00]] to i32
; CHECK:       [[simdSize:%.*]] = call i32 @llvm.genx.GenISA.simdSize()
; CHECK:       [[r05:%.*]] = extractelement <8 x i32> %r0, i32 5
; CHECK:       [[perThreadOffset:%.*]] = and i32 [[r05]], -1024
;

target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024-n8:16:32"
target triple = "spir64-unknown-unknown"

; Function Attrs: nofree nosync nounwind
define spir_kernel void @test(i32 addrspace(1)* nocapture writeonly %d, <8 x i32> %r0, <8 x i32> %payloadHeader, <3 x i32> %enqueuedLocalSize, i16 %localIdX, i16 %localIdY, i16 %localIdZ, i8* nocapture readnone %privateBase) {
entry:
  %payloadHeader.scalar = extractelement <8 x i32> %payloadHeader, i64 0
  %enqueuedLocalSize.scalar = extractelement <3 x i32> %enqueuedLocalSize, i64 0
  %r0.scalar18 = extractelement <8 x i32> %r0, i64 1
  %pb = alloca [1024 x i32], align 4
  %tmp0 = mul i32 %enqueuedLocalSize.scalar, %r0.scalar18
  %localIdX3 = zext i16 %localIdX to i32
  %tmp1 = add i32 %tmp0, %localIdX3
  %ix = add i32 %tmp1, %payloadHeader.scalar
;;
;;  common code for each buffer
;;
; CHECK:       %idx = zext i32 %ix to i64
; CHECK:       [[T10_0:%.*]] = mul i32 [[simdSize]], 0
; CHECK:       [[T10:%.*]] = add i32 0, [[T10_0]]
; CHECK:       [[T11:%.*]] = mul i32 [[simdLaneId]], 4
; CHECK:       [[T12:%.*]] = add i32 [[T10]], [[T11]]
; CHECK:       [[bufferOffset:%.*]] = add {{.*}} i32 [[perThreadOffset]], [[T12]]
;;
;;   first store
;;
; CHECK:       [[T20:%.*]] = trunc i64 %idx to i32
; CHECK:       [[T21:%.*]] = mul nsw i32 [[T20]], 4
; CHECK:       [[T22:%.*]] = lshr i32 [[T21]], 2
; CHECK:       [[T23:%.*]] = mul i32 [[T22]], 4
; CHECK:       [[T24:%.*]] = mul i32 [[simdSize]], [[T23]]
; CHECK:       [[T25:%.*]] = add {{.*}} i32 [[bufferOffset]], [[T24]]
; CHECK:       [[GEPBase:%.*]] = inttoptr i32 [[T25]] to i32*
; CHECK:       [[staddr0:%.*]] = getelementptr i32, i32* [[GEPBase]], i32 0
; CHECK:       store i32 %ix, i32* [[staddr0]]
;;
  %idx = zext i32 %ix to i64
  %tmp2 = bitcast [1024 x i32]* %pb to i8*
  call void @llvm.lifetime.start.p0i8(i64 4096, i8* nonnull %tmp2)
  %staddr0 = getelementptr inbounds [1024 x i32], [1024 x i32]* %pb, i64 0, i64 %idx
  store i32 %ix, i32* %staddr0, align 4
;;
;;  second store
;;
; CHECK:      [[T31:%.*]] = mul i32 [[simdSize]], 128
;;
  %staddr1 = getelementptr inbounds [1024 x i32], [1024 x i32]* %pb, i64 0, i64 32
  %ixx2 = shl i32 %ix, 2
  store i32 %ixx2, i32* %staddr1, align 4
  %tmp3 = add i32 %ix, 1
  %tmp4 = and i32 %tmp3, 1023
  %idx1 = zext i32 %tmp4 to i64
;;
;; first load
;;
; CHECK:      [[T41:%.*]] = mul i32 [[simdSize]]
;
  %ldaddr0 = getelementptr inbounds [1024 x i32], [1024 x i32]* %pb, i64 0, i64 %idx1
  %v0 = load i32, i32* %ldaddr0, align 4
;;
;; second load
;;
; CHECK:      [[T51:%.*]] = mul i32 [[simdSize]], 128
;
  %ldaddr1 = getelementptr inbounds [1024 x i32], [1024 x i32]* %pb, i64 0, i64 32
  %v1 = load i32, i32* %ldaddr1, align 4
  %v2 = add nsw i32 %v0, %v1
  %arrayidx = getelementptr inbounds i32, i32 addrspace(1)* %d, i64 %idx
  store i32 %v2, i32 addrspace(1)* %arrayidx, align 4
  call void @llvm.lifetime.end.p0i8(i64 4096, i8* nonnull %tmp2)
;;
; CHECK:     ret
  ret void
}

; Function Attrs: argmemonly mustprogress nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture)

; Function Attrs: argmemonly mustprogress nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture)

!IGCMetadata = !{!0}
!igc.functions = !{!6}

!0 = !{!"ModuleMD", !1, !3}
!1 = !{!"compOpt", !2}
!2 = !{!"UseScratchSpacePrivateMemory", i1 true}
!3 = !{!"FuncMD", !4, !5}
!4 = !{!"FuncMDMap[1]", void (i32 addrspace(1)*, <8 x i32>, <8 x i32>, <3 x i32>, i16, i16, i16, i8*)* @test}
!5 = !{!"FuncMDValue[1]", !2}
!6 = !{void (i32 addrspace(1)*, <8 x i32>, <8 x i32>, <3 x i32>, i16, i16, i16, i8*)* @test, !408}
!408 = !{!409, !410}
!409 = !{!"function_type", i32 0}
!410 = !{!"implicit_arg_desc", !411, !412, !413, !414, !415, !416, !417}
!411 = !{i32 0}
!412 = !{i32 1}
!413 = !{i32 7}
!414 = !{i32 8}
!415 = !{i32 9}
!416 = !{i32 10}
!417 = !{i32 13}
