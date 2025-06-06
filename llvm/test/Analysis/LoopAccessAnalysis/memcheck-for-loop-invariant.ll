; RUN: opt -passes='print<access-info>' -disable-output  < %s 2>&1 | FileCheck %s

; Handle memchecks involving loop-invariant addresses:
;
; extern int *A, *b;
; for (i = 0; i < N; ++i) {
;  A[i] = b;
; }

target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"

; CHECK: Memory dependences are safe with run-time checks
; CHECK: Run-time memory checks:
; CHECK-NEXT: Check 0:
; CHECK-NEXT:   Comparing group
; CHECK-NEXT:     %arrayidxA = getelementptr inbounds i32, ptr %a, i64 %ind
; CHECK-NEXT:   Against group
; CHECK-NEXT:   ptr %b

define void @f(ptr %a, ptr %b) {
entry:
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %ind = phi i64 [ 0, %entry ], [ %inc, %for.body ]

  %arrayidxA = getelementptr inbounds i32, ptr %a, i64 %ind

  %loadB = load i32, ptr %b, align 4
  store i32 %loadB, ptr %arrayidxA, align 4

  %inc = add nuw nsw i64 %ind, 1
  %exitcond = icmp eq i64 %inc, 20
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body
  ret void
}
