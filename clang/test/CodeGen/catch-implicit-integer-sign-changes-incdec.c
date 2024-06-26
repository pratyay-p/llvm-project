// RUN: %clang_cc1 -emit-llvm %s -o - -triple x86_64-linux-gnu | FileCheck %s --check-prefix=CHECK-NOSANITIZE

// RUN: %clang_cc1 -fsanitize=implicit-integer-sign-change -fno-sanitize-recover=implicit-integer-sign-change -emit-llvm %s -o - -triple x86_64-linux-gnu | FileCheck %s -implicit-check-not="call void @__ubsan_handle_implicit_conversion" --check-prefixes=CHECK-SANITIZE,CHECK-SANITIZE-ANYRECOVER,CHECK-SANITIZE-NORECOVER,CHECK-SANITIZE-UNREACHABLE
// RUN: %clang_cc1 -fsanitize=implicit-integer-sign-change -fsanitize-recover=implicit-integer-sign-change -emit-llvm %s -o - -triple x86_64-linux-gnu | FileCheck %s -implicit-check-not="call void @__ubsan_handle_implicit_conversion" --check-prefixes=CHECK-SANITIZE,CHECK-SANITIZE-ANYRECOVER,CHECK-SANITIZE-RECOVER
// RUN: %clang_cc1 -fsanitize=implicit-integer-sign-change -fsanitize-trap=implicit-integer-sign-change -emit-llvm %s -o - -triple x86_64-linux-gnu | FileCheck %s -implicit-check-not="call void @__ubsan_handle_implicit_conversion" --check-prefixes=CHECK-SANITIZE,CHECK-SANITIZE-TRAP,CHECK-SANITIZE-UNREACHABLE

// CHECK-SANITIZE-ANYRECOVER-DAG: @[[INT:.*]] = {{.*}} c"'int'\00" }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[UNSIGNED_SHORT:.*]] = {{.*}} c"'unsigned short'\00" }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[LINE_100:.*]] = {{.*}}, i32 100, i32 11 }, ptr @[[INT]], ptr @[[UNSIGNED_SHORT]], i8 3, i32 0 }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[LINE_200:.*]] = {{.*}}, i32 200, i32 11 }, ptr @[[INT]], ptr @[[UNSIGNED_SHORT]], i8 3, i32 0 }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[LINE_300:.*]] = {{.*}}, i32 300, i32 10 }, ptr @[[INT]], ptr @[[UNSIGNED_SHORT]], i8 3, i32 0 }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[LINE_400:.*]] = {{.*}}, i32 400, i32 10 }, ptr @[[INT]], ptr @[[UNSIGNED_SHORT]], i8 3, i32 0 }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[SHORT:.*]] = {{.*}} c"'short'\00" }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[LINE_500:.*]] = {{.*}}, i32 500, i32 11 }, ptr @[[INT]], ptr @[[SHORT]], i8 3, i32 0 }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[LINE_600:.*]] = {{.*}}, i32 600, i32 11 }, ptr @[[INT]], ptr @[[SHORT]], i8 3, i32 0 }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[LINE_700:.*]] = {{.*}}, i32 700, i32 10 }, ptr @[[INT]], ptr @[[SHORT]], i8 3, i32 0 }
// CHECK-SANITIZE-ANYRECOVER-DAG: @[[LINE_800:.*]] = {{.*}}, i32 800, i32 10 }, ptr @[[INT]], ptr @[[SHORT]], i8 3, i32 0 }

