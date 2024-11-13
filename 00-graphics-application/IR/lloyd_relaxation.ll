; ModuleID = '/home/ketchupp/Programming/llvm-course/00-graphics-application/src/lloyd_relaxation.c'
source_filename = "/home/ketchupp/Programming/llvm-course/00-graphics-application/src/lloyd_relaxation.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.point_s = type { i32, i32 }
%struct.color_s = type { i32, i32, i32 }

; Function Attrs: noreturn nounwind uwtable
define dso_local void @lloydRelaxation() local_unnamed_addr #0 {
  %1 = alloca [100 x i32], align 16
  %2 = alloca [100 x %struct.point_s], align 16
  %3 = alloca [100 x %struct.color_s], align 16
  %4 = alloca [100 x i32], align 16
  %5 = alloca [100 x i32], align 16
  %6 = alloca [100 x i32], align 16
  call void @llvm.lifetime.start.p0(i64 800, ptr nonnull %2) #5
  br label %7

7:                                                ; preds = %7, %0
  %8 = phi i64 [ 0, %0 ], [ %17, %7 ]
  %9 = tail call i32 @Rand() #5
  %10 = srem i32 %9, 360
  %11 = add nsw i32 %10, 180
  %12 = getelementptr inbounds %struct.point_s, ptr %2, i64 %8
  store i32 %11, ptr %12, align 8, !tbaa !5
  %13 = tail call i32 @Rand() #5
  %14 = srem i32 %13, 360
  %15 = add nsw i32 %14, 180
  %16 = getelementptr inbounds %struct.point_s, ptr %2, i64 %8, i32 1
  store i32 %15, ptr %16, align 4, !tbaa !10
  %17 = add nuw nsw i64 %8, 1
  %18 = icmp eq i64 %17, 100
  br i1 %18, label %19, label %7, !llvm.loop !11

19:                                               ; preds = %7
  call void @llvm.lifetime.start.p0(i64 1200, ptr nonnull %3) #5
  br label %20

20:                                               ; preds = %20, %19
  %21 = phi i64 [ 0, %19 ], [ %31, %20 ]
  %22 = tail call i32 @Rand() #5
  %23 = srem i32 %22, 256
  %24 = getelementptr inbounds %struct.color_s, ptr %3, i64 %21
  store i32 %23, ptr %24, align 4, !tbaa !13
  %25 = tail call i32 @Rand() #5
  %26 = srem i32 %25, 256
  %27 = getelementptr inbounds %struct.color_s, ptr %3, i64 %21, i32 1
  store i32 %26, ptr %27, align 4, !tbaa !15
  %28 = tail call i32 @Rand() #5
  %29 = srem i32 %28, 256
  %30 = getelementptr inbounds %struct.color_s, ptr %3, i64 %21, i32 2
  store i32 %29, ptr %30, align 4, !tbaa !16
  %31 = add nuw nsw i64 %21, 1
  %32 = icmp eq i64 %31, 100
  br i1 %32, label %33, label %20, !llvm.loop !17

33:                                               ; preds = %20, %116
  call void @llvm.lifetime.start.p0(i64 400, ptr nonnull %4) #5
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 16 dereferenceable(400) %4, i8 0, i64 400, i1 false)
  call void @llvm.lifetime.start.p0(i64 400, ptr nonnull %5) #5
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 16 dereferenceable(400) %5, i8 0, i64 400, i1 false)
  call void @llvm.lifetime.start.p0(i64 400, ptr nonnull %6) #5
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 16 dereferenceable(400) %6, i8 0, i64 400, i1 false)
  br label %34

34:                                               ; preds = %33, %37
  %35 = phi i32 [ 0, %33 ], [ %38, %37 ]
  br label %40

36:                                               ; preds = %37
  tail call void @updateScreen() #5
  br label %117

37:                                               ; preds = %97
  %38 = add nuw nsw i32 %35, 1
  %39 = icmp eq i32 %38, 720
  br i1 %39, label %36, label %34, !llvm.loop !18

