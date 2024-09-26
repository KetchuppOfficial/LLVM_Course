; ModuleID = '/home/ketchupp/Programming/llvm-course/00-graphics_application/src/lloyd_relaxation.c'
source_filename = "/home/ketchupp/Programming/llvm-course/00-graphics_application/src/lloyd_relaxation.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.point_s = type { i32, i32 }
%struct.polygon_s = type { [104 x %struct.point_s], i32 }
%struct.tagged_point_s = type { %struct.point_s, i32, i32 }
%struct.line_s = type { i32, i32, i32 }

@generate_points.points = internal global [100 x %struct.point_s] zeroinitializer, align 16
@get_cells.cells = internal global [100 x %struct.polygon_s] zeroinitializer, align 16
@get_cells.intersections = internal global <{ %struct.tagged_point_s, %struct.tagged_point_s, %struct.tagged_point_s, %struct.tagged_point_s, [5098 x %struct.tagged_point_s] }> <{ %struct.tagged_point_s { %struct.point_s zeroinitializer, i32 -4, i32 -3 }, %struct.tagged_point_s { %struct.point_s { i32 720, i32 0 }, i32 -4, i32 -2 }, %struct.tagged_point_s { %struct.point_s { i32 720, i32 720 }, i32 -2, i32 -1 }, %struct.tagged_point_s { %struct.point_s { i32 0, i32 720 }, i32 -1, i32 -3 }, [5098 x %struct.tagged_point_s] zeroinitializer }>, align 16
@borders = internal constant [4 x %struct.line_s] [%struct.line_s { i32 0, i32 1, i32 0 }, %struct.line_s { i32 1, i32 0, i32 0 }, %struct.line_s { i32 1, i32 0, i32 -720 }, %struct.line_s { i32 0, i32 1, i32 -720 }], align 16
@center = external local_unnamed_addr global %struct.point_s, align 4
@compute_normals.perps = internal global [100 x [99 x %struct.line_s]] zeroinitializer, align 16

; Function Attrs: nofree norecurse nosync nounwind memory(argmem: read) uwtable
define dso_local i64 @mass_center(ptr nocapture noundef readonly %0) local_unnamed_addr #0 {
  %2 = getelementptr inbounds i8, ptr %0, i64 832
  %3 = load i32, ptr %2, align 4, !tbaa !5
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %41, label %5

5:                                                ; preds = %1
  %6 = zext i32 %3 to i64
  %7 = icmp ult i32 %3, 8
  br i1 %7, label %37, label %8

8:                                                ; preds = %5
  %9 = and i64 %6, 4294967288
  br label %10

10:                                               ; preds = %10, %8
  %11 = phi i64 [ 0, %8 ], [ %29, %10 ]
  %12 = phi <4 x i32> [ zeroinitializer, %8 ], [ %27, %10 ]
  %13 = phi <4 x i32> [ zeroinitializer, %8 ], [ %28, %10 ]
  %14 = phi <4 x i32> [ zeroinitializer, %8 ], [ %25, %10 ]
  %15 = phi <4 x i32> [ zeroinitializer, %8 ], [ %26, %10 ]
  %16 = or disjoint i64 %11, 4
  %17 = getelementptr inbounds [104 x %struct.point_s], ptr %0, i64 0, i64 %11
  %18 = getelementptr inbounds [104 x %struct.point_s], ptr %0, i64 0, i64 %16
  %19 = load <8 x i32>, ptr %17, align 4, !tbaa !10
  %20 = load <8 x i32>, ptr %18, align 4, !tbaa !10
  %21 = shufflevector <8 x i32> %19, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  %22 = shufflevector <8 x i32> %20, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  %23 = shufflevector <8 x i32> %19, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  %24 = shufflevector <8 x i32> %20, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  %25 = add <4 x i32> %21, %14
  %26 = add <4 x i32> %22, %15
  %27 = add <4 x i32> %23, %12
  %28 = add <4 x i32> %24, %13
  %29 = add nuw i64 %11, 8
  %30 = icmp eq i64 %29, %9
  br i1 %30, label %31, label %10, !llvm.loop !11

31:                                               ; preds = %10
  %32 = add <4 x i32> %28, %27
  %33 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %32)
  %34 = add <4 x i32> %26, %25
  %35 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %34)
  %36 = icmp eq i64 %9, %6
  br i1 %36, label %41, label %37

37:                                               ; preds = %5, %31
  %38 = phi i64 [ 0, %5 ], [ %9, %31 ]
  %39 = phi i32 [ 0, %5 ], [ %33, %31 ]
  %40 = phi i32 [ 0, %5 ], [ %35, %31 ]
  br label %50

