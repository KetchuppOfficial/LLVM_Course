; ModuleID = '/home/ketchupp/Programming/llvm-course/00-graphics_application/src/lloyd_relaxation.c'
source_filename = "/home/ketchupp/Programming/llvm-course/00-graphics_application/src/lloyd_relaxation.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.point_s = type { i32, i32 }
%struct.polygon_s = type { [104 x %struct.point_s], i32 }
%struct.tagged_point_s = type { %struct.point_s, i32, i32 }
%struct.line_s = type { i32, i32, i32 }

@generate_points.points = internal unnamed_addr global [100 x %struct.point_s] zeroinitializer, align 16
@get_cells.cells = internal unnamed_addr global [100 x %struct.polygon_s] zeroinitializer, align 16
@get_cells.intersections = internal global <{ %struct.tagged_point_s, %struct.tagged_point_s, %struct.tagged_point_s, %struct.tagged_point_s, [5098 x %struct.tagged_point_s] }> <{ %struct.tagged_point_s { %struct.point_s zeroinitializer, i32 -4, i32 -3 }, %struct.tagged_point_s { %struct.point_s { i32 720, i32 0 }, i32 -4, i32 -2 }, %struct.tagged_point_s { %struct.point_s { i32 720, i32 720 }, i32 -2, i32 -1 }, %struct.tagged_point_s { %struct.point_s { i32 0, i32 720 }, i32 -1, i32 -3 }, [5098 x %struct.tagged_point_s] zeroinitializer }>, align 16
@center = internal unnamed_addr global %struct.point_s zeroinitializer, align 8
@compute_normals.perps = internal unnamed_addr global [100 x [99 x %struct.line_s]] zeroinitializer, align 16

; Function Attrs: noreturn nounwind uwtable
define dso_local void @lloyd_relaxation() local_unnamed_addr #0 {
  br label %1

1:                                                ; preds = %1, %0
  %2 = phi i64 [ 0, %0 ], [ %15, %1 ]
  %3 = tail call i32 (...) @Rand() #4
  %4 = srem i32 %3, 720
  %5 = icmp slt i32 %4, 10
  %6 = tail call i32 @llvm.umin.i32(i32 %4, i32 710)
  %7 = select i1 %5, i32 10, i32 %6
  %8 = getelementptr inbounds [100 x %struct.point_s], ptr @generate_points.points, i64 0, i64 %2
  store i32 %7, ptr %8, align 8, !tbaa !5
  %9 = tail call i32 (...) @Rand() #4
  %10 = srem i32 %9, 720
  %11 = icmp slt i32 %10, 10
  %12 = tail call i32 @llvm.umin.i32(i32 %10, i32 710)
  %13 = select i1 %11, i32 10, i32 %12
  %14 = getelementptr inbounds [100 x %struct.point_s], ptr @generate_points.points, i64 0, i64 %2, i32 1
  store i32 %13, ptr %14, align 4, !tbaa !10
  %15 = add nuw nsw i64 %2, 1
  %16 = icmp eq i64 %15, 100
  br i1 %16, label %28, label %1, !llvm.loop !11

17:                                               ; preds = %105, %77
  %18 = phi i64 [ 0, %77 ], [ %113, %105 ]
  %19 = icmp eq i64 %79, 0
  br i1 %19, label %23, label %20

20:                                               ; preds = %17
  %21 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %18
  %22 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %18, i64 %78
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %21, ptr noundef nonnull align 4 dereferenceable(12) %22, i64 12, i1 false), !tbaa.struct !13
  br label %23

23:                                               ; preds = %20, %17, %75
  %24 = add nuw nsw i64 %30, 1
  br i1 %33, label %116, label %25

25:                                               ; preds = %519, %23
  %26 = phi i64 [ %32, %23 ], [ 0, %519 ]
  %27 = phi i64 [ %24, %23 ], [ 1, %519 ]
  br label %28, !llvm.loop !15

28:                                               ; preds = %1, %25
  %29 = phi i64 [ %26, %25 ], [ 0, %1 ]
  %30 = phi i64 [ %27, %25 ], [ 1, %1 ]
  %31 = sub nsw i64 99, %29
  %32 = add nuw nsw i64 %29, 1
  %33 = icmp eq i64 %32, 100
  br i1 %33, label %77, label %34

34:                                               ; preds = %28
  %35 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %29
  %36 = load i32, ptr %35, align 8, !tbaa !5
  %37 = getelementptr i8, ptr %35, i64 4
  %38 = load i32, ptr %37, align 4, !tbaa !10
  %39 = icmp ult i64 %31, 4
  br i1 %39, label %73, label %40