40:                                               ; preds = %34, %97
  %41 = phi i32 [ 0, %34 ], [ %114, %97 ]
  call void @llvm.lifetime.start.p0(i64 400, ptr nonnull %1) #5
  br label %70

42:                                               ; preds = %70
  %43 = load i32, ptr %1, align 16, !tbaa !19
  br label %44

44:                                               ; preds = %44, %42
  %45 = phi i64 [ 1, %42 ], [ %68, %44 ]
  %46 = phi i32 [ 0, %42 ], [ %67, %44 ]
  %47 = phi i32 [ %43, %42 ], [ %65, %44 ]
  %48 = getelementptr inbounds i32, ptr %1, i64 %45
  %49 = load i32, ptr %48, align 4, !tbaa !19
  %50 = icmp slt i32 %49, %47
  %51 = tail call i32 @llvm.smin.i32(i32 %49, i32 %47)
  %52 = trunc i64 %45 to i32
  %53 = select i1 %50, i32 %52, i32 %46
  %54 = add nuw nsw i64 %45, 1
  %55 = getelementptr inbounds i32, ptr %1, i64 %54
  %56 = load i32, ptr %55, align 4, !tbaa !19
  %57 = icmp slt i32 %56, %51
  %58 = tail call i32 @llvm.smin.i32(i32 %56, i32 %51)
  %59 = trunc i64 %54 to i32
  %60 = select i1 %57, i32 %59, i32 %53
  %61 = add nuw nsw i64 %45, 2
  %62 = getelementptr inbounds i32, ptr %1, i64 %61
  %63 = load i32, ptr %62, align 4, !tbaa !19
  %64 = icmp slt i32 %63, %58
  %65 = tail call i32 @llvm.smin.i32(i32 %63, i32 %58)
  %66 = trunc i64 %61 to i32
  %67 = select i1 %64, i32 %66, i32 %60
  %68 = add nuw nsw i64 %45, 3
  %69 = icmp eq i64 %68, 100
  br i1 %69, label %97, label %44, !llvm.loop !20

70:                                               ; preds = %70, %40
  %71 = phi i64 [ 0, %40 ], [ %95, %70 ]
  %72 = getelementptr inbounds %struct.point_s, ptr %2, i64 %71
  %73 = load i64, ptr %72, align 16
  %74 = trunc i64 %73 to i32
  %75 = lshr i64 %73, 32
  %76 = trunc i64 %75 to i32
  %77 = sub nsw i32 %74, %35
  %78 = mul nsw i32 %77, %77
  %79 = sub nsw i32 %76, %41
  %80 = mul nsw i32 %79, %79
  %81 = add nuw nsw i32 %80, %78
  %82 = getelementptr inbounds [100 x i32], ptr %1, i64 0, i64 %71
  store i32 %81, ptr %82, align 8, !tbaa !19
  %83 = or disjoint i64 %71, 1
  %84 = getelementptr inbounds %struct.point_s, ptr %2, i64 %83
  %85 = load i64, ptr %84, align 8
  %86 = trunc i64 %85 to i32
  %87 = lshr i64 %85, 32
  %88 = trunc i64 %87 to i32
  %89 = sub nsw i32 %86, %35
  %90 = mul nsw i32 %89, %89
  %91 = sub nsw i32 %88, %41
  %92 = mul nsw i32 %91, %91
  %93 = add nuw nsw i32 %92, %90
  %94 = getelementptr inbounds [100 x i32], ptr %1, i64 0, i64 %83
  store i32 %93, ptr %94, align 4, !tbaa !19
  %95 = add nuw nsw i64 %71, 2
  %96 = icmp eq i64 %95, 100
  br i1 %96, label %42, label %70, !llvm.loop !21