41:                                               ; preds = %50, %31, %1
  %42 = phi i32 [ 0, %1 ], [ %35, %31 ], [ %56, %50 ]
  %43 = phi i32 [ 0, %1 ], [ %33, %31 ], [ %59, %50 ]
  %44 = sdiv i32 %42, %3
  %45 = sdiv i32 %43, %3
  %46 = zext i32 %45 to i64
  %47 = shl nuw i64 %46, 32
  %48 = zext i32 %44 to i64
  %49 = or disjoint i64 %47, %48
  ret i64 %49

50:                                               ; preds = %37, %50
  %51 = phi i64 [ %60, %50 ], [ %38, %37 ]
  %52 = phi i32 [ %59, %50 ], [ %39, %37 ]
  %53 = phi i32 [ %56, %50 ], [ %40, %37 ]
  %54 = getelementptr inbounds [104 x %struct.point_s], ptr %0, i64 0, i64 %51
  %55 = load i32, ptr %54, align 4, !tbaa !15
  %56 = add nsw i32 %55, %53
  %57 = getelementptr inbounds i8, ptr %54, i64 4
  %58 = load i32, ptr %57, align 4, !tbaa !17
  %59 = add nsw i32 %58, %52
  %60 = add nuw nsw i64 %51, 1
  %61 = icmp eq i64 %60, %6
  br i1 %61, label %41, label %50, !llvm.loop !18
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: noreturn nounwind uwtable
define dso_local void @lloyd_relaxation() local_unnamed_addr #2 {
  %1 = alloca %struct.point_s, align 8
  br label %2

2:                                                ; preds = %2, %0
  %3 = phi i64 [ 0, %0 ], [ %16, %2 ]
  %4 = tail call i32 (...) @Rand() #6
  %5 = srem i32 %4, 720
  %6 = icmp slt i32 %5, 10
  %7 = tail call i32 @llvm.umin.i32(i32 %5, i32 710)
  %8 = select i1 %6, i32 10, i32 %7
  %9 = getelementptr inbounds [100 x %struct.point_s], ptr @generate_points.points, i64 0, i64 %3
  store i32 %8, ptr %9, align 8, !tbaa !15
  %10 = tail call i32 (...) @Rand() #6
  %11 = srem i32 %10, 720
  %12 = icmp slt i32 %11, 10
  %13 = tail call i32 @llvm.umin.i32(i32 %11, i32 710)
  %14 = select i1 %12, i32 10, i32 %13
  %15 = getelementptr inbounds i8, ptr %9, i64 4
  store i32 %14, ptr %15, align 4, !tbaa !17
  %16 = add nuw nsw i64 %3, 1
  %17 = icmp eq i64 %16, 100
  br i1 %17, label %18, label %2, !llvm.loop !19

18:                                               ; preds = %2
  %19 = getelementptr inbounds i8, ptr %1, i64 4
  br label %31

20:                                               ; preds = %53, %40
  %21 = phi i64 [ 0, %40 ], [ %61, %53 ]
  %22 = icmp eq i64 %42, 0
  br i1 %22, label %26, label %23

23:                                               ; preds = %20
  %24 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %32, i64 %21
  %25 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %21, i64 %41
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %24, ptr noundef nonnull align 4 dereferenceable(12) %25, i64 12, i1 false), !tbaa.struct !20
  br label %26

26:                                               ; preds = %23, %20, %38
  %27 = add nuw nsw i64 %33, 1
  br i1 %35, label %64, label %28

28:                                               ; preds = %374, %26
  %29 = phi i64 [ %34, %26 ], [ 0, %374 ]
  %30 = phi i64 [ %27, %26 ], [ 1, %374 ]
  br label %31, !llvm.loop !21

31:                                               ; preds = %28, %18
  %32 = phi i64 [ 0, %18 ], [ %29, %28 ]
  %33 = phi i64 [ 1, %18 ], [ %30, %28 ]
  %34 = add nuw nsw i64 %32, 1
  %35 = icmp eq i64 %34, 100
  br i1 %35, label %40, label %36

36:                                               ; preds = %31
  %37 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %32
  br label %46

38:                                               ; preds = %46
  %39 = icmp eq i64 %32, 0
  br i1 %39, label %26, label %40

40:                                               ; preds = %38, %31
  %41 = add nsw i64 %32, -1
  %42 = and i64 %32, 1
  %43 = icmp eq i64 %32, 1
  br i1 %43, label %20, label %44

44:                                               ; preds = %40
  %45 = and i64 %32, -2
  br label %53

