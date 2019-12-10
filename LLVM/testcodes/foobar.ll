; ModuleID = 'foobar.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }

@.str = private unnamed_addr constant [11 x i8] c"im in foo\0A\00", align 1
@.str1 = private unnamed_addr constant [14 x i8] c"im in foobar\0A\00", align 1
@.str2 = private unnamed_addr constant [11 x i8] c"im in bar\0A\00", align 1
@.str3 = private unnamed_addr constant [8 x i8] c"input: \00", align 1
@.str4 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str5 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@stderr = external global %struct._IO_FILE*
@.str6 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@.str7 = private unnamed_addr constant [26 x i8] c"CallInst called %d times\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @foo() #0 {
entry:
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([11 x i8]* @.str, i32 0, i32 0))
  ret i32 1
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: nounwind uwtable
define i32 @foobar() #0 {
entry:
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([14 x i8]* @.str1, i32 0, i32 0))
  ret i32 2
}

; Function Attrs: nounwind uwtable
define i32 @bar(i32 %i) #0 {
entry:
  %retval = alloca i32, align 4
  %i.addr = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 %i, i32* %i.addr, align 4
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([11 x i8]* @.str2, i32 0, i32 0))
  %0 = load i32* %i.addr, align 4
  %tobool = icmp ne i32 %0, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %call1 = call i32 @foo()
  store i32 %call1, i32* %b, align 4
  %1 = load i32* %b, align 4
  %add = add nsw i32 1, %1
  store i32 %add, i32* %retval
  br label %return

if.else:                                          ; preds = %entry
  %call2 = call i32 @foobar()
  store i32 %call2, i32* %c, align 4
  %2 = load i32* %c, align 4
  %add3 = add nsw i32 2, %2
  store i32 %add3, i32* %retval
  br label %return

return:                                           ; preds = %if.else, %if.then
  %3 = load i32* %retval
  ret i32 %3
}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 0, i32* %retval
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @.str3, i32 0, i32 0))
  %call1 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str4, i32 0, i32 0), i32* %d)
  %0 = load i32* %d, align 4
  %call2 = call i32 @bar(i32 %0)
  %call3 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str5, i32 0, i32 0), i32 %call2)
  ret i32 0
}

declare i32 @__isoc99_scanf(i8*, ...) #1

; Function Attrs: nounwind uwtable
define void @log_msg(i8* %msg) #0 {
entry:
  %msg.addr = alloca i8*, align 8
  store i8* %msg, i8** %msg.addr, align 8
  %0 = load %struct._IO_FILE** @stderr, align 8
  %1 = load i8** %msg.addr, align 8
  %call = call i32 (%struct._IO_FILE*, i8*, ...)* @fprintf(%struct._IO_FILE* %0, i8* getelementptr inbounds ([4 x i8]* @.str6, i32 0, i32 0), i8* %1)
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
  %call = call i32 (%struct._IO_FILE*, i8*, ...)* @fprintf(%struct._IO_FILE* %0, i8* getelementptr inbounds ([26 x i8]* @.str7, i32 0, i32 0), i32 %1)
  ret void
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.4 (tags/RELEASE_34/final)"}