40:                                               ; preds = %34
  %41 = and i64 %31, -4
  %42 = add i64 %30, %41
  %43 = insertelement <4 x i32> poison, i32 %36, i64 0
  %44 = shufflevector <4 x i32> %43, <4 x i32> poison, <4 x i32> zeroinitializer
  %45 = insertelement <4 x i32> poison, i32 %38, i64 0
  %46 = shufflevector <4 x i32> %45, <4 x i32> poison, <4 x i32> zeroinitializer
  br label %47

47:                                               ; preds = %47, %40
  %48 = phi i64 [ 0, %40 ], [ %69, %47 ]
  %49 = add i64 %30, %48
  %50 = add nsw i64 %49, -1
  %51 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %49
  %52 = load <8 x i32>, ptr %51, align 8, !tbaa !14
  %53 = shufflevector <8 x i32> %52, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  %54 = shufflevector <8 x i32> %52, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  %55 = add nsw <4 x i32> %53, %44
  %56 = sdiv <4 x i32> %55, <i32 -2, i32 -2, i32 -2, i32 -2>
  %57 = add nsw <4 x i32> %54, %46
  %58 = sdiv <4 x i32> %57, <i32 2, i32 2, i32 2, i32 2>
  %59 = sub nsw <4 x i32> %46, %54
  %60 = sub nsw <4 x i32> %53, %44
  %61 = sub nsw <4 x i32> zeroinitializer, %59
  %62 = mul nsw <4 x i32> %58, %59
  %63 = mul <4 x i32> %56, %60
  %64 = add <4 x i32> %62, %63
  %65 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %50
  %66 = shufflevector <4 x i32> %60, <4 x i32> %61, <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  %67 = shufflevector <4 x i32> %64, <4 x i32> poison, <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison>
  %68 = shufflevector <8 x i32> %66, <8 x i32> %67, <12 x i32> <i32 0, i32 4, i32 8, i32 1, i32 5, i32 9, i32 2, i32 6, i32 10, i32 3, i32 7, i32 11>
  store <12 x i32> %68, ptr %65, align 4, !tbaa !14
  %69 = add nuw i64 %48, 4
  %70 = icmp eq i64 %69, %41
  br i1 %70, label %71, label %47, !llvm.loop !16

71:                                               ; preds = %47
  %72 = icmp eq i64 %31, %41
  br i1 %72, label %75, label %73

73:                                               ; preds = %34, %71
  %74 = phi i64 [ %30, %34 ], [ %42, %71 ]
  br label %83

75:                                               ; preds = %83, %71
  %76 = icmp eq i64 %29, 0
  br i1 %76, label %23, label %77

77:                                               ; preds = %75, %28
  %78 = add nsw i64 %29, -1
  %79 = and i64 %29, 1
  %80 = icmp eq i64 %29, 1
  br i1 %80, label %17, label %81

81:                                               ; preds = %77
  %82 = and i64 %29, -2
  br label %105

83:                                               ; preds = %73, %83
  %84 = phi i64 [ %103, %83 ], [ %74, %73 ]
  %85 = add nsw i64 %84, -1
  %86 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %85
  %87 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %84
  %88 = load i32, ptr %87, align 8, !tbaa !5
  %89 = getelementptr i8, ptr %87, i64 4
  %90 = load i32, ptr %89, align 4, !tbaa !10
  %91 = add nsw i32 %88, %36
  %92 = sdiv i32 %91, -2
  %93 = add nsw i32 %90, %38
  %94 = sdiv i32 %93, 2
  %95 = sub nsw i32 %38, %90
  %96 = sub nsw i32 %88, %36
  store i32 %96, ptr %86, align 4, !tbaa !19
  %97 = sub nsw i32 0, %95
  %98 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %85, i32 1
  store i32 %97, ptr %98, align 4, !tbaa !21
  %99 = mul nsw i32 %94, %95
  %100 = mul i32 %92, %96
  %101 = add i32 %99, %100
  %102 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %85, i32 2
  store i32 %101, ptr %102, align 4, !tbaa !22
  %103 = add nuw nsw i64 %84, 1
  %104 = icmp eq i64 %103, 100
  br i1 %104, label %75, label %83, !llvm.loop !23

105:                                              ; preds = %105, %81
  %106 = phi i64 [ 0, %81 ], [ %113, %105 ]
  %107 = phi i64 [ 0, %81 ], [ %114, %105 ]
  %108 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %106
  %109 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %106, i64 %78
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %108, ptr noundef nonnull align 4 dereferenceable(12) %109, i64 12, i1 false), !tbaa.struct !13
  %110 = or disjoint i64 %106, 1
  %111 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %110
  %112 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %110, i64 %78
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %111, ptr noundef nonnull align 4 dereferenceable(12) %112, i64 12, i1 false), !tbaa.struct !13
  %113 = add nuw nsw i64 %106, 2
  %114 = add i64 %107, 2
  %115 = icmp eq i64 %114, %82
  br i1 %115, label %17, label %105, !llvm.loop !24

