; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 5
; RUN: opt -passes='lto<O3>' -S < %s | FileCheck %s

; We should be able to propagate the constants from @parent to @child.

define void @parent(ptr %p) {
; CHECK-LABEL: define void @parent(
; CHECK-SAME: ptr captures(none) [[P:%.*]]) local_unnamed_addr #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:    tail call fastcc void @child(ptr [[P]])
; CHECK-NEXT:    ret void
;
  %c = alloca i32
  store i32 5, ptr %c
  %n = alloca i32
  store i32 1024, ptr %n
  call void @child(ptr %p, ptr %n, ptr %c)
  ret void
}

define internal void @child(ptr %p, ptr %n, ptr %c) noinline {
; CHECK-LABEL: define internal fastcc void @child(
; CHECK-SAME: ptr captures(none) [[P:%.*]]) unnamed_addr #[[ATTR1:[0-9]+]] {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br label %[[FOR_COND:.*]]
; CHECK:       [[FOR_COND]]:
; CHECK-NEXT:    [[I_0:%.*]] = phi i32 [ 0, %[[ENTRY]] ], [ [[INC:%.*]], %[[FOR_INC:.*]] ]
; CHECK-NEXT:    [[CMP_NOT:%.*]] = icmp eq i32 [[I_0]], 1024
; CHECK-NEXT:    br i1 [[CMP_NOT]], label %[[FOR_END:.*]], label %[[FOR_INC]]
; CHECK:       [[FOR_INC]]:
; CHECK-NEXT:    [[IDXPROM:%.*]] = zext nneg i32 [[I_0]] to i64
; CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds nuw i32, ptr [[P]], i64 [[IDXPROM]]
; CHECK-NEXT:    [[TMP0:%.*]] = load i32, ptr [[ARRAYIDX]], align 4
; CHECK-NEXT:    [[MUL:%.*]] = mul i32 [[TMP0]], 5
; CHECK-NEXT:    store i32 [[MUL]], ptr [[ARRAYIDX]], align 4
; CHECK-NEXT:    [[INC]] = add nuw nsw i32 [[I_0]], 1
; CHECK-NEXT:    br label %[[FOR_COND]]
; CHECK:       [[FOR_END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %for.cond

for.cond:
  %i.0 = phi i32 [ 0, %entry ], [ %inc, %for.inc ]
  %n.val = load i32, ptr %n
  %cmp = icmp ne i32 %i.0, %n.val
  br i1 %cmp, label %for.body, label %for.cond.cleanup

for.cond.cleanup:
  br label %for.end

for.body:
  %idxprom = sext i32 %i.0 to i64
  %arrayidx = getelementptr inbounds i32, ptr %p, i64 %idxprom
  %0 = load i32, ptr %arrayidx, align 4
  %c.val = load i32, ptr %c
  %mul = mul i32 %0, %c.val
  store i32 %mul, ptr %arrayidx, align 4
  br label %for.inc

for.inc:
  %inc = add nsw i32 %i.0, 1
  br label %for.cond

for.end:
  ret void
}