unsigned short t0(unsigned short x) {
// CHECK-NOSANITIZE-LABEL: @t0(
// CHECK-NOSANITIZE-NEXT:  entry:
// CHECK-NOSANITIZE-NEXT:    [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-NOSANITIZE-NEXT:    store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[INC:%.*]] = add i16 [[X_RELOADED]], 1
// CHECK-NOSANITIZE-NEXT:    store i16 [[INC]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    ret i16 [[X_RELOADED]]
//
// CHECK-SANITIZE-LABEL:            @t0(
// CHECK-SANITIZE-NEXT:             entry:
// CHECK-SANITIZE-NEXT:               [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-SANITIZE-NEXT:               store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED:%.*]] = zext i16 [[X_RELOADED]] to i32
// CHECK-SANITIZE-NEXT:               [[INC:%.*]] = add i32 [[X_PROMOTED]], 1
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED_DEMOTED:%.*]] = trunc i32 [[INC]] to i16
// CHECK-SANITIZE-NEXT:               [[SRC_INC_NEGATIVITYCHECK:%.*]] = icmp slt i32 [[INC]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[SIGNCHANGECHECK:%.*]] = icmp eq i1 [[SRC_INC_NEGATIVITYCHECK]], false, !nosanitize
// CHECK-SANITIZE-NEXT:               br i1 [[SIGNCHANGECHECK]], label %[[CONT:.*]], label %[[HANDLER_IMPLICIT_X_PROMOTEDERSION:[^,]+]],{{.*}} !nosanitize
// CHECK-SANITIZE:                  [[HANDLER_IMPLICIT_X_PROMOTEDERSION]]:
// CHECK-SANITIZE-TRAP-NEXT:          call void @llvm.ubsantrap(i8 7){{.*}}, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP1:%.*]] = zext i32 [[INC]] to i64, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP2:%.*]] = zext i16 [[X_PROMOTED_DEMOTED]] to i64, !nosanitize
// CHECK-SANITIZE-NORECOVER-NEXT:     call void @__ubsan_handle_implicit_conversion_abort(ptr @[[LINE_100]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       call void @__ubsan_handle_implicit_conversion(ptr @[[LINE_100]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-UNREACHABLE-NEXT:   unreachable, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       br label %[[CONT]], !nosanitize
// CHECK-SANITIZE:                  [[CONT]]:
// CHECK-SANITIZE-NEXT:               store i16 [[X_PROMOTED_DEMOTED]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               ret i16 [[X_RELOADED]]
#line 100
  return x++;
}
unsigned short t1(unsigned short x) {
// CHECK-NOSANITIZE-LABEL: @t1(
// CHECK-NOSANITIZE-NEXT:  entry:
// CHECK-NOSANITIZE-NEXT:    [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-NOSANITIZE-NEXT:    store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[INC:%.*]] = add i16 [[X_RELOADED]], -1
// CHECK-NOSANITIZE-NEXT:    store i16 [[INC]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    ret i16 [[X_RELOADED]]
//
// CHECK-SANITIZE-LABEL:            @t1(
// CHECK-SANITIZE-NEXT:             entry:
// CHECK-SANITIZE-NEXT:               [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-SANITIZE-NEXT:               store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED:%.*]] = zext i16 [[X_RELOADED]] to i32
// CHECK-SANITIZE-NEXT:               [[INC:%.*]] = add i32 [[X_PROMOTED]], -1
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED_DEMOTED:%.*]] = trunc i32 [[INC]] to i16
// CHECK-SANITIZE-NEXT:               [[SRC_INC_NEGATIVITYCHECK:%.*]] = icmp slt i32 [[INC]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[SIGNCHANGECHECK:%.*]] = icmp eq i1 [[SRC_INC_NEGATIVITYCHECK]], false, !nosanitize
// CHECK-SANITIZE-NEXT:               br i1 [[SIGNCHANGECHECK]], label %[[CONT:.*]], label %[[HANDLER_IMPLICIT_X_PROMOTEDERSION:[^,]+]],{{.*}} !nosanitize
// CHECK-SANITIZE:                  [[HANDLER_IMPLICIT_X_PROMOTEDERSION]]:
// CHECK-SANITIZE-TRAP-NEXT:          call void @llvm.ubsantrap(i8 7){{.*}}, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP1:%.*]] = zext i32 [[INC]] to i64, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP2:%.*]] = zext i16 [[X_PROMOTED_DEMOTED]] to i64, !nosanitize
// CHECK-SANITIZE-NORECOVER-NEXT:     call void @__ubsan_handle_implicit_conversion_abort(ptr @[[LINE_200]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       call void @__ubsan_handle_implicit_conversion(ptr @[[LINE_200]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-UNREACHABLE-NEXT:   unreachable, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       br label %[[CONT]], !nosanitize
// CHECK-SANITIZE:                  [[CONT]]:
// CHECK-SANITIZE-NEXT:               store i16 [[X_PROMOTED_DEMOTED]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               ret i16 [[X_RELOADED]]
#line 200
  return x--;
}