116:                                              ; preds = %23, %308
  %117 = phi i64 [ %309, %308 ], [ 0, %23 ]
  %118 = mul nuw nsw i64 %117, 99
  br label %127

119:                                              ; preds = %250
  %120 = sext i32 %251 to i64
  %121 = getelementptr inbounds %struct.tagged_point_s, ptr @get_cells.intersections, i64 %120
  %122 = icmp eq i32 %251, 0
  br i1 %122, label %264, label %123

123:                                              ; preds = %119
  %124 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %117
  %125 = getelementptr i8, ptr %124, i64 4
  %126 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %117
  br label %254

127:                                              ; preds = %250, %116
  %128 = phi i64 [ 0, %116 ], [ %139, %250 ]
  %129 = phi i64 [ 1, %116 ], [ %253, %250 ]
  %130 = phi i32 [ 4, %116 ], [ %251, %250 ]
  %131 = add nuw nsw i64 %128, %118
  %132 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %131
  %133 = load i32, ptr %132, align 4, !tbaa !19
  %134 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %131, i32 1
  %135 = load i32, ptr %134, align 4, !tbaa !21
  %136 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %131, i32 2
  %137 = trunc i64 %128 to i32
  br label %141

138:                                              ; preds = %181
  %139 = add nuw nsw i64 %128, 1
  %140 = icmp eq i32 %133, 0
  br i1 %140, label %197, label %185

141:                                              ; preds = %181, %127
  %142 = phi i64 [ %129, %127 ], [ %183, %181 ]
  %143 = phi i32 [ %130, %127 ], [ %182, %181 ]
  %144 = add nuw nsw i64 %142, %118
  %145 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %144
  %146 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %144, i32 1
  %147 = load i32, ptr %146, align 4, !tbaa !21
  %148 = mul nsw i32 %147, %133
  %149 = load i32, ptr %145, align 4, !tbaa !19
  %150 = mul nsw i32 %149, %135
  %151 = icmp eq i32 %148, %150
  br i1 %151, label %181, label %152

152:                                              ; preds = %141
  %153 = sub nsw i32 %148, %150
  %154 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %144, i32 2
  %155 = load i32, ptr %154, align 4, !tbaa !22
  %156 = mul nsw i32 %155, %135
  %157 = load i32, ptr %136, align 4, !tbaa !22
  %158 = mul nsw i32 %157, %147
  %159 = sub nsw i32 %156, %158
  %160 = sdiv i32 %159, %153
  %161 = mul nsw i32 %157, %149
  %162 = mul nsw i32 %155, %133
  %163 = sub nsw i32 %161, %162
  %164 = sdiv i32 %163, %153
  %165 = add i32 %160, -720
  %166 = icmp ult i32 %165, -719
  %167 = add i32 %164, -720
  %168 = icmp ult i32 %167, -719
  %169 = select i1 %166, i1 true, i1 %168
  br i1 %169, label %181, label %170

170:                                              ; preds = %152
  %171 = sext i32 %143 to i64
  %172 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %171
  %173 = zext nneg i32 %164 to i64
  %174 = shl nuw nsw i64 %173, 32
  %175 = zext nneg i32 %160 to i64
  %176 = or disjoint i64 %174, %175
  store i64 %176, ptr %172, align 16
  %177 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %171, i32 1
  store i32 %137, ptr %177, align 8, !tbaa !25
  %178 = add nsw i32 %143, 1
  %179 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %171, i32 2
  %180 = trunc i64 %142 to i32
  store i32 %180, ptr %179, align 4, !tbaa !27
  br label %181

181:                                              ; preds = %170, %152, %141
  %182 = phi i32 [ %178, %170 ], [ %143, %152 ], [ %143, %141 ]
  %183 = add nuw nsw i64 %142, 1
  %184 = icmp eq i64 %183, 99
  br i1 %184, label %138, label %141, !llvm.loop !28

185:                                              ; preds = %138
  %186 = load i32, ptr %136, align 4, !tbaa !22
  %187 = sdiv i32 %186, %133
  %188 = icmp ult i32 %187, -719
  br i1 %188, label %197, label %189

189:                                              ; preds = %185
  %190 = sub nsw i32 0, %187
  %191 = sext i32 %182 to i64
  %192 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %191
  %193 = zext nneg i32 %190 to i64
  store i64 %193, ptr %192, align 16
  %194 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %191, i32 1
  store i32 %137, ptr %194, align 8, !tbaa !25
  %195 = add nsw i32 %182, 1
  %196 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %191, i32 2
  store i32 -4, ptr %196, align 4, !tbaa !27
  br label %197

197:                                              ; preds = %189, %185, %138
  %198 = phi i32 [ %195, %189 ], [ %182, %185 ], [ %182, %138 ]
  %199 = icmp eq i32 %135, 0
  br i1 %199, label %233, label %200