46:                                               ; preds = %46, %36
  %47 = phi i64 [ %33, %36 ], [ %51, %46 ]
  %48 = add nsw i64 %47, -1
  %49 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %32, i64 %48
  %50 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %47
  call void @compute_normal(ptr noundef nonnull %49, ptr noundef nonnull %37, ptr noundef nonnull %50) #6
  %51 = add nuw nsw i64 %47, 1
  %52 = icmp eq i64 %51, 100
  br i1 %52, label %38, label %46, !llvm.loop !22

53:                                               ; preds = %53, %44
  %54 = phi i64 [ 0, %44 ], [ %61, %53 ]
  %55 = phi i64 [ 0, %44 ], [ %62, %53 ]
  %56 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %32, i64 %54
  %57 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %54, i64 %41
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %56, ptr noundef nonnull align 4 dereferenceable(12) %57, i64 12, i1 false), !tbaa.struct !20
  %58 = or disjoint i64 %54, 1
  %59 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %32, i64 %58
  %60 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %58, i64 %41
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %59, ptr noundef nonnull align 4 dereferenceable(12) %60, i64 12, i1 false), !tbaa.struct !20
  %61 = add nuw nsw i64 %54, 2
  %62 = add i64 %55, 2
  %63 = icmp eq i64 %62, %45
  br i1 %63, label %20, label %53, !llvm.loop !23

64:                                               ; preds = %26
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %1) #6
  br label %65

65:                                               ; preds = %207, %64
  %66 = phi i64 [ 0, %64 ], [ %208, %207 ]
  %67 = mul nuw nsw i64 %66, 99
  %68 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %67
  br label %80

69:                                               ; preds = %167
  %70 = sext i32 %168 to i64
  %71 = getelementptr inbounds %struct.tagged_point_s, ptr @get_cells.intersections, i64 %70
  %72 = icmp eq i32 %168, 0
  br i1 %72, label %73, label %77

73:                                               ; preds = %69
  %74 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %66, i32 1
  store i32 0, ptr %74, align 4, !tbaa !5
  %75 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %66
  %76 = load i64, ptr %75, align 8, !tbaa.struct !24
  store i64 %76, ptr @center, align 4, !tbaa.struct !24
  br label %183

77:                                               ; preds = %69
  %78 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %66
  %79 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %66
  br label %171

80:                                               ; preds = %167, %65
  %81 = phi i64 [ 0, %65 ], [ %87, %167 ]
  %82 = phi i64 [ 1, %65 ], [ %170, %167 ]
  %83 = phi i32 [ 4, %65 ], [ %168, %167 ]
  %84 = getelementptr inbounds %struct.line_s, ptr %68, i64 %81
  %85 = trunc nuw nsw i64 %81 to i32
  br label %89

86:                                               ; preds = %110
  %87 = add nuw nsw i64 %81, 1
  %88 = call zeroext i1 @lines_intersection(ptr noundef nonnull %84, ptr noundef nonnull @borders, ptr noundef nonnull %1) #6
  br i1 %88, label %114, label %125

89:                                               ; preds = %110, %80
  %90 = phi i64 [ %82, %80 ], [ %112, %110 ]
  %91 = phi i32 [ %83, %80 ], [ %111, %110 ]
  %92 = getelementptr inbounds %struct.line_s, ptr %68, i64 %90
  %93 = call zeroext i1 @lines_intersection(ptr noundef nonnull %84, ptr noundef nonnull %92, ptr noundef nonnull %1) #6
  br i1 %93, label %94, label %110

94:                                               ; preds = %89
  %95 = load i32, ptr %1, align 8, !tbaa !15
  %96 = add i32 %95, -1
  %97 = icmp ult i32 %96, 719
  br i1 %97, label %98, label %110

98:                                               ; preds = %94
  %99 = load i32, ptr %19, align 4, !tbaa !17
  %100 = add i32 %99, -1
  %101 = icmp ult i32 %100, 719
  br i1 %101, label %102, label %110

102:                                              ; preds = %98
  %103 = sext i32 %91 to i64
  %104 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %103
  %105 = load i64, ptr %1, align 8, !tbaa.struct !24
  store i64 %105, ptr %104, align 16, !tbaa.struct !24
  %106 = getelementptr inbounds i8, ptr %104, i64 8
  store i32 %85, ptr %106, align 8, !tbaa !25
  %107 = add nsw i32 %91, 1
  %108 = getelementptr inbounds i8, ptr %104, i64 12
  %109 = trunc nuw nsw i64 %90 to i32
  store i32 %109, ptr %108, align 4, !tbaa !27
  br label %110

110:                                              ; preds = %102, %98, %94, %89
  %111 = phi i32 [ %107, %102 ], [ %91, %98 ], [ %91, %94 ], [ %91, %89 ]
  %112 = add nuw nsw i64 %90, 1
  %113 = icmp eq i64 %112, 99
  br i1 %113, label %86, label %89, !llvm.loop !28