unsigned short t2(unsigned short x) {
// CHECK-NOSANITIZE-LABEL: @t2(
// CHECK-NOSANITIZE-NEXT:  entry:
// CHECK-NOSANITIZE-NEXT:    [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-NOSANITIZE-NEXT:    store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[INC:%.*]] = add i16 [[X_RELOADED]], 1
// CHECK-NOSANITIZE-NEXT:    store i16 [[INC]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    ret i16 [[INC]]
//
// CHECK-SANITIZE-LABEL:            @t2(
// CHECK-SANITIZE-NEXT:             entry:
// CHECK-SANITIZE-NEXT:               [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-SANITIZE-NEXT:               store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED:%.*]] = zext i16 [[X_RELOADED]] to i32
// CHECK-SANITIZE-NEXT:               [[INC:%.*]] = add i32 [[X_PROMOTED]], 1
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED_DEMOTED:%.*]] = trunc i32 [[INC]] to i16
// CHECK-SANITIZE-NEXT:               [[SRC_INC_NEGATIVITYCHECK:%.*]] = icmp slt i32 [[INC]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[SIGNCHANGECHECK:%.*]] = icmp eq i1 [[SRC_INC_NEGATIVITYCHECK]], false, !nosanitize
// CHECK-SANITIZE-NEXT:               br i1 [[SIGNCHANGECHECK]], label %[[CONT:.*]], label %[[HANDLER_IMPLICIT_X_PROMOTEDERSION:[^,]+]],{{.*}} !nosanitize
// CHECK-SANITIZE:                  [[HANDLER_IMPLICIT_X_PROMOTEDERSION]]:
// CHECK-SANITIZE-TRAP-NEXT:          call void @llvm.ubsantrap(i8 7){{.*}}, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP1:%.*]] = zext i32 [[INC]] to i64, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP2:%.*]] = zext i16 [[X_PROMOTED_DEMOTED]] to i64, !nosanitize
// CHECK-SANITIZE-NORECOVER-NEXT:     call void @__ubsan_handle_implicit_conversion_abort(ptr @[[LINE_300]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       call void @__ubsan_handle_implicit_conversion(ptr @[[LINE_300]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-UNREACHABLE-NEXT:   unreachable, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       br label %[[CONT]], !nosanitize
// CHECK-SANITIZE:                  [[CONT]]:
// CHECK-SANITIZE-NEXT:               store i16 [[X_PROMOTED_DEMOTED]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               ret i16 [[X_PROMOTED_DEMOTED]]
#line 300
  return ++x;
}

unsigned short t3(unsigned short x) {
// CHECK-NOSANITIZE-LABEL: @t3(
// CHECK-NOSANITIZE-NEXT:  entry:
// CHECK-NOSANITIZE-NEXT:    [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-NOSANITIZE-NEXT:    store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[INC:%.*]] = add i16 [[X_RELOADED]], -1
// CHECK-NOSANITIZE-NEXT:    store i16 [[INC]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    ret i16 [[INC]]
//
// CHECK-SANITIZE-LABEL:            @t3(
// CHECK-SANITIZE-NEXT:             entry:
// CHECK-SANITIZE-NEXT:               [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-SANITIZE-NEXT:               store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED:%.*]] = zext i16 [[X_RELOADED]] to i32
// CHECK-SANITIZE-NEXT:               [[INC:%.*]] = add i32 [[X_PROMOTED]], -1
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED_DEMOTED:%.*]] = trunc i32 [[INC]] to i16
// CHECK-SANITIZE-NEXT:               [[SRC_INC_NEGATIVITYCHECK:%.*]] = icmp slt i32 [[INC]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[SIGNCHANGECHECK:%.*]] = icmp eq i1 [[SRC_INC_NEGATIVITYCHECK]], false, !nosanitize
// CHECK-SANITIZE-NEXT:               br i1 [[SIGNCHANGECHECK]], label %[[CONT:.*]], label %[[HANDLER_IMPLICIT_X_PROMOTEDERSION:[^,]+]],{{.*}} !nosanitize
// CHECK-SANITIZE:                  [[HANDLER_IMPLICIT_X_PROMOTEDERSION]]:
// CHECK-SANITIZE-TRAP-NEXT:          call void @llvm.ubsantrap(i8 7){{.*}}, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP1:%.*]] = zext i32 [[INC]] to i64, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP2:%.*]] = zext i16 [[X_PROMOTED_DEMOTED]] to i64, !nosanitize
// CHECK-SANITIZE-NORECOVER-NEXT:     call void @__ubsan_handle_implicit_conversion_abort(ptr @[[LINE_400]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       call void @__ubsan_handle_implicit_conversion(ptr @[[LINE_400]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-UNREACHABLE-NEXT:   unreachable, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       br label %[[CONT]], !nosanitize
// CHECK-SANITIZE:                  [[CONT]]:
// CHECK-SANITIZE-NEXT:               store i16 [[X_PROMOTED_DEMOTED]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               ret i16 [[X_PROMOTED_DEMOTED]]
#line 400
  return --x;
}