200:                                              ; preds = %197
  %201 = sub nsw i32 0, %135
  %202 = load i32, ptr %136, align 4, !tbaa !22
  %203 = sdiv i32 %202, %201
  %204 = add i32 %203, -720
  %205 = icmp ult i32 %204, -719
  br i1 %205, label %214, label %206

206:                                              ; preds = %200
  %207 = sext i32 %198 to i64
  %208 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %207
  %209 = zext nneg i32 %203 to i64
  %210 = shl nuw nsw i64 %209, 32
  store i64 %210, ptr %208, align 16
  %211 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %207, i32 1
  store i32 %137, ptr %211, align 8, !tbaa !25
  %212 = add nsw i32 %198, 1
  %213 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %207, i32 2
  store i32 -3, ptr %213, align 4, !tbaa !27
  br label %214

214:                                              ; preds = %206, %200
  %215 = phi i32 [ %198, %200 ], [ %212, %206 ]
  %216 = mul i32 %133, 720
  %217 = add i32 %202, %216
  %218 = sdiv i32 %217, %201
  %219 = add i32 %218, -720
  %220 = icmp ult i32 %219, -719
  br i1 %220, label %233, label %221

221:                                              ; preds = %214
  %222 = mul nsw i32 %135, -720
  %223 = sdiv i32 %222, %201
  %224 = sext i32 %215 to i64
  %225 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %224
  %226 = zext nneg i32 %218 to i64
  %227 = shl nuw nsw i64 %226, 32
  %228 = zext i32 %223 to i64
  %229 = or disjoint i64 %227, %228
  store i64 %229, ptr %225, align 16
  %230 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %224, i32 1
  store i32 %137, ptr %230, align 8, !tbaa !25
  %231 = add nsw i32 %215, 1
  %232 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %224, i32 2
  store i32 -2, ptr %232, align 4, !tbaa !27
  br label %233

233:                                              ; preds = %221, %214, %197
  %234 = phi i32 [ %231, %221 ], [ %215, %214 ], [ %198, %197 ]
  br i1 %140, label %250, label %235

235:                                              ; preds = %233
  %236 = mul nsw i32 %135, -720
  %237 = load i32, ptr %136, align 4, !tbaa !22
  %238 = sub nsw i32 %236, %237
  %239 = sdiv i32 %238, %133
  %240 = add i32 %239, -720
  %241 = icmp ult i32 %240, -719
  br i1 %241, label %250, label %242

242:                                              ; preds = %235
  %243 = sext i32 %234 to i64
  %244 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %243
  %245 = zext nneg i32 %239 to i64
  %246 = or disjoint i64 %245, 3092376453120
  store i64 %246, ptr %244, align 16
  %247 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %243, i32 1
  store i32 %137, ptr %247, align 8, !tbaa !25
  %248 = add nsw i32 %234, 1
  %249 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %243, i32 2
  store i32 -1, ptr %249, align 4, !tbaa !27
  br label %250

250:                                              ; preds = %242, %235, %233
  %251 = phi i32 [ %248, %242 ], [ %234, %235 ], [ %234, %233 ]
  %252 = icmp eq i64 %139, 98
  %253 = add nuw nsw i64 %129, 1
  br i1 %252, label %119, label %127, !llvm.loop !29

254:                                              ; preds = %418, %123
  %255 = phi ptr [ @get_cells.intersections, %123 ], [ %420, %418 ]
  %256 = phi i32 [ 0, %123 ], [ %419, %418 ]
  %257 = getelementptr inbounds %struct.tagged_point_s, ptr %255, i64 0, i32 1
  %258 = load i32, ptr %257, align 4, !tbaa !25
  %259 = getelementptr inbounds %struct.tagged_point_s, ptr %255, i64 0, i32 2
  %260 = getelementptr i8, ptr %255, i64 4
  %261 = zext i32 %258 to i64
  %262 = icmp eq i32 %258, -4
  %263 = load i32, ptr %259, align 4, !tbaa !27
  br i1 %262, label %325, label %311

264:                                              ; preds = %418, %119
  %265 = phi i32 [ 0, %119 ], [ %419, %418 ]
  %266 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %117
  %267 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %117, i32 1
  store i32 %265, ptr %267, align 4, !tbaa !30
  %268 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %117
  %269 = load i64, ptr %268, align 8
  store i64 %269, ptr @center, align 8
  %270 = icmp eq i32 %265, 1
  br i1 %270, label %308, label %271

271:                                              ; preds = %264
  %272 = zext i32 %265 to i64
  br label %273

273:                                              ; preds = %278, %271
  %274 = phi i64 [ 1, %271 ], [ %279, %278 ]
  %275 = trunc i64 %274 to i32
  %276 = sub i32 %265, %275
  %277 = zext i32 %276 to i64
  br label %281