114:                                              ; preds = %86
  %115 = load i32, ptr %1, align 8, !tbaa !15
  %116 = add i32 %115, -1
  %117 = icmp ult i32 %116, 719
  br i1 %117, label %118, label %125

118:                                              ; preds = %114
  %119 = sext i32 %111 to i64
  %120 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %119
  %121 = load i64, ptr %1, align 8, !tbaa.struct !24
  store i64 %121, ptr %120, align 16, !tbaa.struct !24
  %122 = getelementptr inbounds i8, ptr %120, i64 8
  store i32 %85, ptr %122, align 8, !tbaa !25
  %123 = add nsw i32 %111, 1
  %124 = getelementptr inbounds i8, ptr %120, i64 12
  store i32 -4, ptr %124, align 4, !tbaa !27
  br label %125

125:                                              ; preds = %118, %114, %86
  %126 = phi i32 [ %123, %118 ], [ %111, %114 ], [ %111, %86 ]
  %127 = call zeroext i1 @lines_intersection(ptr noundef nonnull %84, ptr noundef nonnull getelementptr inbounds ([4 x %struct.line_s], ptr @borders, i64 0, i64 1), ptr noundef nonnull %1) #6
  br i1 %127, label %128, label %139

128:                                              ; preds = %125
  %129 = load i32, ptr %19, align 4, !tbaa !17
  %130 = add i32 %129, -1
  %131 = icmp ult i32 %130, 719
  br i1 %131, label %132, label %139

132:                                              ; preds = %128
  %133 = sext i32 %126 to i64
  %134 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %133
  %135 = load i64, ptr %1, align 8, !tbaa.struct !24
  store i64 %135, ptr %134, align 16, !tbaa.struct !24
  %136 = getelementptr inbounds i8, ptr %134, i64 8
  store i32 %85, ptr %136, align 8, !tbaa !25
  %137 = add nsw i32 %126, 1
  %138 = getelementptr inbounds i8, ptr %134, i64 12
  store i32 -3, ptr %138, align 4, !tbaa !27
  br label %139

139:                                              ; preds = %132, %128, %125
  %140 = phi i32 [ %137, %132 ], [ %126, %128 ], [ %126, %125 ]
  %141 = call zeroext i1 @lines_intersection(ptr noundef nonnull %84, ptr noundef nonnull getelementptr inbounds ([4 x %struct.line_s], ptr @borders, i64 0, i64 2), ptr noundef nonnull %1) #6
  br i1 %141, label %142, label %153

142:                                              ; preds = %139
  %143 = load i32, ptr %19, align 4, !tbaa !17
  %144 = add i32 %143, -1
  %145 = icmp ult i32 %144, 719
  br i1 %145, label %146, label %153

146:                                              ; preds = %142
  %147 = sext i32 %140 to i64
  %148 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %147
  %149 = load i64, ptr %1, align 8, !tbaa.struct !24
  store i64 %149, ptr %148, align 16, !tbaa.struct !24
  %150 = getelementptr inbounds i8, ptr %148, i64 8
  store i32 %85, ptr %150, align 8, !tbaa !25
  %151 = add nsw i32 %140, 1
  %152 = getelementptr inbounds i8, ptr %148, i64 12
  store i32 -2, ptr %152, align 4, !tbaa !27
  br label %153

153:                                              ; preds = %146, %142, %139
  %154 = phi i32 [ %151, %146 ], [ %140, %142 ], [ %140, %139 ]
  %155 = call zeroext i1 @lines_intersection(ptr noundef nonnull %84, ptr noundef nonnull getelementptr inbounds ([4 x %struct.line_s], ptr @borders, i64 0, i64 3), ptr noundef nonnull %1) #6
  br i1 %155, label %156, label %167

156:                                              ; preds = %153
  %157 = load i32, ptr %1, align 8, !tbaa !15
  %158 = add i32 %157, -1
  %159 = icmp ult i32 %158, 719
  br i1 %159, label %160, label %167

160:                                              ; preds = %156
  %161 = sext i32 %154 to i64
  %162 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %161
  %163 = load i64, ptr %1, align 8, !tbaa.struct !24
  store i64 %163, ptr %162, align 16, !tbaa.struct !24
  %164 = getelementptr inbounds i8, ptr %162, i64 8
  store i32 %85, ptr %164, align 8, !tbaa !25
  %165 = add nsw i32 %154, 1
  %166 = getelementptr inbounds i8, ptr %162, i64 12
  store i32 -1, ptr %166, align 4, !tbaa !27
  br label %167

167:                                              ; preds = %160, %156, %153
  %168 = phi i32 [ %165, %160 ], [ %154, %156 ], [ %154, %153 ]
  %169 = icmp eq i64 %87, 98
  %170 = add nuw nsw i64 %82, 1
  br i1 %169, label %69, label %80, !llvm.loop !29

