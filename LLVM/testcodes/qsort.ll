; ModuleID = 'qsort.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }

@.str = private unnamed_addr constant [43 x i8] c"Input 5 numbers delimited by Enter('\5Cn'):\0A\00", align 1
@.str1 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str2 = private unnamed_addr constant [17 x i8] c"sorted outputs:\0A\00", align 1
@.str3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@stderr = external global %struct._IO_FILE*
@.str4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str5 = private unnamed_addr constant [26 x i8] c"CallInst called %d times\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %array = alloca [5 x i32], align 16
  %i = alloca i32, align 4
  %i3 = alloca i32, align 4
  store i32 0, i32* %retval
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([43 x i8]* @.str, i32 0, i32 0))
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32* %i, align 4
  %cmp = icmp slt i32 %0, 5
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32* %i, align 4
  %idxprom = sext i32 %1 to i64
  %arrayidx = getelementptr inbounds [5 x i32]* %array, i32 0, i64 %idxprom
  %call1 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str1, i32 0, i32 0), i32* %arrayidx)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %2 = load i32* %i, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %arraydecay = getelementptr inbounds [5 x i32]* %array, i32 0, i32 0
  call void @qsort(i32* %arraydecay, i32 0, i32 4)
  %call2 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([17 x i8]* @.str2, i32 0, i32 0))
  store i32 0, i32* %i3, align 4
  br label %for.cond4

for.cond4:                                        ; preds = %for.inc10, %for.end
  %3 = load i32* %i3, align 4
  %cmp5 = icmp slt i32 %3, 5
  br i1 %cmp5, label %for.body6, label %for.end12

for.body6:                                        ; preds = %for.cond4
  %4 = load i32* %i3, align 4
  %idxprom7 = sext i32 %4 to i64
  %arrayidx8 = getelementptr inbounds [5 x i32]* %array, i32 0, i64 %idxprom7
  %5 = load i32* %arrayidx8, align 4
  %call9 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str3, i32 0, i32 0), i32 %5)
  br label %for.inc10

for.inc10:                                        ; preds = %for.body6
  %6 = load i32* %i3, align 4
  %inc11 = add nsw i32 %6, 1
  store i32 %inc11, i32* %i3, align 4
  br label %for.cond4

for.end12:                                        ; preds = %for.cond4
  ret i32 0
}

declare i32 @printf(i8*, ...) #1

declare i32 @__isoc99_scanf(i8*, ...) #1

; Function Attrs: nounwind uwtable
define void @qsort(i32* %a, i32 %left, i32 %right) #0 {
entry:
  %a.addr = alloca i32*, align 8
  %left.addr = alloca i32, align 4
  %right.addr = alloca i32, align 4
  %i = alloca i32, align 4
  %last = alloca i32, align 4
  store i32* %a, i32** %a.addr, align 8
  store i32 %left, i32* %left.addr, align 4
  store i32 %right, i32* %right.addr, align 4
  %0 = load i32* %left.addr, align 4
  %1 = load i32* %right.addr, align 4
  %cmp = icmp sge i32 %0, %1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  br label %return

if.end:                                           ; preds = %entry
  %2 = load i32** %a.addr, align 8
  %3 = load i32* %left.addr, align 4
  %4 = load i32* %left.addr, align 4
  %5 = load i32* %right.addr, align 4
  %add = add nsw i32 %4, %5
  %div = sdiv i32 %add, 2
  call void @swap(i32* %2, i32 %3, i32 %div)
  %6 = load i32* %left.addr, align 4
  store i32 %6, i32* %last, align 4
  %7 = load i32* %left.addr, align 4
  %add1 = add nsw i32 %7, 1
  store i32 %add1, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %if.end
  %8 = load i32* %i, align 4
  %9 = load i32* %right.addr, align 4
  %cmp2 = icmp sle i32 %8, %9
  br i1 %cmp2, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %10 = load i32* %i, align 4
  %idxprom = sext i32 %10 to i64
  %11 = load i32** %a.addr, align 8
  %arrayidx = getelementptr inbounds i32* %11, i64 %idxprom
  %12 = load i32* %arrayidx, align 4
  %13 = load i32* %left.addr, align 4
  %idxprom3 = sext i32 %13 to i64
  %14 = load i32** %a.addr, align 8
  %arrayidx4 = getelementptr inbounds i32* %14, i64 %idxprom3
  %15 = load i32* %arrayidx4, align 4
  %cmp5 = icmp slt i32 %12, %15
  br i1 %cmp5, label %if.then6, label %if.end7