97:                                               ; preds = %44
  call void @llvm.lifetime.end.p0(i64 400, ptr nonnull %1) #5
  %98 = sext i32 %67 to i64
  %99 = getelementptr inbounds [100 x i32], ptr %4, i64 0, i64 %98
  %100 = load i32, ptr %99, align 4, !tbaa !19
  %101 = add nsw i32 %100, %35
  store i32 %101, ptr %99, align 4, !tbaa !19
  %102 = getelementptr inbounds [100 x i32], ptr %5, i64 0, i64 %98
  %103 = load i32, ptr %102, align 4, !tbaa !19
  %104 = add nsw i32 %103, %41
  store i32 %104, ptr %102, align 4, !tbaa !19
  %105 = getelementptr inbounds [100 x i32], ptr %6, i64 0, i64 %98
  %106 = load i32, ptr %105, align 4, !tbaa !19
  %107 = add nsw i32 %106, 1
  store i32 %107, ptr %105, align 4, !tbaa !19
  %108 = getelementptr inbounds [100 x %struct.color_s], ptr %3, i64 0, i64 %98
  %109 = load i32, ptr %108, align 4, !tbaa !13
  %110 = getelementptr inbounds [100 x %struct.color_s], ptr %3, i64 0, i64 %98, i32 1
  %111 = load i32, ptr %110, align 4, !tbaa !15
  %112 = getelementptr inbounds [100 x %struct.color_s], ptr %3, i64 0, i64 %98, i32 2
  %113 = load i32, ptr %112, align 4, !tbaa !16
  tail call void @setColor(i32 noundef %109, i32 noundef %111, i32 noundef %113) #5
  tail call void @putPixel(i32 noundef %35, i32 noundef %41) #5
  %114 = add nuw nsw i32 %41, 1
  %115 = icmp eq i32 %114, 720
  br i1 %115, label %37, label %40, !llvm.loop !22

116:                                              ; preds = %117
  call void @llvm.lifetime.end.p0(i64 400, ptr nonnull %6) #5
  call void @llvm.lifetime.end.p0(i64 400, ptr nonnull %5) #5
  call void @llvm.lifetime.end.p0(i64 400, ptr nonnull %4) #5
  br label %33

117:                                              ; preds = %36, %117
  %118 = phi i64 [ 0, %36 ], [ %129, %117 ]
  %119 = getelementptr inbounds [100 x i32], ptr %4, i64 0, i64 %118
  %120 = load i32, ptr %119, align 4, !tbaa !19
  %121 = getelementptr inbounds [100 x i32], ptr %6, i64 0, i64 %118
  %122 = load i32, ptr %121, align 4, !tbaa !19
  %123 = sdiv i32 %120, %122
  %124 = getelementptr inbounds [100 x %struct.point_s], ptr %2, i64 0, i64 %118
  store i32 %123, ptr %124, align 8, !tbaa !5
  %125 = getelementptr inbounds [100 x i32], ptr %5, i64 0, i64 %118
  %126 = load i32, ptr %125, align 4, !tbaa !19
  %127 = sdiv i32 %126, %122
  %128 = getelementptr inbounds [100 x %struct.point_s], ptr %2, i64 0, i64 %118, i32 1
  store i32 %127, ptr %128, align 4, !tbaa !10
  %129 = add nuw nsw i64 %118, 1
  %130 = icmp eq i64 %129, 100
  br i1 %130, label %116, label %117, !llvm.loop !23
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

declare void @setColor(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #3

declare void @putPixel(i32 noundef, i32 noundef) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

declare void @updateScreen() local_unnamed_addr #3

declare i32 @Rand() local_unnamed_addr #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.smin.i32(i32, i32) #4

attributes #0 = { noreturn nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #5 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
!5 = !{!6, !7, i64 0}
!6 = !{!"point_s", !7, i64 0, !7, i64 4}
!7 = !{!"int", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C/C++ TBAA"}
!10 = !{!6, !7, i64 4}
!11 = distinct !{!11, !12}
!12 = !{!"llvm.loop.mustprogress"}
!13 = !{!14, !7, i64 0}
!14 = !{!"color_s", !7, i64 0, !7, i64 4, !7, i64 8}
!15 = !{!14, !7, i64 4}
!16 = !{!14, !7, i64 8}
!17 = distinct !{!17, !12}
!18 = distinct !{!18, !12}
!19 = !{!7, !7, i64 0}
!20 = distinct !{!20, !12}
!21 = distinct !{!21, !12}
!22 = distinct !{!22, !12}
!23 = distinct !{!23, !12}