171:                                              ; preds = %273, %77
  %172 = phi ptr [ @get_cells.intersections, %77 ], [ %275, %273 ]
  %173 = phi i32 [ 0, %77 ], [ %274, %273 ]
  %174 = getelementptr inbounds i8, ptr %172, i64 8
  %175 = getelementptr inbounds i8, ptr %172, i64 12
  %176 = load i32, ptr %174, align 4, !tbaa !25
  %177 = icmp eq i32 %176, -4
  %178 = load i32, ptr %175, align 4, !tbaa !27
  br i1 %177, label %221, label %210

179:                                              ; preds = %273
  %180 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %66, i32 1
  store i32 %274, ptr %180, align 4, !tbaa !5
  %181 = load i64, ptr %78, align 8, !tbaa.struct !24
  store i64 %181, ptr @center, align 4, !tbaa.struct !24
  %182 = icmp eq i32 %274, 1
  br i1 %182, label %207, label %183

183:                                              ; preds = %179, %73
  %184 = phi i32 [ 0, %73 ], [ %274, %179 ]
  %185 = zext i32 %184 to i64
  %186 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %66
  br label %187

187:                                              ; preds = %192, %183
  %188 = phi i64 [ 1, %183 ], [ %193, %192 ]
  %189 = trunc i64 %188 to i32
  %190 = sub i32 %184, %189
  %191 = zext i32 %190 to i64
  br label %195

192:                                              ; preds = %205
  %193 = add nuw nsw i64 %188, 1
  %194 = icmp eq i64 %193, %185
  br i1 %194, label %207, label %187, !llvm.loop !30

195:                                              ; preds = %205, %187
  %196 = phi i64 [ 0, %187 ], [ %198, %205 ]
  %197 = getelementptr inbounds %struct.point_s, ptr %186, i64 %196
  %198 = add nuw nsw i64 %196, 1
  %199 = getelementptr inbounds %struct.point_s, ptr %186, i64 %198
  %200 = call i32 @point_compare(ptr noundef nonnull %197, ptr noundef nonnull %199) #6
  %201 = icmp sgt i32 %200, 0
  br i1 %201, label %202, label %205

202:                                              ; preds = %195
  %203 = load <4 x i32>, ptr %197, align 4, !tbaa !10
  %204 = shufflevector <4 x i32> %203, <4 x i32> poison, <4 x i32> <i32 2, i32 3, i32 0, i32 1>
  store <4 x i32> %204, ptr %197, align 4, !tbaa !10
  br label %205

205:                                              ; preds = %202, %195
  %206 = icmp eq i64 %198, %191
  br i1 %206, label %192, label %195, !llvm.loop !31

207:                                              ; preds = %192, %179
  %208 = add nuw nsw i64 %66, 1
  %209 = icmp eq i64 %208, 100
  br i1 %209, label %277, label %65, !llvm.loop !32

210:                                              ; preds = %171
  %211 = icmp eq i32 %178, -4
  br i1 %211, label %217, label %212

212:                                              ; preds = %210
  %213 = call zeroext i1 @on_the_same_side(ptr noundef nonnull @borders, ptr noundef nonnull %78, ptr noundef nonnull %172) #6
  br i1 %213, label %214, label %273

214:                                              ; preds = %212
  %215 = load i32, ptr %174, align 4, !tbaa !25
  %216 = load i32, ptr %175, align 4, !tbaa !27
  br label %217

217:                                              ; preds = %214, %210
  %218 = phi i32 [ %216, %214 ], [ -4, %210 ]
  %219 = phi i32 [ %215, %214 ], [ %176, %210 ]
  %220 = icmp eq i32 %219, -3
  br i1 %220, label %234, label %221

221:                                              ; preds = %217, %171
  %222 = phi i32 [ %178, %171 ], [ %218, %217 ]
  %223 = phi i32 [ -4, %171 ], [ %219, %217 ]
  %224 = icmp eq i32 %222, -3
  br i1 %224, label %230, label %225

225:                                              ; preds = %221
  %226 = call zeroext i1 @on_the_same_side(ptr noundef nonnull getelementptr inbounds ([4 x %struct.line_s], ptr @borders, i64 0, i64 1), ptr noundef nonnull %78, ptr noundef nonnull %172) #6
  br i1 %226, label %227, label %273

227:                                              ; preds = %225
  %228 = load i32, ptr %174, align 4, !tbaa !25
  %229 = load i32, ptr %175, align 4, !tbaa !27
  br label %230