if.then6:                                         ; preds = %for.body
  %16 = load i32** %a.addr, align 8
  %17 = load i32* %last, align 4
  %inc = add nsw i32 %17, 1
  store i32 %inc, i32* %last, align 4
  %18 = load i32* %i, align 4
  call void @swap(i32* %16, i32 %inc, i32 %18)
  br label %if.end7

if.end7:                                          ; preds = %if.then6, %for.body
  br label %for.inc

for.inc:                                          ; preds = %if.end7
  %19 = load i32* %i, align 4
  %inc8 = add nsw i32 %19, 1
  store i32 %inc8, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %20 = load i32** %a.addr, align 8
  %21 = load i32* %last, align 4
  %22 = load i32* %left.addr, align 4
  call void @swap(i32* %20, i32 %21, i32 %22)
  %23 = load i32** %a.addr, align 8
  %24 = load i32* %left.addr, align 4
  %25 = load i32* %last, align 4
  %sub = sub nsw i32 %25, 1
  call void @qsort(i32* %23, i32 %24, i32 %sub)
  %26 = load i32** %a.addr, align 8
  %27 = load i32* %last, align 4
  %add9 = add nsw i32 1, %27
  %28 = load i32* %right.addr, align 4
  call void @qsort(i32* %26, i32 %add9, i32 %28)
  br label %return

return:                                           ; preds = %for.end, %if.then
  ret void
}

; Function Attrs: nounwind uwtable
define void @swap(i32* %v, i32 %i, i32 %j) #0 {
entry:
  %v.addr = alloca i32*, align 8
  %i.addr = alloca i32, align 4
  %j.addr = alloca i32, align 4
  %temp = alloca i32, align 4
  store i32* %v, i32** %v.addr, align 8
  store i32 %i, i32* %i.addr, align 4
  store i32 %j, i32* %j.addr, align 4
  %0 = load i32* %i.addr, align 4
  %idxprom = sext i32 %0 to i64
  %1 = load i32** %v.addr, align 8
  %arrayidx = getelementptr inbounds i32* %1, i64 %idxprom
  %2 = load i32* %arrayidx, align 4
  store i32 %2, i32* %temp, align 4
  %3 = load i32* %j.addr, align 4
  %idxprom1 = sext i32 %3 to i64
  %4 = load i32** %v.addr, align 8
  %arrayidx2 = getelementptr inbounds i32* %4, i64 %idxprom1
  %5 = load i32* %arrayidx2, align 4
  %6 = load i32* %i.addr, align 4
  %idxprom3 = sext i32 %6 to i64
  %7 = load i32** %v.addr, align 8
  %arrayidx4 = getelementptr inbounds i32* %7, i64 %idxprom3
  store i32 %5, i32* %arrayidx4, align 4
  %8 = load i32* %temp, align 4
  %9 = load i32* %j.addr, align 4
  %idxprom5 = sext i32 %9 to i64
  %10 = load i32** %v.addr, align 8
  %arrayidx6 = getelementptr inbounds i32* %10, i64 %idxprom5
  store i32 %8, i32* %arrayidx6, align 4
  ret void
}

; Function Attrs: nounwind uwtable
define void @log_msg(i8* %msg) #0 {
entry:
  %msg.addr = alloca i8*, align 8
  store i8* %msg, i8** %msg.addr, align 8
  %0 = load %struct._IO_FILE** @stderr, align 8
  %1 = load i8** %msg.addr, align 8
  %call = call i32 (%struct._IO_FILE*, i8*, ...)* @fprintf(%struct._IO_FILE* %0, i8* getelementptr inbounds ([3 x i8]* @.str4, i32 0, i32 0), i8* %1)
  ret void
}

declare i32 @fprintf(%struct._IO_FILE*, i8*, ...) #1

; Function Attrs: nounwind uwtable
define void @log_call_count(i32 %cnt) #0 {
entry:
  %cnt.addr = alloca i32, align 4
  store i32 %cnt, i32* %cnt.addr, align 4
  %0 = load %struct._IO_FILE** @stderr, align 8
  %1 = load i32* %cnt.addr, align 4
  %call = call i32 (%struct._IO_FILE*, i8*, ...)* @fprintf(%struct._IO_FILE* %0, i8* getelementptr inbounds ([26 x i8]* @.str5, i32 0, i32 0), i32 %1)
  ret void
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.4 (tags/RELEASE_34/final)"}
