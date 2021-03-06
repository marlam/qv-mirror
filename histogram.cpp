/*
 * Copyright (C) 2019 Computer Graphics Group, University of Siegen
 * Written by Martin Lambers <martin.lambers@uni-siegen.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cassert>
#include <cmath>

#include <omp.h>

#include "histogram.hpp"


Histogram::Histogram()
{
}

static int binIndexHelper(float v, float minVal, float maxVal, int bins)
{
    int b = (v - minVal) / (maxVal - minVal) * bins;
    if (b < 0)
        b = 0;
    else if (b >= bins)
        b = bins - 1;
    return b;
}

int Histogram::binIndex(float v) const
{
    return binIndexHelper(v, _minVal, _maxVal, _bins.size());
}

template<typename T, size_t BINS>
static void initHelper(const TAD::Array<T> array, size_t componentIndex,
        float _minVal, float _maxVal,
        std::vector<unsigned long long>& _bins, unsigned long long& _maxBinVal)
{
    _bins.resize(BINS, 0);
    size_t n = array.elementCount();
    size_t cc = array.componentCount();
    const T* data = array[0];

    #pragma omp parallel
    {
        int parts = omp_get_num_threads();
        size_t partSize = n / parts + (n % parts == 0 ? 0 : 1);
        int p = omp_get_thread_num();

        unsigned long long partBins[BINS];
        for (size_t b = 0; b < BINS; b++)
            partBins[b] = 0;

        for (size_t pe = 0; pe < partSize; pe++) {
            size_t e = p * partSize + pe;
            if (e >= n)
                break;
            T val = data[e * cc + componentIndex];
            if (std::isfinite(val)) {
                partBins[binIndexHelper(val, _minVal, _maxVal, BINS)]++;
            }
        }

        #pragma omp critical
        for (size_t b = 0; b < BINS; b++) {
            _bins[b] += partBins[b];
        }
    }

    _maxBinVal = _bins[0];
    for (size_t b = 1; b < BINS; b++) {
        if (_bins[b] > _maxBinVal)
            _maxBinVal = _bins[b];
    }
}

void Histogram::init(const TAD::ArrayContainer& array, size_t componentIndex, float minVal, float maxVal)
{
    assert(_bins.size() == 0);
    _minVal = minVal;
    _maxVal = maxVal;
    switch (array.componentType()) {
    case TAD::int8:
        initHelper<int8_t, 256>(TAD::Array<int8_t>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::uint8:
        initHelper<uint8_t, 256>(TAD::Array<uint8_t>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::int16:
        initHelper<int16_t, 1024>(TAD::Array<int16_t>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::uint16:
        initHelper<uint16_t, 1024>(TAD::Array<uint16_t>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::int32:
        initHelper<int32_t, 1024>(TAD::Array<int32_t>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::uint32:
        initHelper<uint32_t, 1024>(TAD::Array<uint32_t>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::int64:
        initHelper<int64_t, 1024>(TAD::Array<int64_t>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::uint64:
        initHelper<uint64_t, 1024>(TAD::Array<uint64_t>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::float32:
        initHelper<float, 1024>(TAD::Array<float>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    case TAD::float64:
        initHelper<double, 1024>(TAD::Array<double>(array), componentIndex, _minVal, _maxVal, _bins, _maxBinVal);
        break;
    }
}