signed short t4(signed short x) {
// CHECK-NOSANITIZE-LABEL: @t4(
// CHECK-NOSANITIZE-NEXT:  entry:
// CHECK-NOSANITIZE-NEXT:    [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-NOSANITIZE-NEXT:    store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[INC:%.*]] = add i16 [[X_RELOADED]], 1
// CHECK-NOSANITIZE-NEXT:    store i16 [[INC]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    ret i16 [[X_RELOADED]]
//
// CHECK-SANITIZE-LABEL:            @t4(
// CHECK-SANITIZE-NEXT:             entry:
// CHECK-SANITIZE-NEXT:               [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-SANITIZE-NEXT:               store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED:%.*]] = sext i16 [[X_RELOADED]] to i32
// CHECK-SANITIZE-NEXT:               [[INC:%.*]] = add i32 [[X_PROMOTED]], 1
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED_DEMOTED:%.*]] = trunc i32 [[INC]] to i16
// CHECK-SANITIZE-NEXT:               [[SRC_INC_NEGATIVITYCHECK:%.*]] = icmp slt i32 [[INC]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[DST_NEGATIVITYCHECK:%.*]] = icmp slt i16 [[X_PROMOTED_DEMOTED]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[SIGNCHANGECHECK:%.*]] = icmp eq i1 [[SRC_INC_NEGATIVITYCHECK]], [[DST_NEGATIVITYCHECK]], !nosanitize
// CHECK-SANITIZE-NEXT:               br i1 [[SIGNCHANGECHECK]], label %[[CONT:.*]], label %[[HANDLER_IMPLICIT_X_PROMOTEDERSION:[^,]+]],{{.*}} !nosanitize
// CHECK-SANITIZE:                  [[HANDLER_IMPLICIT_X_PROMOTEDERSION]]:
// CHECK-SANITIZE-TRAP-NEXT:          call void @llvm.ubsantrap(i8 7){{.*}}, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP1:%.*]] = zext i32 [[INC]] to i64, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP2:%.*]] = zext i16 [[X_PROMOTED_DEMOTED]] to i64, !nosanitize
// CHECK-SANITIZE-NORECOVER-NEXT:     call void @__ubsan_handle_implicit_conversion_abort(ptr @[[LINE_500]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       call void @__ubsan_handle_implicit_conversion(ptr @[[LINE_500]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-UNREACHABLE-NEXT:   unreachable, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       br label %[[CONT]], !nosanitize
// CHECK-SANITIZE:                  [[CONT]]:
// CHECK-SANITIZE-NEXT:               store i16 [[X_PROMOTED_DEMOTED]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               ret i16 [[X_RELOADED]]
#line 500
  return x++;
}
signed short t5(signed short x) {
// CHECK-NOSANITIZE-LABEL: @t5(
// CHECK-NOSANITIZE-NEXT:  entry:
// CHECK-NOSANITIZE-NEXT:    [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-NOSANITIZE-NEXT:    store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[INC:%.*]] = add i16 [[X_RELOADED]], -1
// CHECK-NOSANITIZE-NEXT:    store i16 [[INC]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    ret i16 [[X_RELOADED]]
//
// CHECK-SANITIZE-LABEL:            @t5(
// CHECK-SANITIZE-NEXT:             entry:
// CHECK-SANITIZE-NEXT:               [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-SANITIZE-NEXT:               store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED:%.*]] = sext i16 [[X_RELOADED]] to i32
// CHECK-SANITIZE-NEXT:               [[INC:%.*]] = add i32 [[X_PROMOTED]], -1
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED_DEMOTED:%.*]] = trunc i32 [[INC]] to i16
// CHECK-SANITIZE-NEXT:               [[SRC_INC_NEGATIVITYCHECK:%.*]] = icmp slt i32 [[INC]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[DST_NEGATIVITYCHECK:%.*]] = icmp slt i16 [[X_PROMOTED_DEMOTED]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[SIGNCHANGECHECK:%.*]] = icmp eq i1 [[SRC_INC_NEGATIVITYCHECK]], [[DST_NEGATIVITYCHECK]], !nosanitize
// CHECK-SANITIZE-NEXT:               br i1 [[SIGNCHANGECHECK]], label %[[CONT:.*]], label %[[HANDLER_IMPLICIT_X_PROMOTEDERSION:[^,]+]],{{.*}} !nosanitize
// CHECK-SANITIZE:                  [[HANDLER_IMPLICIT_X_PROMOTEDERSION]]:
// CHECK-SANITIZE-TRAP-NEXT:          call void @llvm.ubsantrap(i8 7){{.*}}, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP1:%.*]] = zext i32 [[INC]] to i64, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP2:%.*]] = zext i16 [[X_PROMOTED_DEMOTED]] to i64, !nosanitize
// CHECK-SANITIZE-NORECOVER-NEXT:     call void @__ubsan_handle_implicit_conversion_abort(ptr @[[LINE_600]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       call void @__ubsan_handle_implicit_conversion(ptr @[[LINE_600]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-UNREACHABLE-NEXT:   unreachable, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       br label %[[CONT]], !nosanitize
// CHECK-SANITIZE:                  [[CONT]]:
// CHECK-SANITIZE-NEXT:               store i16 [[X_PROMOTED_DEMOTED]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               ret i16 [[X_RELOADED]]
#line 600
  return x--;
}