230:                                              ; preds = %227, %221
  %231 = phi i32 [ %229, %227 ], [ -3, %221 ]
  %232 = phi i32 [ %228, %227 ], [ %223, %221 ]
  %233 = icmp eq i32 %232, -2
  br i1 %233, label %247, label %234

234:                                              ; preds = %230, %217
  %235 = phi i32 [ %218, %217 ], [ %231, %230 ]
  %236 = phi i32 [ -3, %217 ], [ %232, %230 ]
  %237 = icmp eq i32 %235, -2
  br i1 %237, label %242, label %238

238:                                              ; preds = %234
  %239 = call zeroext i1 @on_the_same_side(ptr noundef nonnull getelementptr inbounds ([4 x %struct.line_s], ptr @borders, i64 0, i64 2), ptr noundef nonnull %78, ptr noundef nonnull %172) #6
  br i1 %239, label %240, label %273

240:                                              ; preds = %238
  %241 = load i32, ptr %174, align 4, !tbaa !25
  br label %242

242:                                              ; preds = %240, %234
  %243 = phi i32 [ %241, %240 ], [ %236, %234 ]
  %244 = icmp eq i32 %243, -1
  br i1 %244, label %252, label %245

245:                                              ; preds = %242
  %246 = load i32, ptr %175, align 4, !tbaa !27
  br label %247

247:                                              ; preds = %245, %230
  %248 = phi i32 [ %246, %245 ], [ %231, %230 ]
  %249 = icmp eq i32 %248, -1
  br i1 %249, label %252, label %250

250:                                              ; preds = %247
  %251 = call zeroext i1 @on_the_same_side(ptr noundef nonnull getelementptr inbounds ([4 x %struct.line_s], ptr @borders, i64 0, i64 3), ptr noundef nonnull %78, ptr noundef nonnull %172) #6
  br i1 %251, label %252, label %273

252:                                              ; preds = %250, %247, %242
  br label %253

253:                                              ; preds = %252, %265
  %254 = phi i64 [ %266, %265 ], [ 0, %252 ]
  %255 = load i32, ptr %174, align 4, !tbaa !25
  %256 = zext i32 %255 to i64
  %257 = icmp eq i64 %254, %256
  br i1 %257, label %265, label %258

258:                                              ; preds = %253
  %259 = load i32, ptr %175, align 4, !tbaa !27
  %260 = zext i32 %259 to i64
  %261 = icmp eq i64 %254, %260
  br i1 %261, label %265, label %262

262:                                              ; preds = %258
  %263 = getelementptr inbounds %struct.line_s, ptr %68, i64 %254
  %264 = call zeroext i1 @on_the_same_side(ptr noundef nonnull %263, ptr noundef nonnull %78, ptr noundef nonnull %172) #6
  br i1 %264, label %265, label %273

265:                                              ; preds = %262, %258, %253
  %266 = add nuw nsw i64 %254, 1
  %267 = icmp eq i64 %266, 99
  br i1 %267, label %268, label %253, !llvm.loop !33

268:                                              ; preds = %265
  %269 = add nsw i32 %173, 1
  %270 = sext i32 %173 to i64
  %271 = getelementptr inbounds [104 x %struct.point_s], ptr %79, i64 0, i64 %270
  %272 = load i64, ptr %172, align 4, !tbaa.struct !24
  store i64 %272, ptr %271, align 4, !tbaa.struct !24
  br label %273

273:                                              ; preds = %262, %268, %250, %238, %225, %212
  %274 = phi i32 [ %269, %268 ], [ %173, %250 ], [ %173, %238 ], [ %173, %225 ], [ %173, %212 ], [ %173, %262 ]
  %275 = getelementptr inbounds i8, ptr %172, i64 16
  %276 = icmp eq ptr %275, %71
  br i1 %276, label %179, label %171, !llvm.loop !34

277:                                              ; preds = %207
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %1) #6
  call void @set_color(i32 noundef 0, i32 noundef 0, i32 noundef 0) #6
  br label %278

278:                                              ; preds = %277, %278
  %279 = phi i64 [ 0, %277 ], [ %284, %278 ]
  %280 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %279
  %281 = load i32, ptr %280, align 8, !tbaa !15
  %282 = getelementptr inbounds i8, ptr %280, i64 4
  %283 = load i32, ptr %282, align 4, !tbaa !17
  call void @draw_point(i32 noundef %281, i32 noundef %283) #6
  %284 = add nuw nsw i64 %279, 1
  %285 = icmp eq i64 %284, 100
  br i1 %285, label %287, label %278, !llvm.loop !35

286:                                              ; preds = %296
  call void (...) @update_screen() #6
  br label %319

