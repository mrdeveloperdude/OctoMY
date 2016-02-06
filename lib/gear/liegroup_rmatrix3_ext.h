// -------------------------------------------------------------------------------
// Copyright (c) 2012, Junggon Kim
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// -------------------------------------------------------------------------------

//================================================================================
//         Addon function for liegroup.h and rmatrix3j.h
//                                                               junggon@gmail.com
//================================================================================

#ifndef _ADDON_FUNCTIONS_FOR_LIEGROUP_AND_RMATRIX3_
#define _ADDON_FUNCTIONS_FOR_LIEGROUP_AND_RMATRIX3_

#include <math.h>
#include "liegroup.h"
#include "rmatrix3j.h"

RMatrix Ad(const SE3 &T, const RMatrix &J);					// return [Ad(T,J_1), ..., Ad(T,J_n)] where J = [J_1, ..., J_n] = 6 by n matrix
RMatrix dAd(const SE3 &T, const RMatrix &J);				// return [dAd(T,J_1), ..., dAd(T,J_n)] where J = [J_1, ..., J_n] = 6 by n matrix
RMatrix ad(const se3 &S, const RMatrix &J);					// return [ad(S,J_1), ..., ad(S,J_n)] where J = [J_1, ..., J_n] = 6 by n matrix
RMatrix ad(const RMatrix &S1, const RMatrix &S2);			// return ad(convert_to_se3(S1), S2)
RMatrix dad(const se3 &S, const RMatrix &J);				// return [dad(S,J_1), ..., dad(S,J_n)] where J = [J_1, ..., J_n] = 6 by n matrix
RMatrix Cross(const Vec3 &a, const RMatrix &B);				// return [Cross(a,b_1), ..., Cross(a,b_n)] where B = [b_1, ..., b_n] = 3 by n matrix

#include "liegroup_rmatrix3_ext.inl"

#endif