278:                                              ; preds = %306
  %279 = add nuw nsw i64 %274, 1
  %280 = icmp eq i64 %279, %272
  br i1 %280, label %308, label %273, !llvm.loop !32

281:                                              ; preds = %306, %273
  %282 = phi i64 [ 0, %273 ], [ %284, %306 ]
  %283 = getelementptr inbounds %struct.point_s, ptr %266, i64 %282
  %284 = add nuw nsw i64 %282, 1
  %285 = getelementptr inbounds %struct.point_s, ptr %266, i64 %284
  %286 = load i32, ptr %283, align 4, !tbaa !5
  %287 = getelementptr i8, ptr %283, i64 4
  %288 = load i32, ptr %287, align 4, !tbaa !10
  %289 = load i32, ptr getelementptr inbounds (%struct.point_s, ptr @center, i64 0, i32 1), align 4, !tbaa !10
  %290 = sub nsw i32 %288, %289
  %291 = load i32, ptr @center, align 8, !tbaa !5
  %292 = sub nsw i32 %286, %291
  %293 = tail call i32 @atan2_int(i32 noundef %290, i32 noundef %292) #4
  %294 = getelementptr inbounds %struct.point_s, ptr %266, i64 %284, i32 1
  %295 = load i32, ptr %294, align 4, !tbaa !10
  %296 = load i32, ptr getelementptr inbounds (%struct.point_s, ptr @center, i64 0, i32 1), align 4, !tbaa !10
  %297 = sub nsw i32 %295, %296
  %298 = load i32, ptr %285, align 4, !tbaa !5
  %299 = load i32, ptr @center, align 8, !tbaa !5
  %300 = sub nsw i32 %298, %299
  %301 = tail call i32 @atan2_int(i32 noundef %297, i32 noundef %300) #4
  %302 = icmp sgt i32 %293, %301
  br i1 %302, label %303, label %306

303:                                              ; preds = %281
  %304 = load <4 x i32>, ptr %283, align 4, !tbaa !14
  %305 = shufflevector <4 x i32> %304, <4 x i32> poison, <4 x i32> <i32 2, i32 3, i32 0, i32 1>
  store <4 x i32> %305, ptr %283, align 4, !tbaa !14
  br label %306

306:                                              ; preds = %303, %281
  %307 = icmp eq i64 %284, %277
  br i1 %307, label %278, label %281, !llvm.loop !33

308:                                              ; preds = %278, %264
  %309 = add nuw nsw i64 %117, 1
  %310 = icmp eq i64 %309, 100
  br i1 %310, label %422, label %116, !llvm.loop !34

311:                                              ; preds = %254
  %312 = icmp eq i32 %263, -4
  br i1 %312, label %323, label %313

313:                                              ; preds = %311
  %314 = load i32, ptr %125, align 4, !tbaa !10
  %315 = load i32, ptr %260, align 4, !tbaa !10
  %316 = icmp sgt i32 %314, 0
  %317 = icmp sgt i32 %315, 0
  %318 = select i1 %316, i1 %317, i1 false
  %319 = icmp slt i32 %314, 0
  %320 = icmp slt i32 %315, 0
  %321 = select i1 %319, i1 %320, i1 false
  %322 = select i1 %318, i1 true, i1 %321
  br i1 %322, label %323, label %418

323:                                              ; preds = %313, %311
  %324 = icmp eq i32 %258, -3
  br i1 %324, label %339, label %325

325:                                              ; preds = %323, %254
  %326 = icmp eq i32 %263, -3
  br i1 %326, label %337, label %327

327:                                              ; preds = %325
  %328 = load i32, ptr %124, align 8, !tbaa !5
  %329 = load i32, ptr %255, align 4, !tbaa !5
  %330 = icmp sgt i32 %328, 0
  %331 = icmp sgt i32 %329, 0
  %332 = select i1 %330, i1 %331, i1 false
  %333 = icmp slt i32 %328, 0
  %334 = icmp slt i32 %329, 0
  %335 = select i1 %333, i1 %334, i1 false
  %336 = select i1 %332, i1 true, i1 %335
  br i1 %336, label %337, label %418

337:                                              ; preds = %327, %325
  %338 = icmp eq i32 %258, -2
  br i1 %338, label %355, label %339

339:                                              ; preds = %337, %323
  %340 = icmp eq i32 %263, -2
  br i1 %340, label %351, label %341

341:                                              ; preds = %339
  %342 = load i32, ptr %124, align 8, !tbaa !5
  %343 = load i32, ptr %255, align 4, !tbaa !5
  %344 = icmp sgt i32 %342, 720
  %345 = icmp sgt i32 %343, 720
  %346 = select i1 %344, i1 %345, i1 false
  %347 = icmp slt i32 %342, 720
  %348 = icmp slt i32 %343, 720
  %349 = select i1 %347, i1 %348, i1 false
  %350 = select i1 %346, i1 true, i1 %349
  br i1 %350, label %351, label %418

