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

#include "histogram.hpp"


Histogram::Histogram()
{
}

int Histogram::binIndex(float v) const
{
    long b = (v - _minVal) / (_maxVal - _minVal) * binCount();
    if (b < 0)
        b = 0;
    else if (b >= binCount())
        b = binCount() - 1;
    return b;
}

void Histogram::init(const TAD::Array<uint8_t>& array, size_t componentIndex)
{
    assert(_bins.size() == 0);
    _bins.resize(256);
    _minVal = 0.0;
    _maxVal = 255.0f;
    for (size_t e = 0; e < array.elementCount(); e++) {
        uint8_t val = array.get<uint8_t>(e, componentIndex);
        _bins[val]++;
    }
    _maxBinVal = 0;
    for (size_t i = 0; i < _bins.size(); i++)
        if (_bins[i] > _maxBinVal)
            _maxBinVal = _bins[i];
}

void Histogram::init(const TAD::Array<float>& array, size_t componentIndex, float minVal, float maxVal)
{
    assert(_bins.size() == 0);
    _bins.resize(1024);
    _minVal = minVal;
    _maxVal = maxVal;
    for (size_t e = 0; e < array.elementCount(); e++) {
        float val = array.get<float>(e, componentIndex);
        if (std::isfinite(val)) {
            _bins[binIndex(val)]++;
        }
    }
    _maxBinVal = 0;
    for (size_t i = 0; i < _bins.size(); i++)
        if (_bins[i] > _maxBinVal)
            _maxBinVal = _bins[i];
}