signed short t6(signed short x) {
// CHECK-NOSANITIZE-LABEL: @t6(
// CHECK-NOSANITIZE-NEXT:  entry:
// CHECK-NOSANITIZE-NEXT:    [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-NOSANITIZE-NEXT:    store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[INC:%.*]] = add i16 [[X_RELOADED]], 1
// CHECK-NOSANITIZE-NEXT:    store i16 [[INC]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    ret i16 [[INC]]
//
// CHECK-SANITIZE-LABEL:            @t6(
// CHECK-SANITIZE-NEXT:             entry:
// CHECK-SANITIZE-NEXT:               [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-SANITIZE-NEXT:               store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED:%.*]] = sext i16 [[X_RELOADED]] to i32
// CHECK-SANITIZE-NEXT:               [[INC:%.*]] = add i32 [[X_PROMOTED]], 1
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED_DEMOTED:%.*]] = trunc i32 [[INC]] to i16
// CHECK-SANITIZE-NEXT:               [[SRC_INC_NEGATIVITYCHECK:%.*]] = icmp slt i32 [[INC]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[DST_NEGATIVITYCHECK:%.*]] = icmp slt i16 [[X_PROMOTED_DEMOTED]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[SIGNCHANGECHECK:%.*]] = icmp eq i1 [[SRC_INC_NEGATIVITYCHECK]], [[DST_NEGATIVITYCHECK]], !nosanitize
// CHECK-SANITIZE-NEXT:               br i1 [[SIGNCHANGECHECK]], label %[[CONT:.*]], label %[[HANDLER_IMPLICIT_X_PROMOTEDERSION:[^,]+]],{{.*}} !nosanitize
// CHECK-SANITIZE:                  [[HANDLER_IMPLICIT_X_PROMOTEDERSION]]:
// CHECK-SANITIZE-TRAP-NEXT:          call void @llvm.ubsantrap(i8 7){{.*}}, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP1:%.*]] = zext i32 [[INC]] to i64, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP2:%.*]] = zext i16 [[X_PROMOTED_DEMOTED]] to i64, !nosanitize
// CHECK-SANITIZE-NORECOVER-NEXT:     call void @__ubsan_handle_implicit_conversion_abort(ptr @[[LINE_700]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       call void @__ubsan_handle_implicit_conversion(ptr @[[LINE_700]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-UNREACHABLE-NEXT:   unreachable, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       br label %[[CONT]], !nosanitize
// CHECK-SANITIZE:                  [[CONT]]:
// CHECK-SANITIZE-NEXT:               store i16 [[X_PROMOTED_DEMOTED]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               ret i16 [[X_PROMOTED_DEMOTED]]
#line 700
  return ++x;
}