287:                                              ; preds = %278, %296
  %288 = phi i64 [ %305, %296 ], [ 0, %278 ]
  %289 = getelementptr inbounds %struct.polygon_s, ptr @get_cells.cells, i64 %288
  %290 = getelementptr inbounds i8, ptr %289, i64 832
  %291 = load i32, ptr %290, align 4, !tbaa !5
  %292 = add nsw i32 %291, -1
  %293 = icmp eq i32 %292, 0
  br i1 %293, label %296, label %294

294:                                              ; preds = %287
  %295 = zext i32 %292 to i64
  br label %307

296:                                              ; preds = %307, %287
  %297 = sext i32 %292 to i64
  %298 = getelementptr inbounds %struct.point_s, ptr %289, i64 %297
  %299 = load i32, ptr %298, align 4, !tbaa !15
  %300 = getelementptr inbounds i8, ptr %298, i64 4
  %301 = load i32, ptr %300, align 4, !tbaa !17
  %302 = load i32, ptr %289, align 4, !tbaa !15
  %303 = getelementptr inbounds i8, ptr %289, i64 4
  %304 = load i32, ptr %303, align 4, !tbaa !17
  call void @draw_line(i32 noundef %299, i32 noundef %301, i32 noundef %302, i32 noundef %304) #6
  %305 = add nuw nsw i64 %288, 1
  %306 = icmp eq i64 %305, 100
  br i1 %306, label %286, label %287, !llvm.loop !36

307:                                              ; preds = %294, %307
  %308 = phi i64 [ 0, %294 ], [ %313, %307 ]
  %309 = getelementptr inbounds %struct.point_s, ptr %289, i64 %308
  %310 = load i32, ptr %309, align 4, !tbaa !15
  %311 = getelementptr inbounds i8, ptr %309, i64 4
  %312 = load i32, ptr %311, align 4, !tbaa !17
  %313 = add nuw nsw i64 %308, 1
  %314 = getelementptr inbounds %struct.point_s, ptr %289, i64 %313
  %315 = load i32, ptr %314, align 4, !tbaa !15
  %316 = getelementptr inbounds i8, ptr %314, i64 4
  %317 = load i32, ptr %316, align 4, !tbaa !17
  call void @draw_line(i32 noundef %310, i32 noundef %312, i32 noundef %315, i32 noundef %317) #6
  %318 = icmp eq i64 %313, %295
  br i1 %318, label %296, label %307, !llvm.loop !37

319:                                              ; preds = %286, %374
  %320 = phi i64 [ 0, %286 ], [ %383, %374 ]
  %321 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %320
  %322 = getelementptr inbounds %struct.polygon_s, ptr @get_cells.cells, i64 %320
  %323 = getelementptr inbounds i8, ptr %322, i64 832
  %324 = load i32, ptr %323, align 4, !tbaa !5
  %325 = icmp eq i32 %324, 0
  br i1 %325, label %374, label %326

326:                                              ; preds = %319
  %327 = zext i32 %324 to i64
  %328 = icmp ult i32 %324, 8
  br i1 %328, label %358, label %329

329:                                              ; preds = %326
  %330 = and i64 %327, 4294967288
  br label %331

331:                                              ; preds = %331, %329
  %332 = phi i64 [ 0, %329 ], [ %350, %331 ]
  %333 = phi <4 x i32> [ zeroinitializer, %329 ], [ %348, %331 ]
  %334 = phi <4 x i32> [ zeroinitializer, %329 ], [ %349, %331 ]
  %335 = phi <4 x i32> [ zeroinitializer, %329 ], [ %346, %331 ]
  %336 = phi <4 x i32> [ zeroinitializer, %329 ], [ %347, %331 ]
  %337 = or disjoint i64 %332, 4
  %338 = getelementptr inbounds [104 x %struct.point_s], ptr %322, i64 0, i64 %332
  %339 = getelementptr inbounds [104 x %struct.point_s], ptr %322, i64 0, i64 %337
  %340 = load <8 x i32>, ptr %338, align 4, !tbaa !10
  %341 = load <8 x i32>, ptr %339, align 4, !tbaa !10
  %342 = shufflevector <8 x i32> %340, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  %343 = shufflevector <8 x i32> %341, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  %344 = shufflevector <8 x i32> %340, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  %345 = shufflevector <8 x i32> %341, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  %346 = add <4 x i32> %342, %335
  %347 = add <4 x i32> %343, %336
  %348 = add <4 x i32> %344, %333
  %349 = add <4 x i32> %345, %334
  %350 = add nuw i64 %332, 8
  %351 = icmp eq i64 %350, %330
  br i1 %351, label %352, label %331, !llvm.loop !38

352:                                              ; preds = %331
  %353 = add <4 x i32> %349, %348
  %354 = call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %353)
  %355 = add <4 x i32> %347, %346
  %356 = call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %355)
  %357 = icmp eq i64 %330, %327
  br i1 %357, label %374, label %358