351:                                              ; preds = %341, %339
  %352 = icmp eq i32 %258, -1
  %353 = icmp eq i32 %263, -1
  %354 = select i1 %352, i1 true, i1 %353
  br i1 %354, label %367, label %357

355:                                              ; preds = %337
  %356 = icmp eq i32 %263, -1
  br i1 %356, label %367, label %357

357:                                              ; preds = %351, %355
  %358 = load i32, ptr %125, align 4, !tbaa !10
  %359 = load i32, ptr %260, align 4, !tbaa !10
  %360 = icmp sgt i32 %358, 720
  %361 = icmp sgt i32 %359, 720
  %362 = select i1 %360, i1 %361, i1 false
  %363 = icmp slt i32 %358, 720
  %364 = icmp slt i32 %359, 720
  %365 = select i1 %363, i1 %364, i1 false
  %366 = select i1 %362, i1 true, i1 %365
  br i1 %366, label %367, label %418

367:                                              ; preds = %357, %355, %351
  %368 = zext i32 %263 to i64
  br label %369

369:                                              ; preds = %410, %367
  %370 = phi i64 [ 0, %367 ], [ %411, %410 ]
  %371 = icmp eq i64 %370, %261
  %372 = icmp eq i64 %370, %368
  %373 = select i1 %371, i1 true, i1 %372
  br i1 %373, label %410, label %374

374:                                              ; preds = %369
  %375 = add nuw nsw i64 %370, %118
  %376 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %375
  %377 = load i32, ptr %124, align 8, !tbaa !5
  %378 = load i32, ptr %125, align 4, !tbaa !10
  %379 = load i32, ptr %255, align 4, !tbaa !5
  %380 = load i32, ptr %260, align 4, !tbaa !10
  %381 = load i32, ptr %376, align 4, !tbaa !19
  %382 = mul nsw i32 %381, %381
  %383 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %375, i32 1
  %384 = load i32, ptr %383, align 4, !tbaa !21
  %385 = mul nsw i32 %384, %384
  %386 = add nuw nsw i32 %385, %382
  %387 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %375, i32 2
  %388 = load i32, ptr %387, align 4, !tbaa !22
  %389 = mul nsw i32 %388, %381
  %390 = sdiv i32 %389, %386
  %391 = mul nsw i32 %388, %384
  %392 = sdiv i32 %391, %386
  %393 = add nsw i32 %390, %377
  %394 = mul nsw i32 %393, %381
  %395 = add nsw i32 %392, %378
  %396 = mul nsw i32 %395, %384
  %397 = add nsw i32 %396, %394
  %398 = add nsw i32 %390, %379
  %399 = mul nsw i32 %398, %381
  %400 = add nsw i32 %392, %380
  %401 = mul nsw i32 %400, %384
  %402 = add nsw i32 %401, %399
  %403 = icmp sgt i32 %397, 0
  %404 = icmp sgt i32 %402, 0
  %405 = select i1 %403, i1 %404, i1 false
  %406 = icmp slt i32 %397, 0
  %407 = icmp slt i32 %402, 0
  %408 = select i1 %406, i1 %407, i1 false
  %409 = select i1 %405, i1 true, i1 %408
  br i1 %409, label %410, label %418

410:                                              ; preds = %374, %369
  %411 = add nuw nsw i64 %370, 1
  %412 = icmp eq i64 %411, 99
  br i1 %412, label %413, label %369, !llvm.loop !35

413:                                              ; preds = %410
  %414 = add nsw i32 %256, 1
  %415 = sext i32 %256 to i64
  %416 = getelementptr inbounds [104 x %struct.point_s], ptr %126, i64 0, i64 %415
  %417 = load i64, ptr %255, align 4
  store i64 %417, ptr %416, align 4
  br label %418

418:                                              ; preds = %374, %413, %357, %341, %327, %313
  %419 = phi i32 [ %414, %413 ], [ %256, %357 ], [ %256, %341 ], [ %256, %327 ], [ %256, %313 ], [ %256, %374 ]
  %420 = getelementptr inbounds %struct.tagged_point_s, ptr %255, i64 1
  %421 = icmp eq ptr %420, %121
  br i1 %421, label %264, label %254, !llvm.loop !36

422:                                              ; preds = %308
  tail call void @set_color(i32 noundef 0, i32 noundef 0, i32 noundef 0) #4
  br label %423