signed short t7(signed short x) {
// CHECK-NOSANITIZE-LABEL: @t7(
// CHECK-NOSANITIZE-NEXT:  entry:
// CHECK-NOSANITIZE-NEXT:    [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-NOSANITIZE-NEXT:    store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    [[INC:%.*]] = add i16 [[X_RELOADED]], -1
// CHECK-NOSANITIZE-NEXT:    store i16 [[INC]], ptr [[X_ADDR]], align 2
// CHECK-NOSANITIZE-NEXT:    ret i16 [[INC]]
//
// CHECK-SANITIZE-LABEL:            @t7(
// CHECK-SANITIZE-NEXT:             entry:
// CHECK-SANITIZE-NEXT:               [[X_ADDR:%.*]] = alloca i16, align 2
// CHECK-SANITIZE-NEXT:               store i16 [[X:%.*]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_RELOADED:%.*]] = load i16, ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED:%.*]] = sext i16 [[X_RELOADED]] to i32
// CHECK-SANITIZE-NEXT:               [[INC:%.*]] = add i32 [[X_PROMOTED]], -1
// CHECK-SANITIZE-NEXT:               [[X_PROMOTED_DEMOTED:%.*]] = trunc i32 [[INC]] to i16
// CHECK-SANITIZE-NEXT:               [[SRC_INC_NEGATIVITYCHECK:%.*]] = icmp slt i32 [[INC]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[DST_NEGATIVITYCHECK:%.*]] = icmp slt i16 [[X_PROMOTED_DEMOTED]], 0, !nosanitize !2
// CHECK-SANITIZE-NEXT:               [[SIGNCHANGECHECK:%.*]] = icmp eq i1 [[SRC_INC_NEGATIVITYCHECK]], [[DST_NEGATIVITYCHECK]], !nosanitize
// CHECK-SANITIZE-NEXT:               br i1 [[SIGNCHANGECHECK]], label %[[CONT:.*]], label %[[HANDLER_IMPLICIT_X_PROMOTEDERSION:[^,]+]],{{.*}} !nosanitize
// CHECK-SANITIZE:                  [[HANDLER_IMPLICIT_X_PROMOTEDERSION]]:
// CHECK-SANITIZE-TRAP-NEXT:          call void @llvm.ubsantrap(i8 7){{.*}}, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP1:%.*]] = zext i32 [[INC]] to i64, !nosanitize
// CHECK-SANITIZE-ANYRECOVER-NEXT:    [[TMP2:%.*]] = zext i16 [[X_PROMOTED_DEMOTED]] to i64, !nosanitize
// CHECK-SANITIZE-NORECOVER-NEXT:     call void @__ubsan_handle_implicit_conversion_abort(ptr @[[LINE_800]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       call void @__ubsan_handle_implicit_conversion(ptr @[[LINE_800]], i64 [[TMP1]], i64 [[TMP2]]) #2, !nosanitize
// CHECK-SANITIZE-UNREACHABLE-NEXT:   unreachable, !nosanitize
// CHECK-SANITIZE-RECOVER-NEXT:       br label %[[CONT]], !nosanitize
// CHECK-SANITIZE:                  [[CONT]]:
// CHECK-SANITIZE-NEXT:               store i16 [[X_PROMOTED_DEMOTED]], ptr [[X_ADDR]], align 2
// CHECK-SANITIZE-NEXT:               ret i16 [[X_PROMOTED_DEMOTED]]
#line 800
  return --x;
}