358:                                              ; preds = %326, %352
  %359 = phi i64 [ 0, %326 ], [ %330, %352 ]
  %360 = phi i32 [ 0, %326 ], [ %354, %352 ]
  %361 = phi i32 [ 0, %326 ], [ %356, %352 ]
  br label %362

362:                                              ; preds = %358, %362
  %363 = phi i64 [ %372, %362 ], [ %359, %358 ]
  %364 = phi i32 [ %371, %362 ], [ %360, %358 ]
  %365 = phi i32 [ %368, %362 ], [ %361, %358 ]
  %366 = getelementptr inbounds [104 x %struct.point_s], ptr %322, i64 0, i64 %363
  %367 = load i32, ptr %366, align 4, !tbaa !15
  %368 = add nsw i32 %367, %365
  %369 = getelementptr inbounds i8, ptr %366, i64 4
  %370 = load i32, ptr %369, align 4, !tbaa !17
  %371 = add nsw i32 %370, %364
  %372 = add nuw nsw i64 %363, 1
  %373 = icmp eq i64 %372, %327
  br i1 %373, label %374, label %362, !llvm.loop !39

374:                                              ; preds = %362, %352, %319
  %375 = phi i32 [ 0, %319 ], [ %356, %352 ], [ %368, %362 ]
  %376 = phi i32 [ 0, %319 ], [ %354, %352 ], [ %371, %362 ]
  %377 = sdiv i32 %375, %324
  %378 = sdiv i32 %376, %324
  %379 = zext i32 %378 to i64
  %380 = shl nuw i64 %379, 32
  %381 = zext i32 %377 to i64
  %382 = or disjoint i64 %380, %381
  store i64 %382, ptr %321, align 8, !tbaa.struct !24
  %383 = add nuw nsw i64 %320, 1
  %384 = icmp eq i64 %383, 100
  br i1 %384, label %28, label %319, !llvm.loop !21
}

declare void @set_color(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #3

declare void @draw_point(i32 noundef, i32 noundef) local_unnamed_addr #3

declare void @draw_line(i32 noundef, i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #3

declare void @update_screen(...) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #4

declare i32 @Rand(...) local_unnamed_addr #3

declare zeroext i1 @lines_intersection(ptr noundef, ptr noundef, ptr noundef) local_unnamed_addr #3

declare zeroext i1 @on_the_same_side(ptr noundef, ptr noundef, ptr noundef) local_unnamed_addr #3

declare void @compute_normal(ptr noundef, ptr noundef, ptr noundef) local_unnamed_addr #3

declare i32 @point_compare(ptr noundef, ptr noundef) local_unnamed_addr #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #5

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #5

attributes #0 = { nofree norecurse nosync nounwind memory(argmem: read) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { noreturn nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #6 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 19.0.0 (++20240514042205+cff9e77783ac-1~exp1~20240514042338.1681)"}
!5 = !{!6, !9, i64 832}
!6 = !{!"polygon_s", !7, i64 0, !9, i64 832}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!"int", !7, i64 0}
!10 = !{!9, !9, i64 0}
!11 = distinct !{!11, !12, !13, !14}
!12 = !{!"llvm.loop.mustprogress"}
!13 = !{!"llvm.loop.isvectorized", i32 1}
!14 = !{!"llvm.loop.unroll.runtime.disable"}
!15 = !{!16, !9, i64 0}
!16 = !{!"point_s", !9, i64 0, !9, i64 4}
!17 = !{!16, !9, i64 4}
!18 = distinct !{!18, !12, !14, !13}
!19 = distinct !{!19, !12}
!20 = !{i64 0, i64 4, !10, i64 4, i64 4, !10, i64 8, i64 4, !10}
!21 = distinct !{!21, !12}
!22 = distinct !{!22, !12}
!23 = distinct !{!23, !12}
!24 = !{i64 0, i64 4, !10, i64 4, i64 4, !10}
!25 = !{!26, !9, i64 8}
!26 = !{!"tagged_point_s", !16, i64 0, !9, i64 8, !9, i64 12}
!27 = !{!26, !9, i64 12}
!28 = distinct !{!28, !12}
!29 = distinct !{!29, !12}
!30 = distinct !{!30, !12}
!31 = distinct !{!31, !12}
!32 = distinct !{!32, !12}
!33 = distinct !{!33, !12}
!34 = distinct !{!34, !12}
!35 = distinct !{!35, !12}
!36 = distinct !{!36, !12}
!37 = distinct !{!37, !12}
!38 = distinct !{!38, !12, !13, !14}
!39 = distinct !{!39, !12, !14, !13}