423:                                              ; preds = %422, %423
  %424 = phi i64 [ 0, %422 ], [ %429, %423 ]
  %425 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %424
  %426 = load i32, ptr %425, align 8, !tbaa !5
  %427 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %424, i32 1
  %428 = load i32, ptr %427, align 4, !tbaa !10
  tail call void @draw_point(i32 noundef %426, i32 noundef %428) #4
  %429 = add nuw nsw i64 %424, 1
  %430 = icmp eq i64 %429, 100
  br i1 %430, label %432, label %423, !llvm.loop !37

431:                                              ; preds = %441
  tail call void (...) @update_screen() #4
  br label %464

432:                                              ; preds = %423, %441
  %433 = phi i64 [ %450, %441 ], [ 0, %423 ]
  %434 = getelementptr inbounds %struct.polygon_s, ptr @get_cells.cells, i64 %433
  %435 = getelementptr inbounds %struct.polygon_s, ptr @get_cells.cells, i64 %433, i32 1
  %436 = load i32, ptr %435, align 4, !tbaa !30
  %437 = add nsw i32 %436, -1
  %438 = icmp eq i32 %437, 0
  br i1 %438, label %441, label %439

439:                                              ; preds = %432
  %440 = zext i32 %437 to i64
  br label %452

441:                                              ; preds = %452, %432
  %442 = sext i32 %437 to i64
  %443 = getelementptr inbounds %struct.point_s, ptr %434, i64 %442
  %444 = load i32, ptr %443, align 4, !tbaa !5
  %445 = getelementptr inbounds %struct.point_s, ptr %434, i64 %442, i32 1
  %446 = load i32, ptr %445, align 4, !tbaa !10
  %447 = load i32, ptr %434, align 4, !tbaa !5
  %448 = getelementptr inbounds %struct.point_s, ptr %434, i64 0, i32 1
  %449 = load i32, ptr %448, align 4, !tbaa !10
  tail call void @draw_line(i32 noundef %444, i32 noundef %446, i32 noundef %447, i32 noundef %449) #4
  %450 = add nuw nsw i64 %433, 1
  %451 = icmp eq i64 %450, 100
  br i1 %451, label %431, label %432, !llvm.loop !38

452:                                              ; preds = %439, %452
  %453 = phi i64 [ 0, %439 ], [ %458, %452 ]
  %454 = getelementptr inbounds %struct.point_s, ptr %434, i64 %453
  %455 = load i32, ptr %454, align 4, !tbaa !5
  %456 = getelementptr inbounds %struct.point_s, ptr %434, i64 %453, i32 1
  %457 = load i32, ptr %456, align 4, !tbaa !10
  %458 = add nuw nsw i64 %453, 1
  %459 = getelementptr inbounds %struct.point_s, ptr %434, i64 %458
  %460 = load i32, ptr %459, align 4, !tbaa !5
  %461 = getelementptr inbounds %struct.point_s, ptr %434, i64 %458, i32 1
  %462 = load i32, ptr %461, align 4, !tbaa !10
  tail call void @draw_line(i32 noundef %455, i32 noundef %457, i32 noundef %460, i32 noundef %462) #4
  %463 = icmp eq i64 %458, %440
  br i1 %463, label %441, label %452, !llvm.loop !39

464:                                              ; preds = %431, %519
  %465 = phi i64 [ 0, %431 ], [ %528, %519 ]
  %466 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %465
  %467 = getelementptr inbounds %struct.polygon_s, ptr @get_cells.cells, i64 %465
  %468 = getelementptr inbounds %struct.polygon_s, ptr @get_cells.cells, i64 %465, i32 1
  %469 = load i32, ptr %468, align 4, !tbaa !30
  %470 = icmp eq i32 %469, 0
  br i1 %470, label %519, label %471

471:                                              ; preds = %464
  %472 = zext i32 %469 to i64
  %473 = icmp ult i32 %469, 8
  br i1 %473, label %503, label %474

474:                                              ; preds = %471
  %475 = and i64 %472, 4294967288
  br label %476

476:                                              ; preds = %476, %474
  %477 = phi i64 [ 0, %474 ], [ %495, %476 ]
  %478 = phi <4 x i32> [ zeroinitializer, %474 ], [ %493, %476 ]
  %479 = phi <4 x i32> [ zeroinitializer, %474 ], [ %494, %476 ]
  %480 = phi <4 x i32> [ zeroinitializer, %474 ], [ %491, %476 ]
  %481 = phi <4 x i32> [ zeroinitializer, %474 ], [ %492, %476 ]
  %482 = or disjoint i64 %477, 4
  %483 = getelementptr inbounds [104 x %struct.point_s], ptr %467, i64 0, i64 %477
  %484 = getelementptr inbounds [104 x %struct.point_s], ptr %467, i64 0, i64 %482
  %485 = load <8 x i32>, ptr %483, align 4, !tbaa !14
  %486 = load <8 x i32>, ptr %484, align 4, !tbaa !14
  %487 = shufflevector <8 x i32> %485, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  %488 = shufflevector <8 x i32> %486, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  %489 = shufflevector <8 x i32> %485, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  %490 = shufflevector <8 x i32> %486, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  %491 = add <4 x i32> %487, %480
  %492 = add <4 x i32> %488, %481
  %493 = add <4 x i32> %489, %478
  %494 = add <4 x i32> %490, %479
  %495 = add nuw i64 %477, 8
  %496 = icmp eq i64 %495, %475
  br i1 %496, label %497, label %476, !llvm.loop !40

