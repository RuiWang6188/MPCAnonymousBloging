/***************************************************************************************************
 * Copyright (c) 2017-2018, NVIDIA CORPORATION.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of
 *       conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the names of its contributors may be used
 *       to endorse or promote products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TOR (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************************************/
/*! \file
    \brief Defies structural properties of single-precision GEMM.
*/
#pragma once

#include "cutlass/gemm/gemm.h"
#include "cutlass/gemm/gemm_epilogue.h"
#include "cutlass/gemm/gemm_epilogue_traits.h"
#include "cutlass/gemm/gemm_global_tile.h"
#include "cutlass/gemm/gemm_shared_tile.h"
#include "cutlass/gemm/gemm_traits.h"
#include "cutlass/gemm/thread_multiply_add.h"

namespace cutlass {
namespace gemm {

////////////////////////////////////////////////////////////////////////////////////////////////////

template <
    /// The tile size for the GEMM KxNxM.
    typename OutputTile_,
    /// Tile size for thread-level GEMM (K-by-N-by-M)
    typename ThreadGemmShape_,
    /// The number of scalars per LDG for A.
    int kScalarsPerLdgA_ = 1,
    /// The number of scalars per LDG for B.
    int kScalarsPerLdgB_ = 1,
    /// Whether to specify launch bounds
    bool kLaunchBounds = true>
struct SgemmConfig : public GemmConfig<
                         /// The scalar type for A.
                         float,
                         /// The scalar type for B.
                         float,
                         /// The scalar type for C.
                         float,
                         /// The scalar type for D.
                         float,
                         /// The tile size for the GEMM KxNxM.
                         OutputTile_,
                         /// The functor to do the math in the main loop.
                         ThreadMultiplyAdd<ThreadGemmShape_, Shape<1, 4, 8>, float, float, float>,
                         /// The number of scalars per LDG for A.
                         kScalarsPerLdgA_,
                         /// The number of scalars per STS for A.
                         kScalarsPerLdgA_,
                         /// The number of scalars per LDS for A.
                         4,
                         /// The number of scalars per LDG for B.
                         kScalarsPerLdgB_,
                         /// The number of scalars per STS for B.
                         kScalarsPerLdgB_,
                         /// The number of scalars per LDS for B.
                         4,
                         /// The number of scalars per LDG for C and STG for D.
                         1,
                         /// The number of scalars per STS for D.
                         4,
                         /// The number of scalars per LDS for D.
                         1,
                         /// The number of stages in shared memory.
                         2,
                         /// kResidueSeparate
                         false,
                         /// kResidueInPrologue
                         true,
                         /// kLaunchBounds
                         kLaunchBounds> {};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <
    /// The layout for A.
    MatrixLayout::Kind kLayoutA_,
    /// The layout for B.
    MatrixLayout::Kind kLayoutB_,
    /// The output tile.
    typename OutputTile_ = Shape<8, 128, 128>,
    /// The functor to use in the epilogue.
    typename EpilogueFunctor_ = LinearScaling<float>,
    /// Tile size for thread-level GEMM (K-by-N-by-M)
    typename ThreadGemmShape_ = Shape<8, 8, 8>,
    /// The number of floats loaded in one LDG for A.
    int kScalarsPerLdgA_ = 1,
    /// The number of floats loaded in one LDG for B.
    int kScalarsPerLdgB_ = 1,
    /// The index.
    typename Index_ = int,
    /// The SGEMM config.
    typename GemmConfig_ =
        SgemmConfig<OutputTile_, ThreadGemmShape_, kScalarsPerLdgA_, kScalarsPerLdgB_, false>,
    /// The traits class for the epilogue.
    typename GemmEpilogueTraits_ =
        SimplifiedGemmEpilogueTraits<GemmConfig_, EpilogueFunctor_, Index_> >
struct SgemmTraits : public SimplifiedGemmTraits<
                         // The layout for A.
                         kLayoutA_,
                         // The layout for B.
                         kLayoutB_,
                         // The config.
                         GemmConfig_,
                         // The epilogue.
                         GemmEpilogue<GemmEpilogueTraits_>,
                         // The index.
                         Index_> {};

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Helper to define SGEMM traits using Launch Bounds
template <
    /// The layout for A.
    MatrixLayout::Kind kLayoutA_,
    /// The layout for B.
    MatrixLayout::Kind kLayoutB_,
    /// The output tile.
    typename OutputTile_ = Shape<8, 128, 128>,
    /// The functor to use in the epilogue.
    typename EpilogueFunctor_ = LinearScaling<float>,
    /// Tile size for thread-level GEMM (K-by-N-by-M)
    typename ThreadGemmShape_ = Shape<8, 8, 8>,
    /// The number of floats loaded in one LDG for A.
    int kScalarsPerLdgA_ = 1,
    /// The number of floats loaded in one LDG for B.
    int kScalarsPerLdgB_ = 1,
    /// The index.
    typename Index_ = int,
    /// The SGEMM config.
    typename GemmConfig_ =
        SgemmConfig<OutputTile_, ThreadGemmShape_, kScalarsPerLdgA_, kScalarsPerLdgB_, true>,
    /// The traits class for the epilogue.
    typename GemmEpilogueTraits_ =
        SimplifiedGemmEpilogueTraits<GemmConfig_, EpilogueFunctor_, Index_> >
struct SgemmLBTraits : public SimplifiedGemmTraits<
                         // The layout for A.
                         kLayoutA_,
                         // The layout for B.
                         kLayoutB_,
                         // The config.
                         GemmConfig_,
                         // The epilogue.
                         GemmEpilogue<GemmEpilogueTraits_>,
                         // The index.
                         Index_> {};

////////////////////////////////////////////////////////////////////////////////////////////////////

}  // namespace gemm
}  // namespace cutlass