497:                                              ; preds = %476
  %498 = add <4 x i32> %494, %493
  %499 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %498)
  %500 = add <4 x i32> %492, %491
  %501 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %500)
  %502 = icmp eq i64 %475, %472
  br i1 %502, label %519, label %503

503:                                              ; preds = %471, %497
  %504 = phi i64 [ 0, %471 ], [ %475, %497 ]
  %505 = phi i32 [ 0, %471 ], [ %499, %497 ]
  %506 = phi i32 [ 0, %471 ], [ %501, %497 ]
  br label %507

507:                                              ; preds = %503, %507
  %508 = phi i64 [ %517, %507 ], [ %504, %503 ]
  %509 = phi i32 [ %516, %507 ], [ %505, %503 ]
  %510 = phi i32 [ %513, %507 ], [ %506, %503 ]
  %511 = getelementptr inbounds [104 x %struct.point_s], ptr %467, i64 0, i64 %508
  %512 = load i32, ptr %511, align 4, !tbaa !5
  %513 = add nsw i32 %512, %510
  %514 = getelementptr inbounds [104 x %struct.point_s], ptr %467, i64 0, i64 %508, i32 1
  %515 = load i32, ptr %514, align 4, !tbaa !10
  %516 = add nsw i32 %515, %509
  %517 = add nuw nsw i64 %508, 1
  %518 = icmp eq i64 %517, %472
  br i1 %518, label %519, label %507, !llvm.loop !41

519:                                              ; preds = %507, %497, %464
  %520 = phi i32 [ 0, %464 ], [ %501, %497 ], [ %513, %507 ]
  %521 = phi i32 [ 0, %464 ], [ %499, %497 ], [ %516, %507 ]
  %522 = sdiv i32 %520, %469
  %523 = sdiv i32 %521, %469
  %524 = zext i32 %523 to i64
  %525 = shl nuw i64 %524, 32
  %526 = zext i32 %522 to i64
  %527 = or disjoint i64 %525, %526
  store i64 %527, ptr %466, align 8, !tbaa.struct !42
  %528 = add nuw nsw i64 %465, 1
  %529 = icmp eq i64 %528, 100
  br i1 %529, label %25, label %464, !llvm.loop !15
}

declare void @set_color(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #1

declare void @draw_point(i32 noundef, i32 noundef) local_unnamed_addr #1

declare void @draw_line(i32 noundef, i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #1

declare void @update_screen(...) local_unnamed_addr #1

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #2

declare i32 @Rand(...) local_unnamed_addr #1

declare i32 @atan2_int(i32 noundef, i32 noundef) local_unnamed_addr #1

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #3

attributes #0 = { noreturn nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #4 = { nounwind }

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
!13 = !{i64 0, i64 4, !14, i64 4, i64 4, !14, i64 8, i64 4, !14}
!14 = !{!7, !7, i64 0}
!15 = distinct !{!15, !12}
!16 = distinct !{!16, !12, !17, !18}
!17 = !{!"llvm.loop.isvectorized", i32 1}
!18 = !{!"llvm.loop.unroll.runtime.disable"}
!19 = !{!20, !7, i64 0}
!20 = !{!"line_s", !7, i64 0, !7, i64 4, !7, i64 8}
!21 = !{!20, !7, i64 4}
!22 = !{!20, !7, i64 8}
!23 = distinct !{!23, !12, !18, !17}
!24 = distinct !{!24, !12}
!25 = !{!26, !7, i64 8}
!26 = !{!"tagged_point_s", !6, i64 0, !7, i64 8, !7, i64 12}
!27 = !{!26, !7, i64 12}
!28 = distinct !{!28, !12}
!29 = distinct !{!29, !12}
!30 = !{!31, !7, i64 832}
!31 = !{!"polygon_s", !8, i64 0, !7, i64 832}
!32 = distinct !{!32, !12}
!33 = distinct !{!33, !12}
!34 = distinct !{!34, !12}
!35 = distinct !{!35, !12}
!36 = distinct !{!36, !12}
!37 = distinct !{!37, !12}
!38 = distinct !{!38, !12}
!39 = distinct !{!39, !12}
!40 = distinct !{!40, !12, !17, !18}
!41 = distinct !{!41, !12, !18, !17}
!42 = !{i64 0, i64 4, !14, i64 4, i64 4, !14}
